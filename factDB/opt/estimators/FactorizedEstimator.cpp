// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/estimators/FactorizedEstimator.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/JoinMode.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Reference.hpp"
#include "factDB/algebra/visitors/GraphvizQueryPlan.hpp"
#include "factDB/infra/BitSet.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/opt/CardinalityOracle.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/opt/PredecessorDetector.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/algs/OptimizerAlgorithms.hpp"
#include "factDB/opt/estimators/DummyEstimator.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
#include "factDB/util/DoOnDestruction.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
infra::BitSetVar FactorizedEstimator::getInterestingClasses(const infra::BitSet64& coveredRelations) const {
   // collect the interesting equivalence classes.
   // i.e. collect the classes which are currently only partially covered. The size of these classes is actually interesting for the joins
   infra::BitSetVar usedClasses(queryGraph.maxEquivalenceClasses()), unusedClasses(queryGraph.maxEquivalenceClasses());
   for (uint32_t relation = 0; relation != queryGraph.getRelations().size(); ++relation) {
      auto& curBitSet = coveredRelations.contains(relation) ? usedClasses : unusedClasses;
      curBitSet += queryGraph.getIUEquivalenceClasses(relation);
   }
   return usedClasses & unusedClasses;
}
// ---------------------------------------------------------------------------------------------------
bool FactorizedEstimator::comparable(const Plan& p1, const Plan& p2) const {
   auto p1intersting = getInterestingClasses(p1.getCoveredRelations());
   auto p2intersting = getInterestingClasses(p2.getCoveredRelations());

   // p1 can be compared to p2 => p1 is better iff it is smaller for all interesting relations
   // => p2 could be dropped in this case

   // the other direction does not hold: p2 is not necessary comparable to p1 since it may contain some unbounded variables todo?
   return p1intersting.isSubsetOf(p2intersting);
}

// ---------------------------------------------------------------------------------------------------
Estimate* FactorizedEstimator::estimateBaseTablePlan(const BaseTablePlan&) {
   return &estimates.emplace_back();
}
// ---------------------------------------------------------------------------------------------------
struct SubgraphHelperEstimator : public DummyEstimator {
   const infra::BitSet64& focusedRelations;

   SubgraphHelperEstimator(const QueryGraph& qg, Database& db, const infra::BitSet64& requiredRelations_)
      : DummyEstimator(qg, db), focusedRelations(requiredRelations_) {}

   const IU* getFocusIU(const IUSet& ius, const IU* defaultIU) const {
      for (const IU* iu : ius)
         if (focusedRelations.contains(queryGraph.getRelation(iu)))
            return iu;
      return defaultIU;
   }

   JoinCondition optimizeJoiningRelations(std::unique_ptr<algebra::Operator>&, std::unique_ptr<algebra::Operator>&, const IUSet& leftIUs, const IUSet& rightIUs) const override {
      // generates the join edges such that edges within the focus relations are preferred. If this is not possible, it prefers outgoing edges.
      { // input validation
         assert(!leftIUs.empty() && !rightIUs.empty());
         [[maybe_unused]] const auto equivClass = queryGraph.getEquivalenceClass(*(*leftIUs.begin()));
         for ([[maybe_unused]] auto& iu : leftIUs)
            assert(queryGraph.getEquivalenceClass(*iu) == equivClass);
         for ([[maybe_unused]] auto& iu : rightIUs)
            assert(queryGraph.getEquivalenceClass(*iu) == equivClass);
      }

      const IU* leftIU = getFocusIU(leftIUs, *leftIUs.begin());
      const IU* rightIU = getFocusIU(rightIUs, *rightIUs.begin());

      return JoinCondition::create(leftIU, rightIU);
   }
};
// ---------------------------------------------------------------------------------------------------
uint64_t FactorizedEstimator::executeSemiJoinPlan(const Plan& currentPlan, const infra::BitSet64& requiredRelations, const infra::BitSet64& semiJoinedRelations) {
   assert(!requiredRelations.doesIntersectWith(semiJoinedRelations));
   assert((requiredRelations + semiJoinedRelations).isSubsetOf(infra::BitSet64(0, queryGraph.getRelations().size(), infra::BitSet64::RangeEnum::Range)));

   if (requiredRelations.size() == 0)
      return 0;
   if (semiJoinedRelations.empty()) // if no semi-joins are introduced, we can just execute the current plan.
      return oracle.estimate(&currentPlan, queryGraph, *this);

   SubgraphHelperEstimator subgraphHelperEstimator(queryGraph, oracle.getDatabase(), requiredRelations);
   auto algebraTree = currentPlan.generateOperatorPlan(queryGraph, subgraphHelperEstimator);
   auto curQueryGraph = QueryGraph::generateQueryGraph(queryGraph.getDatabase(), *algebraTree);
   // we have a plan only of relevant relations, map input relations to relations in curQueryGraph

   std::deque<JoinConditionList> joinConditionList;
   std::vector<JoinInfos> joinInfos;

   // update requiredRelations & semiJoinedRelations for new QueryGraph
   infra::BitSet64 newRequiredRelations, newSemiJoinedRelations;
   {
      std::unordered_map<std::string_view, unsigned> relation2idx;
      for (size_t idx = 0; idx != curQueryGraph.getRelations().size(); ++idx)
         relation2idx[curQueryGraph.getRelation(idx).alias] = idx;

      for (auto idx : requiredRelations) {
         const auto& curRelation = queryGraph.getRelation(idx);
         assert(relation2idx.contains(curRelation.alias));
         newRequiredRelations.insert(relation2idx[curRelation.alias]);
      }
      for (auto idx : semiJoinedRelations) {
         const auto& curRelation = queryGraph.getRelation(idx);
         assert(relation2idx.contains(curRelation.alias));
         newSemiJoinedRelations.insert(relation2idx[curRelation.alias]);
      }
   }

   infra::UnionFindBase connectedRelations(curQueryGraph.getRelations().size());
   std::vector<OptimizerJoin> joins, changedJoins;
   std::vector<JoinConditionList*> id2jcl;
   id2jcl.resize(curQueryGraph.getJoins().size());
   for (const auto& join : curQueryGraph.getJoins()) {
      auto neededForJoin = join.leftRelation + join.rightRelation;
      assert(join.joinID == joinInfos.size() && "the actual join infos idx should not change.");
      joinInfos.emplace_back(curQueryGraph.getJoinInfo(join));
      if (neededForJoin.isSubsetOf(newRequiredRelations) || neededForJoin.isSubsetOf(newSemiJoinedRelations)) {
         for (auto leftRel : join.leftRelation)
            for (auto rightRel : join.rightRelation)
               connectedRelations.unionSets(leftRel, rightRel);
         joins.emplace_back(join);
      } else {
         assert(neededForJoin.isSubsetOf(newRequiredRelations + newSemiJoinedRelations)); // all relations must be covered here
         // handle case where we have a connection from required with semi-joined relations
         auto newJCL = &joinConditionList.emplace_back(JoinConditionList::dynCast(curQueryGraph.getJoinInfo(join).joinCondition)->get_conditions());
         joinInfos.back().joinCondition = newJCL;
         if (join.rightRelation.isSubsetOf(newRequiredRelations)) {
            changedJoins.emplace_back(join.leftRelation, join.rightRelation, join.joinID, algebra::JoinType::RightSemi);
         } else if (join.leftRelation.isSubsetOf(newRequiredRelations)) {
            changedJoins.emplace_back(join.rightRelation, join.leftRelation, join.joinID, algebra::JoinType::RightSemi);
         } else {
            unreachable();
         }
         id2jcl[join.joinID] = newJCL;
      }
   }
   if (!changedJoins.empty()) {
      // create new semi-joins
      for (auto& changed : changedJoins) {
         // all relations from the left side should belong to the same equivalence class, at least for now, since we haven't merged introduced brigdes yet
         if constexpr (debugMode) {
            [[maybe_unused]] auto id = connectedRelations.find(changed.leftRelation.front());
            for ([[maybe_unused]] auto allChangedRels : changed.leftRelation)
               assert(connectedRelations.find(allChangedRels) == id);
            for ([[maybe_unused]] auto allChangedRels : changed.rightRelation)
               assert(connectedRelations.find(allChangedRels) != id);
         }

         // add all relations from the left connected relation set to the required right set.
         auto newLeft = changed.leftRelation;
         for (auto leftRelation : changed.leftRelation)
            newLeft += connectedRelations.getEqualElements(leftRelation);
         assert(changed.leftRelation.isSubsetOf(newLeft));
         changed.leftRelation = std::move(newLeft);
      }

      // check if we would disconnect the query graph with the current setup. This can happen, if we remove a relation/cluster connecting two other relations.
      // then, we have to connect the semijoins to be able to filter correctly => introduce hyperedge.
      // 1. detect all semi-joins with same right dependent set: sort, then all equal elements are consecutive in the array
      std::sort(changedJoins.begin(), changedJoins.end(), [](const OptimizerJoin& a, const OptimizerJoin& b) { return a.leftRelation.compareNumeric(b.leftRelation); });
      // warning: the indexes are now different from the indexes in the join Info lists, since the order may has changed due to reorderning

      size_t curJoinIdx = 0;
      for (size_t idx = 1; idx != changedJoins.size(); ++idx) {
         auto& curJoin = changedJoins[curJoinIdx];
         auto& otherJoin = changedJoins[idx];

         if (curJoin.leftRelation == otherJoin.leftRelation) {
            // we have to merge the joins to be able to execute them properly
            assert(joinInfos[curJoin.joinID].joinCondition == id2jcl[curJoin.joinID]);
            assert(joinInfos[otherJoin.joinID].joinCondition == id2jcl[otherJoin.joinID]);
            id2jcl[curJoin.joinID]->insert(*id2jcl[otherJoin.joinID]);
            curJoin.rightRelation += otherJoin.rightRelation;
            // maybe add here an edge for cross join?
         } else {
            // no merge required, write the currrent join out and continue with the check for the other join
            joins.emplace_back(curJoin);
            curJoinIdx = idx;
         }
      }
      joins.emplace_back(changedJoins[curJoinIdx]); // add the last join
   }

   auto qg = QueryGraph::generateQueryGraph(queryGraph.getDatabase(), curQueryGraph.getRelations(), joins, joinInfos, curQueryGraph.getTablePredicates());
   // generate the plan
   auto tmpSet = SettingBase::getSetting<bool>("algebra.join.checkCrossProduct")->setTemporary(false); // we have to allow cross products for estimations since here we may enforce some by construction.
   auto optAlg = OptimizerAlgorithm::executeAlgorithm(OptimizerAlgorithm::AnyPlan, qg, oracle);
   // execute the algebra tree
   return oracle.estimate(optAlg->getOptimalPlan(), qg, *this);
}
// ---------------------------------------------------------------------------------------------------
std::vector<infra::BitSet64> FactorizedEstimator::getEquivalenceClass2Relations(const Plan& plan) const {
   std::vector<infra::BitSet64> eqClasses2Relations;
   eqClasses2Relations.resize(queryGraph.maxEquivalenceClasses());
   for (auto relation : plan.getCoveredRelations()) {
      auto eqClasses = queryGraph.getIUEquivalenceClasses(relation);
      for (auto eqClass : eqClasses) {
         assert(eqClass < eqClasses2Relations.size());
         eqClasses2Relations[eqClass].insert(relation);
      }
   }
   return eqClasses2Relations;
}
// ---------------------------------------------------------------------------------------------------
Estimate* FactorizedEstimator::estimateJoinPlan(const JoinPlan& plan) {
   assert(plan.getType() == Join);

   auto& estimate = estimates.emplace_back();

   // here is the algebra tree generated. Maybe we can optimize here to generate the factorized tree only once?
   PredecessorDetector predecessorDetector(plan, queryGraph, *this);
   auto* topJoin = algebra::Print::dynCast(&predecessorDetector.getRootTree())->getChild().get();
   while (topJoin->getType() == algebra::Operator::Reference) {
      topJoin = &static_cast<algebra::Reference*>(topJoin)->getChild();
   }
   assert(topJoin->getType() == algebra::Operator::InnerJoin);
   auto& topJoinTree = *algebra::InnerJoin::dynCast(topJoin);

   auto leftPredecessorInformation = predecessorDetector.genPredecessorInformation(*topJoinTree.getLeftChild());
   auto rightPredecessorInformation = predecessorDetector.genPredecessorInformation(*topJoinTree.getRightChild());
   auto outputPredecessorInformation = predecessorDetector.genPredecessorInformation(topJoinTree);

   infra::BitSet64 connectingRelations;
   for (auto& joinCondition : topJoinTree.getJoinCondition()) { // reuse join condition to get the connecting joins, these used IUs should already be optimized.
      assert(queryGraph.knowsIU(&joinCondition.get_left()) && queryGraph.knowsIU(&joinCondition.get_right()));
      connectingRelations.insert(queryGraph.getRelation(&joinCondition.get_left()));
      connectingRelations.insert(queryGraph.getRelation(&joinCondition.get_right()));
   }

   {
      auto leftConnectingRelations = leftPredecessorInformation.getPredecessors(connectingRelations & plan.getLeft()->getCoveredRelations());
      auto rightConnectingRelations = rightPredecessorInformation.getPredecessors(connectingRelations & plan.getRight()->getCoveredRelations());

      auto leftCostEstimate = executeSemiJoinPlan(*plan.getLeft(), leftConnectingRelations, plan.getLeft()->getCoveredRelations() - leftConnectingRelations);
      auto rightCostEstimate = executeSemiJoinPlan(*plan.getRight(), rightConnectingRelations, plan.getRight()->getCoveredRelations() - rightConnectingRelations);

      estimate.cost = static_cast<const FactorizedEstimate*>(plan.getLeft()->getEstimate())->cost + static_cast<const FactorizedEstimate*>(plan.getRight()->getEstimate())->cost;
      estimate.cost += leftCostEstimate;
      estimate.cost += rightCostEstimate;
   }

   {
      const auto* leftEstimate = static_cast<const FactorizedEstimate*>(plan.getLeft()->getEstimate());
      const auto* rightEstimate = static_cast<const FactorizedEstimate*>(plan.getRight()->getEstimate());
      estimate.previousInsertedTuples += leftEstimate->previousInsertedTuples + leftEstimate->currentlyInsertedTuples;
      estimate.previousInsertedTuples += rightEstimate->previousInsertedTuples + rightEstimate->currentlyInsertedTuples;

      // estimate.insertedTuples += static_cast<const FactorizedEstimate*>(plan.getLeft()->getEstimate())->insertedTuples;
      // estimate.insertedTuples += static_cast<const FactorizedEstimate*>(plan.getRight()->getEstimate())->insertedTuples;
      auto totalConnectingRelations = outputPredecessorInformation.getPredecessors(connectingRelations);
      estimate.currentlyInsertedTuples = executeSemiJoinPlan(plan, totalConnectingRelations, plan.getCoveredRelations() - totalConnectingRelations);
   }
   auto eqClasses2Relations = getEquivalenceClass2Relations(plan);
   auto interestingClasses = getInterestingClasses(plan.getCoveredRelations());
   for (auto interest : interestingClasses) {
      auto firstOccurrence = outputPredecessorInformation.findFirstRelation(eqClasses2Relations[interest]);
      auto requiredRelations = outputPredecessorInformation.getPredecessors(firstOccurrence);

      assert(!estimate.estimates.contains(interest));
      estimate.estimates[interest] = executeSemiJoinPlan(plan, requiredRelations, plan.getCoveredRelations() - requiredRelations);
   }
   assert(estimate.estimates.size() == interestingClasses.size());

   return &estimate;
}
// ---------------------------------------------------------------------------------------------------
JoinCondition FactorizedEstimator::optimizeJoiningRelations(std::unique_ptr<algebra::Operator>& leftOp, std::unique_ptr<algebra::Operator>& rightOp, const IUSet& leftIUs, const IUSet& rightIUs) const {
   { // input validation
      assert(!leftIUs.empty() && !rightIUs.empty());
      [[maybe_unused]] const auto equivClass = queryGraph.getEquivalenceClass(*(*leftIUs.begin()));
      for ([[maybe_unused]] auto& iu : leftIUs)
         assert(queryGraph.getEquivalenceClass(*iu) == equivClass);
      for ([[maybe_unused]] auto& iu : rightIUs)
         assert(queryGraph.getEquivalenceClass(*iu) == equivClass);
   }

   auto findOptimalIU = [&](std::unique_ptr<algebra::Operator>& op, const IUSet& ius) {
      PredecessorDetector predecessorDetector(op, queryGraph);
      auto predecessorInformation = predecessorDetector.genPredecessorInformation(predecessorDetector.getRootTree());
      return predecessorInformation.findFirstIU(ius, queryGraph);
   };

   const IU* leftIU = findOptimalIU(leftOp, leftIUs);
   const IU* rightIU = findOptimalIU(rightOp, rightIUs);

   // return join of the optimal IUs
   return JoinCondition::create(leftIU, rightIU);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
