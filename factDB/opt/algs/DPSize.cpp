// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/algs/DPSize.hpp"
#include "factDB/algebra/JoinMode.hpp"
#include "factDB/algebra/Operator.hpp"
#include "factDB/algebra/visitors/GraphvizQueryPlan.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include <iostream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt::algs {
// ---------------------------------------------------------------------------------------------------
void DPSize::prepareBaseTables() {
   for (size_t idx = 0, limit = queryGraph.getRelations().size(); idx < limit; ++idx) {
      BaseTablePlan& baseTablePlan = baseTables.emplace_back(idx, estimator);
      planMap[infra::BitSet64(idx)] = &baseTablePlan;
   }
}
// ---------------------------------------------------------------------------------------------------
Plan* DPSize::getPlan(infra::BitSet64& curBitSet) const {
   assert(planMap.contains(curBitSet));
   Plan* p = planMap.find(curBitSet)->second;
   assert(p != nullptr);
   return p;
}
// ---------------------------------------------------------------------------------------------------
Plan* DPSize::chooseBetterPlan(infra::BitSet64& leftProblem, infra::BitSet64& rightProblem, factDB::opt::Plan* alternativePlan) {
   if (alternativePlan != nullptr && FlatEstimate::cast(alternativePlan->getEstimate())->cost < FlatEstimate::cast(getPlan(leftProblem)->getEstimate())->cost + FlatEstimate::cast(getPlan(rightProblem)->getEstimate())->cost)
      return alternativePlan; // merged plan cannot be better, give up
   auto *leftPlan = getPlan(leftProblem), *rightPlan = getPlan(rightProblem);
   if (!leftPlan->hasJoinEdgeWith(*rightPlan, queryGraph))
      return alternativePlan;
   JoinPlan combinedPlan = Plan::makeJoin(leftPlan, rightPlan, estimator, algebra::JoinMode::TopInsert);
   if (alternativePlan == nullptr) {
      auto newPlan = planAllocator.allocate();
      *newPlan = std::move(combinedPlan);
      return newPlan;
   } else {
      if (FlatEstimate::cast(combinedPlan.getEstimate())->cost < FlatEstimate::cast(alternativePlan->getEstimate())->cost) // check if new plan dominates the old plan
         *alternativePlan = std::move(combinedPlan);
      return alternativePlan;
   }
}
// ---------------------------------------------------------------------------------------------------
void DPSize::createJoin(infra::BitSet64& leftProblem, infra::BitSet64& rightProblem) {
   auto combinedProblem = leftProblem + rightProblem;
   auto iterator = planMap.find(combinedProblem);
   Plan* combinedPlan = (iterator == planMap.end()) ? nullptr : iterator->second;
   auto betterPlan = chooseBetterPlan(leftProblem, rightProblem, combinedPlan);
   if (iterator == planMap.end() && betterPlan) { // INSERT
      planMap[combinedProblem] = betterPlan;
   }
}
// ---------------------------------------------------------------------------------------------------
Plan* DPSize::getOptimalPlan() const {
   infra::BitSet64 curPlanSet(0, queryGraph.getRelations().size(), infra::BitSet64::RangeEnum::Range);
   return getPlan(curPlanSet);
}
// ---------------------------------------------------------------------------------------------------
void DPSize::generateOptimalPlan() {
   prepareBaseTables();

   std::vector<std::vector<infra::BitSet64>> sizes;
   sizes.resize(queryGraph.getRelations().size() + 1);
   for (auto& r : baseTables)
      sizes[1].push_back(infra::BitSet64{r.getRelation()});

   for (size_t curSize = 2; curSize <= queryGraph.getRelations().size(); ++curSize) {
      for (size_t curLeftSize = 1; curLeftSize < curSize; ++curLeftSize) {
         for (auto& leftProblem : sizes[curLeftSize]) {
            for (auto& rightProblem : sizes[curSize - curLeftSize]) {
               if (leftProblem.doesIntersectWith(rightProblem))
                  continue; // right and left problem contain same relations

               auto beforeSize = planMap.size();
               createJoin(leftProblem, rightProblem);
               if (beforeSize != planMap.size()) { // created a new plan, insert into sizes array
                  sizes[curSize].push_back(leftProblem + rightProblem);
               }
            }
         }
      }
   }
}
// ---------------------------------------------------------------------------------------------------
void DPSizeLinear::generateOptimalPlan() {
   prepareBaseTables();

   std::vector<std::vector<infra::BitSet64>> sizes;
   sizes.resize(queryGraph.getRelations().size() + 1);
   for (auto& r : baseTables)
      sizes[1].push_back(infra::BitSet64{r.getRelation()});

   for (size_t curSize = 2; curSize <= queryGraph.getRelations().size(); ++curSize) {
      size_t curLeftSize = curSize - 1; // right-deep plans
      for (auto& leftProblem : sizes[curLeftSize]) {
         for (auto& rightProblem : sizes[curSize - curLeftSize]) {
            if (leftProblem.doesIntersectWith(rightProblem))
               continue; // right and left problem contain same relations

            auto beforeSize = planMap.size();
            createJoin(leftProblem, rightProblem);
            if (beforeSize != planMap.size()) { // created a new plan, insert into sizes array
               sizes[curSize].push_back(leftProblem + rightProblem);
            }
         }
      }
   }
}
// ---------------------------------------------------------------------------------------------------
void DPSize::printDPTable(std::vector<std::vector<infra::BitSet64>> sizes) const {
   for (size_t curSize = 1; curSize <= queryGraph.getRelations().size(); ++curSize) {
      std::cout << "size = " << curSize << "  ---------------------------------" << std::endl;
      for (auto& prob : sizes[curSize]) {
         Plan* plan = getPlan(prob);
         std::cout << "--- cost: " << reinterpret_cast<FlatEstimate*>(plan->getEstimate())->cost << std::endl;
         auto operatorPlan = plan->generateOperatorPlan(queryGraph, estimator);
         algebra::visitors::GraphvizQueryPlan::plotSubPlan(queryGraph.getDatabase(), *operatorPlan);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt::algs
// ---------------------------------------------------------------------------------------------------