// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/algs/DPFact.hpp"
#include "factDB/algebra/JoinMode.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/opt/PredecessorDetector.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/queryc/NewFileWriter.hpp"
#include <factDB/infra/Setting.hpp>
#include <factDB/queryc/ExecutionMode.hpp>
#include <sstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt::algs {
// ---------------------------------------------------------------------------------------------------
void DPFact::prepareBaseTables() {
   for (size_t idx = 0, limit = queryGraph.getRelations().size(); idx < limit; ++idx) {
      BaseTablePlan& baseTablePlan = baseTables.emplace_back(idx, estimator);
      planMap[infra::BitSet64(idx)].emplace_back(&baseTablePlan);
   }
}
// ---------------------------------------------------------------------------------------------------
infra::BitSet64 DPFact::getPredecessorRelations(const Plan& plan, const infra::BitSetVar& interestingSets) {
   PredecessorDetector predecessorDetector(plan, queryGraph, estimator);
   auto predecessorInformation = predecessorDetector.genPredecessorInformation(predecessorDetector.getRootTree());

   infra::BitSet64 interestingRelations;
   auto eqClasses2Relations = estimator.getEquivalenceClass2Relations(plan);
   for (auto interest : interestingSets) {
      assert(interest < eqClasses2Relations.size());
      auto firstOccurrence = predecessorInformation.findFirstRelation(eqClasses2Relations[interest]);
      interestingRelations += predecessorInformation.getPredecessors(firstOccurrence);
   }
   return interestingRelations;
}
// ---------------------------------------------------------------------------------------------------
Plan* DPFact::chooseBetterPlan(Plan* leftPlan, Plan* rightPlan, algebra::JoinMode joinMode, std::list<factDB::opt::Plan*>& alternativePlans) {
   if (!leftPlan->hasJoinEdgeWith(*rightPlan, queryGraph))
      return nullptr; // ignore cross products
   const auto& interestingSets = estimator.getInterestingClasses(leftPlan->getCoveredRelations() + rightPlan->getCoveredRelations());
   std::unordered_set<const Plan*> theoreticallyDominatedPlans;
   size_t unsimulatedPlanId;
   { // first check if the plan dominates another plan already theoretically
      JoinPlan combinedPlanNotEstimated = Plan::makeJoin(leftPlan, rightPlan, joinMode);
      unsimulatedPlanId = combinedPlanNotEstimated.getPlanID();
      auto combinedPlanPredecessors = getPredecessorRelations(combinedPlanNotEstimated, interestingSets);

      for (const Plan* alternative : alternativePlans) {
         auto alternativePlanPredecessors = getPredecessorRelations(*alternative, interestingSets);
         if (alternativePlanPredecessors.isSubsetNotEqualOf(combinedPlanPredecessors)) {
            // we can already give up the new combined plan

            // new plan cannot be better. But can it be at least as good as the alternative plan? what happens if we have a 1:1 join?
            // => todo try it with a fix plan which uses only top inserts
            return nullptr;
         } else if (combinedPlanPredecessors.isSubsetNotEqualOf(alternativePlanPredecessors)) {
            // we know for sure that this plan is better, but have to calculate for the next iterations
            // BUT: it may still have the same cardinality for all interesting sets and if the cost is higher, the new plan may be thrown away due to higher costs. See dblp_aclyclic_204_03 relations 26,2,19,15
            theoreticallyDominatedPlans.insert(alternative);
         } else { // equality or difference?
            // we know nothing
         }
      }
   }

   JoinPlan combinedPlan = Plan::makeJoin(leftPlan, rightPlan, estimator, joinMode, unsimulatedPlanId);
   assert(combinedPlan.getPlanID() == unsimulatedPlanId);
   const auto& newPlanEstimate = *static_cast<const FactorizedEstimate*>(combinedPlan.getEstimate());

   std::list<factDB::opt::Plan*> paretoEfficientPlans;

   // Problem: use -BCD- rather than ABC-
   // => cost of ABC-D seems to be higher than cost of A-BCD. Why is this the case,
   //    does this consider the tree height which has to be iterated?
   // Base Table Cost should not matter since they contained in every join

   // compare with all other plans joining the same relations -> all have to cover the same IUSets.
   // We can stop and return the old set of alternative plans as soon as we have found one plan, which is better than the new plan, since every plan in the set is pareto optimal with regard to the same points.
   // I.e. it cannot be fully dominated by an inefficient plan because otherwise it would also be dominated by the plan, which dominates the inefficient plan.
   for (Plan* alternative : alternativePlans) {
      assert(interestingSets == estimator.getInterestingClasses(combinedPlan.getCoveredRelations()));

      const auto* alternativeEstimate = static_cast<const FactorizedEstimate*>(alternative->getEstimate());
      // check if the new plan outperforms the alternative plan.
      // if yes, the alternative plan has to be removed and possibly also other plans.
      // if no, continue to the next possible alternative.

      bool isParetoEqual, isNewPlanParetoBetter, isOldPlanParetoBetter;
      {
         bool isNewPlanLE = true, isNewPlanGE = true;
         bool isNewPlanOnceL = false, isNewPlanOnceG = false;
         for (const auto& [iu, estimate] : newPlanEstimate.estimates) {
            assert(alternativeEstimate->estimates.contains(iu));
            isNewPlanLE &= estimate <= alternativeEstimate->estimates.find(iu)->second;
            isNewPlanGE &= estimate >= alternativeEstimate->estimates.find(iu)->second;
            isNewPlanOnceL |= estimate < alternativeEstimate->estimates.find(iu)->second;
            isNewPlanOnceG |= estimate > alternativeEstimate->estimates.find(iu)->second;
         }
         isParetoEqual = isNewPlanLE && isNewPlanGE;
         isNewPlanParetoBetter = isNewPlanOnceL && isNewPlanLE;
         isOldPlanParetoBetter = isNewPlanOnceG && isNewPlanGE;

         assert(isParetoEqual != isNewPlanParetoBetter || isParetoEqual == false);
         assert(!isNewPlanParetoBetter || !isOldPlanParetoBetter);
      }

      if (isParetoEqual) { // both plans are equally pareto efficient, select plan with smaller cost
         // interesting relations of new plan: first relations containing interesting IU classes
         // check if interesting relations of plan A is a subset of (interesting relations+predecessors) of plan B => if yes, plan B can only perform worse, since it has more tuple to expand.
         // we do not even need to calc the estimates for such a plan.
         if (newPlanEstimate.cost == alternativeEstimate->cost) {
            // prefer top inserts over bottom inserts since they are only a half pipeline breaker.
            auto alternativeJoinPlan = static_cast<JoinPlan*>(alternative);
            bool isEquivalentPlan = (leftPlan == alternativeJoinPlan->getLeft() && rightPlan == alternativeJoinPlan->getRight());
            bool preferNewJoinMode = isEquivalentPlan && static_cast<JoinPlan*>(alternative)->getJoinMode() == algebra::JoinMode::BottomInsert && joinMode == algebra::JoinMode::TopInsert;
            bool preferAlternativeJoinMode = isEquivalentPlan && static_cast<JoinPlan*>(alternative)->getJoinMode() == algebra::JoinMode::TopInsert && joinMode == algebra::JoinMode::BottomInsert;
            if (theoreticallyDominatedPlans.contains(alternative) || preferNewJoinMode) { // the current alternative seems to be the worst plan
               break;
            } else if (alternativeEstimate->previousInsertedTuples > newPlanEstimate.previousInsertedTuples) { // alternative would insert more tuples -> more expensive
               break;
            } else if (alternativeEstimate->previousInsertedTuples < newPlanEstimate.previousInsertedTuples || preferAlternativeJoinMode) { // alternative is not worse than newly created plan
               paretoEfficientPlans.emplace_back(alternative);
               return nullptr; // drop new plan since it provides no benefits
            } else { // alternative is not worse than newly created plan
               paretoEfficientPlans.emplace_back(alternative);
               continue;
            }
         } else if (newPlanEstimate.cost < alternativeEstimate->cost) { // new plan is better, drop the other plan
            break; // this case must only occur once (otherwise set would be ill-formed) and then there must be no other plan which is more pareto efficient
         } else { // other plan is better, drop new plan
            paretoEfficientPlans.emplace_back(alternative);
            return nullptr;
         }
      }
      if (!isNewPlanParetoBetter) // other plan is more pareto efficient than this plan, but not necessary pareto dominated
         paretoEfficientPlans.emplace_back(alternative);
      if (isOldPlanParetoBetter) { // the new plan is worse than the current alternative plan, give it up.
         assert(!isNewPlanParetoBetter);
         return nullptr;
      }
   }

   alternativePlans = std::move(paretoEfficientPlans);
   auto allocation = planAllocator.allocate();
   *allocation = std::move(combinedPlan); // store the plan for later
   alternativePlans.emplace_back(allocation);
   return allocation;
}
// ---------------------------------------------------------------------------------------------------
void DPFact::addParetoEfficientPlans(const infra::BitSet64& leftProblem, const infra::BitSet64& rightProblem, std::list<Plan*>& otherPlans) {
   const auto& leftSolutions = planMap[leftProblem];
   const auto& rightSolutions = planMap[rightProblem];

   for (auto& leftSol : leftSolutions) {
      for (auto& rightSol : rightSolutions) {
         chooseBetterPlan(leftSol, rightSol, algebra::JoinMode::TopInsert, otherPlans);
         if (mode == OptimizerAlgorithm::DPFactLeftDeep) {
            if ((rightProblem + leftProblem).size() == 2)
               continue; // do not generate symmetric BI Joins for two relations
            chooseBetterPlan(leftSol, rightSol, algebra::JoinMode::BottomInsert, otherPlans);
         }
      }
   }
}
// ---------------------------------------------------------------------------------------------------
void DPFact::createJoins(infra::BitSet64& leftProblem, infra::BitSet64& rightProblem) {
   auto combinedProblem = leftProblem + rightProblem;
   addParetoEfficientPlans(leftProblem, rightProblem, planMap[combinedProblem]);
}
// ---------------------------------------------------------------------------------------------------
void DPFact::iteratePlans(std::vector<std::vector<infra::BitSet64>>& sizes, size_t totalSize, size_t leftSize) {
   // right-deep plans
   for (auto& leftProblem : sizes[leftSize]) {
      for (auto& rightProblem : sizes[totalSize - leftSize]) {
         if (leftProblem.doesIntersectWith(rightProblem))
            continue; // right and left problem contain same relations

         auto beforeSize = planMap.size();
         createJoins(leftProblem, rightProblem);
         if (beforeSize != planMap.size()) { // created a new plan, insert into sizes array
            sizes[totalSize].push_back(leftProblem + rightProblem);
         }
      }
   }
}
// ---------------------------------------------------------------------------------------------------
void DPFactLeftDeep::generateOptimalPlan() {
   prepareBaseTables();

   std::vector<std::vector<infra::BitSet64>> sizes;
   sizes.resize(queryGraph.getRelations().size() + 1);
   for (auto& r : baseTables)
      sizes[1].push_back(infra::BitSet64{r.getRelation()});

   for (size_t curSize = 2; curSize <= queryGraph.getRelations().size(); ++curSize) {
      size_t curLeftSize = curSize - 1;
      iteratePlans(sizes, curSize, curLeftSize);
   }
}
// ---------------------------------------------------------------------------------------------------
void DPFact::generateOptimalPlan() {
   prepareBaseTables();

   std::vector<std::vector<infra::BitSet64>> sizes;
   sizes.resize(queryGraph.getRelations().size() + 1);
   for (auto& r : baseTables)
      sizes[1].push_back(infra::BitSet64{r.getRelation()});

   for (size_t curSize = 2; curSize <= queryGraph.getRelations().size(); ++curSize) {
      for (size_t curLeftSize = 1; curLeftSize < curSize; ++curLeftSize) {
         iteratePlans(sizes, curSize, curLeftSize);
      }
   }
   for (auto& s : sizes) {
      for (auto& p : s) {
         auto solutions = planMap[p];
         std::cout << p.toBitString() << ": " << solutions.size() << "; ";
      }
      std::cout << std::endl;
   }
   std::cout << " --- " << std::endl;
}
// ---------------------------------------------------------------------------------------------------
Plan* DPFact::getOptimalPlan() const {
   infra::BitSet64 curPlanSet(0, queryGraph.getRelations().size(), infra::BitSet64::RangeEnum::Range);
   assert(planMap.contains(curPlanSet));
   auto& plans = planMap.find(curPlanSet)->second;
   assert(plans.size() >= 1);
   assert(plans.front() != nullptr);
   return plans.front();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt::algs
// ---------------------------------------------------------------------------------------------------