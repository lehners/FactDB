// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/JoinMode.hpp"
#include "factDB/algebra/Operator.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Reference.hpp"
#include "factDB/gen/QueryHandler.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/opt/OptimizerRepresentationGenerator.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/opt/estimators/BaseEstimator.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
QueryGraph QueryGraph::generateQueryGraph(const Database& database, std::vector<OptimizerRelation> relations, std::vector<OptimizerJoin> joins, std::vector<JoinInfos> joinInfos, std::vector<std::pair<unsigned, const Expression&>> tablePredicates) {
   QueryGraph qc(database);
   qc.relations = std::move(relations);
   qc.joins = std::move(joins);
   qc.joinInfos = std::move(joinInfos);
   qc.tablePredicates = std::move(tablePredicates);
   qc.prepareForOptimization();
   return qc;
}
// ---------------------------------------------------------------------------------------------------
const infra::BitSetVar& QueryGraph::getIUEquivalenceClasses(uint32_t relation) const {
   assert(relation < relations2equivClasses.size());
   return relations2equivClasses[relation];
}
// ---------------------------------------------------------------------------------------------------
QueryGraph QueryGraph::generateQueryGraph(const Database& database, const algebra::Operator& query) {
   QueryGraph qc(database, query);
   qc.deriveOperators();
   qc.prepareForOptimization();
   return qc;
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<algebra::Operator> OperatorCache::getOperatorPlan(const factDB::opt::Plan& plan) const {
#if 1
   auto res = plan2operator.find(plan.getPlanID());
   if (res == plan2operator.end())
      return {};
   else
      return std::make_unique<algebra::Reference>(*res->second);
#else
   return {};
#endif
}
// ---------------------------------------------------------------------------------------------------
void OperatorCache::insertOperatorPlan(const Plan& plan, std::unique_ptr<algebra::Operator>& op) {
#if 1
   assert(op->getType() != algebra::Operator::Reference);
   assert(!plan2operator.contains(plan.getPlanID()));
   auto& location = plan2operator[plan.getPlanID()];
   if (op->getType() == algebra::Operator::Reference)
      return;
   location = std::move(op);
   op = std::make_unique<algebra::Reference>(*location);
#endif
}
// ---------------------------------------------------------------------------------------------------
void QueryGraph::deriveOperators() {
   auto representation = OptimizerRepresentationGenerator::apply(query);
   relations = std::move(representation.relations);
   joins = std::move(representation.joins);
   joinInfos = std::move(representation.joinInfos);
   joinConditionLists = std::move(representation.adaptedJoinConditions);
   tablePredicates = std::move(representation.tablePredicates);
   aggregationMode = representation.aggregationMode;
}
// ---------------------------------------------------------------------------------------------------
void QueryGraph::prepareForOptimization() {
   generateIuEquivalenceClasses();

   relations2equivClasses.resize(relations.size(), infra::BitSetVar(equivalentIUs.upperBound()));
   relations2strongEquivClasses.resize(relations.size(), infra::BitSetVar(innerJoinEquivalentIUs.upperBound()));
   equivClass2ius.resize(equivalentIUs.upperBound());
   strongEquivClass2ius.resize(innerJoinEquivalentIUs.upperBound());

   for (size_t idx = 0; idx != relations.size(); ++idx)
      relation2id[relations[idx].alias] = idx;

   for (auto& curJoin : joinInfos) {
      // generate map from relation to the respective equivalence classes they hold/participate in.
      auto participatingIUs = curJoin.joinCondition->collectIUs();
      for (const IU* iu : participatingIUs) {
         assert(relation2id.contains(iu->table));
         auto relation = relation2id.find(iu->table)->second;

         relations2equivClasses[relation].insert(equivalentIUs.findConst(*iu));
         if (innerJoinEquivalentIUs.contains(*iu))
            relations2strongEquivClasses[relation].insert(innerJoinEquivalentIUs.findConst(*iu));
         iu2relation[iu] = relation;
      }
   }

   for (const IU* iu : equivalentIUs.getValues()) {
      auto equivClass = equivalentIUs.findConst(*iu);
      equivClass2ius[equivClass].insert(iu);
   }
   for (const IU* iu : innerJoinEquivalentIUs.getValues()) {
      auto equivClass = innerJoinEquivalentIUs.findConst(*iu);
      strongEquivClass2ius[equivClass].insert(iu);
   }
}
// ---------------------------------------------------------------------------------------------------
void QueryGraph::printQueryGraph(factDB::FileWriter& out) const {
   out << fw::verbosity(false)
       << "graph structs {" << fw::endl()
       << "  node [shape=record];" << fw::endl();
   for (size_t relationIdx = 0; relationIdx < relations.size(); ++relationIdx) {
      auto& curRelation = relations[relationIdx];
      out << fw::fmt("relation{} [label=\"{} {}\"];", relationIdx, db.getSchema(curRelation.relation).name, curRelation.alias) << fw::endl();
   }

   for (size_t hyperedgeNode = 0; auto& join : joins) {
      auto joinTypeStr = join.joinType == algebra::JoinType::RightSemi ? "RightSemi" : "Inner";
      auto& joinCondition = *joinInfos[join.joinID].joinCondition;
      if (join.leftRelation.size() == 1 && join.rightRelation.size() == 1) {
         out << fw::fmt("relation{} -- relation{}[label=\"", join.leftRelation.front(), join.rightRelation.front()) << joinCondition << fw::fmt(" ({}) \"];", joinTypeStr) << fw::endl();
      } else { // hyper edge => insert pseudo hyper-edge
         out << fw::fmt("hypernode{} [penwidth=0 label=\"", hyperedgeNode) << joinCondition << fw::fmt(" ({})\"];", joinTypeStr) << fw::endl();
         for (auto leftRelation : join.leftRelation)
            out << fw::fmt("relation{} -- hypernode{};", leftRelation, hyperedgeNode) << fw::endl();
         for (auto rightRelation : join.rightRelation)
            out << fw::fmt("hypernode{} -- relation{};", hyperedgeNode, rightRelation) << fw::endl();

         ++hyperedgeNode;
      }
   }

   out << "}" << fw::endl()
       << fw::verbosity(true);
}
// ---------------------------------------------------------------------------------------------------
void QueryGraph::printIUEquivalences(FileWriter& writer) const {
   auto classes = std::vector<std::list<const IU*>>(equivalentIUs.size());
   for (const IU* iu : equivalentIUs.getValues()) {
      auto eqClass = equivalentIUs.findConst(*iu);
      assert(eqClass < classes.size());
      classes[eqClass].push_back(iu);
   }
   writer << fw::verbosity(false);
   for (const auto& equivClass : classes) {
      if (equivClass.empty())
         continue;
      writer << "{" << fw::pushSeparator(", ") << equivClass << fw::popSeparator() << "}" << fw::endl();
   }
   writer << fw::verbosity(true);
}
// ---------------------------------------------------------------------------------------------------
void QueryGraph::generateIuEquivalenceClasses() {
   // called during preparation of query graph
   if (query != nullptr) {
      for (const IU* iu : query->collectIUs()) {
         equivalentIUs.addElement(*iu);
         innerJoinEquivalentIUs.addElement(*iu);
      }
   }

   for (auto& join : joins) {
      auto& joinCondition = joinInfos[join.joinID].joinCondition;
      const auto joinType = join.joinType;
      auto participatingIUs = joinCondition->collectIUs();
      assert(joinCondition->get_type() == Expression::JoinConditionList && "other types are currently not supported");
      for (auto iu : participatingIUs) {
         equivalentIUs.unionSets(**participatingIUs.begin(), *iu);
         if (joinType == algebra::JoinType::Inner)
            innerJoinEquivalentIUs.unionSets(**participatingIUs.begin(), *iu);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
bool QueryGraph::isGraphConnected() const {
   // checks if all nodes in the graph are at least connected by an hyperedge.
   // but even if the graph is connected, it may be necessary to introduce crossproducts to create an algebra tree
   infra::UnionFindBase connectedRelations(relations.size());
   for (auto& j : joins) {
      for (auto rel : j.leftRelation + j.rightRelation)
         connectedRelations.unionSets(rel, j.leftRelation.front());
   }
   return connectedRelations.isSingleSet();
}
// ---------------------------------------------------------------------------------------------------
uint32_t QueryGraph::getRelation(const factDB::IU* iu) const {
   assert(iu2relation.contains(iu));
   auto iter = iu2relation.find(iu);
   assert(iter != iu2relation.end());
   return iter->second;
}
// ---------------------------------------------------------------------------------------------------
bool QueryGraph::hasJoinEdge(const infra::BitSet64& leftRelations, const infra::BitSet64& rightRelations) const {
   assert(!leftRelations.empty() && !rightRelations.empty());
   infra::BitSetVar leftSet(equivalentIUs.getValues().size()), rightSet(equivalentIUs.getValues().size());
   auto strongClasses = innerJoinEquivalentIUs.getValues().size();
   infra::BitSetVar leftSetStrong(strongClasses), rightSetStrong(strongClasses);

   for (auto rel : leftRelations) {
      leftSet += relations2equivClasses[rel];
      leftSetStrong += relations2strongEquivClasses[rel];
   }
   for (auto rel : rightRelations) {
      rightSet += relations2equivClasses[rel];
      rightSetStrong += relations2strongEquivClasses[rel];
   }
   if (leftSetStrong.doesIntersectWith(rightSetStrong)) { // there are inner joins.
      return true;
   } else if (leftSet.doesIntersectWith(rightSet)) {
      // check if there is an actual edge between the left and the right side.
      for (auto& join : joins) {
         if ((join.leftRelation.isSubsetOf(leftRelations) && join.rightRelation.isSubsetOf(rightRelations)) ||
             (join.joinType == algebra::JoinType::Inner && join.rightRelation.isSubsetOf(leftRelations) && join.leftRelation.isSubsetOf(rightRelations))) {
            return true;
         }
      }
      return false;
   }
   return false;
}
// ---------------------------------------------------------------------------------------------------
std::vector<ConnectingJoinInfo> QueryGraph::getConnectingJoins(infra::BitSet64 leftRelations, infra::BitSet64 rightRelations, std::unique_ptr<algebra::Operator>& leftOp, std::unique_ptr<algebra::Operator>& rightOp, const BaseEstimator& estimator) const {
   // check for each join, if both relations contain elements from the join
   assert(!leftRelations.doesIntersectWith(rightRelations));
   if (leftRelations.empty() || rightRelations.empty())
      return {};

   infra::BitSetVar leftSet(equivalentIUs.getValues().size());
   infra::BitSetVar rightSet(equivalentIUs.getValues().size());

   for (auto rel : leftRelations)
      leftSet += relations2strongEquivClasses[rel];
   for (auto rel : rightRelations)
      rightSet += relations2strongEquivClasses[rel];

   // derive the equivalence classes which have to be considered.
   auto interestingEqClasses = leftSet & rightSet;

   std::vector<ConnectingJoinInfo> joinConditions;
   for (auto eqClass : interestingEqClasses) {
      // iterate through interesting classes and add "good" edges
      auto& interestingIUs = strongEquivClass2ius[eqClass];

      IUSet leftIUSet, rightIUSet;
      for (const IU* iu : interestingIUs) {
         auto relationId = getRelation(iu);
         if (leftRelations.contains(relationId)) {
            leftIUSet.insert(iu);
         } else if (rightRelations.contains(relationId)) {
            rightIUSet.insert(iu);
         }
      }
      assert(!leftIUSet.empty() && !rightIUSet.empty());
      // now we know which ius from the left and the right participate in the join, and we can add the optimal edge for the join

      // the joinConditions from the previous iterations cannot contain the same IUs, since then the equivalence classes would be merged.
      // i.e. we only consider the found IUs of the current equivalence class for the connecting join.
      auto join = estimator.optimizeJoiningRelations(leftOp, rightOp, leftIUSet, rightIUSet);
      joinConditions.emplace_back(algebra::JoinType::Inner, join);
   }

   for (auto& join : joins) {
      if (join.joinType == algebra::JoinType::Inner)
         continue;
      if ((join.leftRelation.isSubsetOf(leftRelations) && join.rightRelation.isSubsetOf(rightRelations)) ||
          (join.leftRelation.isSubsetOf(rightRelations) && join.rightRelation.isSubsetOf(leftRelations))) {
         for (auto jc : JoinConditionList::dynCast(getJoinInfo(join).joinCondition)->get_conditions())
            joinConditions.emplace_back(algebra::JoinType::RightSemi, jc);
      }
   }
   return joinConditions;
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<algebra::Operator> QueryGraph::generateAggregatedTree(std::unique_ptr<algebra::Operator> newTree, const algebra::Operator& oldTree) const {
   auto aggMode = getAggregationMode();
   if (aggMode == AggregationMode::NotSelected) {
      if (oldTree.getType() == algebra::Operator::Count)
         aggMode = AggregationMode::CountStar;
      else if (oldTree.getType() == algebra::Operator::Print)
         aggMode = AggregationMode::Print;
   }

   switch (aggMode) {
      case AggregationMode::NotSelected:
         unreachable();
      case AggregationMode::Print: {
         auto& rootPrint = static_cast<const algebra::Print&>(oldTree);
         return std::make_unique<algebra::Print>(std::move(newTree), rootPrint.getSeperator());
      } break;
      case AggregationMode::CountStar:
         return std::make_unique<algebra::Count>(std::move(newTree));
         break;
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
const JoinInfos& QueryGraph::getJoinInfo(const OptimizerJoin& join) const {
   assert(join.joinID < joinInfos.size());
   return joinInfos[join.joinID];
}
// ---------------------------------------------------------------------------------------------------
std::vector<const Expression*> QueryGraph::getFilterPredicates(size_t relationIdx) const {
   std::vector<const Expression*> ret;
   for (auto& p : tablePredicates) {
      if (p.first == relationIdx) {
         ret.push_back(&p.second);
      }
   }
   return ret;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
