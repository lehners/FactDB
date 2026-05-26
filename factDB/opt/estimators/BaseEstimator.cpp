// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/estimators/BaseEstimator.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/IUSet.hpp"
#include "factDB/opt/OptimizerRepresentation.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/opt/QueryGraph.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
Estimate* BaseEstimator::estimatePlan(const Plan* plan) {
   assert(plan != nullptr);
   switch (plan->getType()) {
      case BaseTable: return estimateBaseTablePlan(*static_cast<const BaseTablePlan*>(plan));
      case Join: return estimateJoinPlan(*static_cast<const JoinPlan*>(plan));
      case Uninitialized:
      case TableScan:
      default:
         unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
JoinCondition BaseEstimator::optimizeJoiningRelations(std::unique_ptr<algebra::Operator>&, std::unique_ptr<algebra::Operator>&, const IUSet& leftIUs, const IUSet& rightIUs) const {
   { // input validation
      assert(!leftIUs.empty() && !rightIUs.empty());
      [[maybe_unused]] const auto equivClass = queryGraph.getEquivalenceClass(*(*leftIUs.begin()));
      for ([[maybe_unused]] auto& iu : leftIUs)
         assert(queryGraph.getEquivalenceClass(*iu) == equivClass);
      for ([[maybe_unused]] auto& iu : rightIUs)
         assert(queryGraph.getEquivalenceClass(*iu) == equivClass);
   }

   OrderedIUSet leftOIUs(leftIUs);
   OrderedIUSet rightOIUs(rightIUs);
   auto cmp = [](const IU* a, const IU* b) {
      if (a->table == b->table)
         return a->column <= b->column;
      return a->table < b->table;
   };

   auto leftIU = *std::min_element(leftOIUs.begin(), leftOIUs.end(), cmp);
   auto rightIU = *std::min_element(rightOIUs.begin(), rightOIUs.end(), cmp);
   return JoinCondition::create(leftIU, rightIU);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------