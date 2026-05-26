// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/algs/AnyPlan.hpp"
#include "factDB/algebra/JoinMode.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/algs/OptimizerAlgorithms.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt::algs {
// ---------------------------------------------------------------------------------------------------
void AnyPlan::generateOptimalPlan() {
   prepareBaseTables();

   assert(!planMap.empty());
   auto allJoins = queryGraph.getJoins();
   const OptimizerJoin* halfDoableJoin;
   while (planMap.size() != 1) {
      halfDoableJoin = nullptr;
      [[maybe_unused]] size_t beforeSize = planMap.size();
      // select first query graph edge, which can be applied.
      for (const auto& join : allJoins) {
         Plan *leftCandidate = nullptr, *rightCandidate = nullptr;
         for (auto& [key, plan] : planMap) {
            if (join.leftRelation.isSubsetOf(key) && !join.rightRelation.doesIntersectWith(key))
               leftCandidate = plan;
            if (join.rightRelation.isSubsetOf(key) && !join.leftRelation.doesIntersectWith(key))
               rightCandidate = plan;
         }
         if (leftCandidate == nullptr || rightCandidate == nullptr) {
            if (leftCandidate != nullptr || rightCandidate != nullptr) {
               if (halfDoableJoin == nullptr)
                  halfDoableJoin = &join;
               else if (leftCandidate != nullptr && halfDoableJoin->leftRelation == join.leftRelation && join.rightRelation.isSubsetNotEqualOf(halfDoableJoin->rightRelation)) {
                  halfDoableJoin = &join;
               } else if (rightCandidate != nullptr && halfDoableJoin->rightRelation == join.rightRelation && join.leftRelation.isSubsetNotEqualOf(halfDoableJoin->leftRelation)) {
                  halfDoableJoin = &join;
               }
            }
            continue; // we cannot apply join (yet)
         }

         assert(leftCandidate != nullptr && rightCandidate != nullptr);
         // we found a plan-pair which we want to combine with the current join

         assert(queryGraph.hasJoinEdge(leftCandidate->getCoveredRelations(), rightCandidate->getCoveredRelations()));

         JoinPlan* newPlan = planAllocator.allocate();
         *newPlan = Plan::makeJoin(leftCandidate, rightCandidate, estimator, algebra::JoinMode::TopInsert);
         planMap.erase(leftCandidate->getCoveredRelations());
         planMap.erase(rightCandidate->getCoveredRelations());

         assert(!planMap.contains(newPlan->getCoveredRelations()));
         planMap[newPlan->getCoveredRelations()] = newPlan;
         break;
      }

      auto newPlanMap = planMap;
      auto createCrossproduct = [&](Plan* plan, Plan* candidate) {
         if (candidate == nullptr) {
            return plan;
         } else {
            newPlanMap.erase(candidate->getCoveredRelations());
            newPlanMap.erase(plan->getCoveredRelations());

            Plan* newPlan = planAllocator.allocate();
            *newPlan = Plan::makeJoin(candidate, plan, estimator, algebra::JoinMode::TopInsert);
            newPlanMap[newPlan->getCoveredRelations()] = newPlan;

            return newPlan;
         }
      };

      if (planMap.size() == beforeSize) {
         assert(halfDoableJoin != nullptr && "some other cross product has to be constructed, currently not implemented");
         Plan *leftCandidate = nullptr, *rightCandidate = nullptr;
         for (auto& [key, plan] : planMap) {
            if (halfDoableJoin->leftRelation.doesIntersectWith(key) && !halfDoableJoin->rightRelation.doesIntersectWith(key))
               leftCandidate = createCrossproduct(plan, leftCandidate);
            if (halfDoableJoin->rightRelation.doesIntersectWith(key) && !halfDoableJoin->leftRelation.doesIntersectWith(key))
               rightCandidate = createCrossproduct(plan, rightCandidate);
         }
         planMap = std::move(newPlanMap);
      }
      assert(planMap.size() <= beforeSize - 1 && "has not found a suitable pair to join");
   }
   assert(planMap.size() == 1);
}
// ---------------------------------------------------------------------------------------------------
void AnyPlan::prepareBaseTables() {
   for (size_t idx = 0, limit = queryGraph.getRelations().size(); idx < limit; ++idx) {
      BaseTablePlan& baseTablePlan = baseTables.emplace_back(idx, estimator);
      planMap[infra::BitSet64(idx)] = &baseTablePlan;
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt::algs
// ---------------------------------------------------------------------------------------------------
