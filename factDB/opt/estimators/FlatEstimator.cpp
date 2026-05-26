// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/estimators/FlatEstimator.hpp"
#include "factDB/opt/CardinalityOracle.hpp"
#include "factDB/opt/Plan.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
Estimate* FlatEstimator::estimateBaseTablePlan(const factDB::opt::BaseTablePlan& baseTablePlan) {
   assert(baseTablePlan.getType() == BaseTable);
   auto cardinalityEstimate = oracle.estimate(baseTablePlan, queryGraph, *this);

   return &estimates.emplace_back(cardinalityEstimate, 0);
}
// ---------------------------------------------------------------------------------------------------
Estimate* FlatEstimator::estimateJoinPlan(const factDB::opt::JoinPlan& join) {
   auto& estimate = estimates.emplace_back();
   estimate.cardinality = oracle.estimate(join, queryGraph, *this);

   const FlatEstimate* leftEstimate = static_cast<const FlatEstimate*>(join.getLeft()->getEstimate());
   const FlatEstimate* rightEstimate = static_cast<const FlatEstimate*>(join.getRight()->getEstimate());

   estimate.cost = estimate.cardinality + leftEstimate->cost + rightEstimate->cost;
   return &estimate;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
