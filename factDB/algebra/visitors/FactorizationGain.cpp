// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/FactorizationGain.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/opt/estimators/FactorizedEstimator.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
#include <cassert>
#include <factDB/opt/PredecessorDetector.hpp>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
opt::Estimate* FlatEstimatorVariant::estimateBaseTablePlan(const opt::BaseTablePlan& baseTablePlan) {
   assert(baseTablePlan.getType() == opt::BaseTable);
   auto cardinalityEstimate = oracle.estimate(baseTablePlan, queryGraph, *this);

   return &estimates.emplace_back(cardinalityEstimate, 0);
}
// ---------------------------------------------------------------------------------------------------
opt::Estimate* FlatEstimatorVariant::estimateJoinPlan(const factDB::opt::JoinPlan& join) {
   auto& estimate = estimates.emplace_back();
   estimate.cardinality = oracle.estimate(join, queryGraph, *this);

   const opt::FlatEstimate* leftEstimate = static_cast<const opt::FlatEstimate*>(join.getLeft()->getEstimate());
   const opt::FlatEstimate* rightEstimate = static_cast<const opt::FlatEstimate*>(join.getRight()->getEstimate());

   estimate.cost = leftEstimate->cost + rightEstimate->cost + leftEstimate->cardinality + rightEstimate->cardinality;
   return &estimate;
}
// ---------------------------------------------------------------------------------------------------
void FactorizationGain::visitProduce(const Print& print) {
   queryGraph.query = &print;
   queryGraph.deriveOperators();
   queryGraph.prepareForOptimization();
   print.getChild()->produce(*this);

   // generated estimated trees, todo output
}
// ---------------------------------------------------------------------------------------------------
void FactorizationGain::visitProduce(const Count& count) {
   queryGraph.query = &count;
   queryGraph.deriveOperators();
   queryGraph.prepareForOptimization();
   count.getChild()->produce(*this);

   // generated estimated trees, now print output
   auto querynameSetting = SettingBase::getSetting<std::string>("queryname");

   for (auto [costFlat, costFact, joinMode] : toPrint) {
      assert(*costFact <= *costFlat);
      std::stringstream ss;
      ss << std::setprecision(5) << (1.0 * *costFact) / *costFlat;
      out << fw::fmt("[{}] (diff: {}, {}) flat: {} vs. fact {} ({})", querynameSetting->get(), ss.str(), *costFlat - *costFact, *costFlat, *costFact, InnerJoin::toString(joinMode)) << fw::endl();
   }
}
// ---------------------------------------------------------------------------------------------------
void FactorizationGain::visitProduce(const TableScan& tableScan) {
   auto iter = queryGraph.relation2id.find(tableScan.getAlias());
   assert(iter != queryGraph.relation2id.end());
   generatedPlanFactorized = &baseTablePlans.emplace_back(iter->second, factorizedEstimator);
   generatedPlanFlat = &baseTablePlans.emplace_back(iter->second, flatEstimator);
}
// ---------------------------------------------------------------------------------------------------
void FactorizationGain::visitProduce(const Selection&) {
   not_implemented();
}
// ---------------------------------------------------------------------------------------------------
void FactorizationGain::visitProduce(const CrossProduct&) {
   not_implemented();
}
// ---------------------------------------------------------------------------------------------------
void FactorizationGain::visitProduce(const InnerJoin& join) {
   join.getLeftChild()->produce(*this);
   opt::Plan* leftPlanFact = generatedPlanFactorized;
   opt::Plan* leftPlanFlat = generatedPlanFlat;

   join.getRightChild()->produce(*this);

   generatedPlanFactorized = &joinPlans.emplace_back(opt::Plan::makeJoin(leftPlanFact, generatedPlanFactorized, factorizedEstimator, join.getJoinMode()));
   generatedPlanFlat = &joinPlans.emplace_back(opt::Plan::makeJoin(leftPlanFlat, generatedPlanFlat, flatEstimator, join.getJoinMode()));

   auto& cardinalityFlat = static_cast<const opt::FlatEstimate*>(generatedPlanFlat->getEstimate())->cardinality;
   auto& cardinalityFact = static_cast<const opt::FlatEstimate*>(generatedPlanFactorized->getEstimate())->cardinality;

   toPrint.emplace_back(&cardinalityFlat, &cardinalityFact, join.getJoinMode());
}
// ---------------------------------------------------------------------------------------------------
void FlatSizes::visitProduce(const Print& print) {
   queryGraph.query = &print;
   queryGraph.deriveOperators();
   queryGraph.prepareForOptimization();
   print.getChild()->produce(*this);

   // generated estimated trees, todo output
}
// ---------------------------------------------------------------------------------------------------
void FlatSizes::visitProduce(const Count& count) {
   queryGraph.query = &count;
   queryGraph.deriveOperators();
   queryGraph.prepareForOptimization();
   count.getChild()->produce(*this);

   // generated estimated trees, now print output
   auto querynameSetting = SettingBase::getSetting<std::string>("queryname");

   for (auto costFlat : toPrint) {
      out << fw::fmt("[{}] flat: {}", querynameSetting->get(), *costFlat) << fw::endl();
   }
}
// ---------------------------------------------------------------------------------------------------
void FlatSizes::visitProduce(const TableScan& tableScan) {
   auto iter = queryGraph.relation2id.find(tableScan.getAlias());
   assert(iter != queryGraph.relation2id.end());
   generatedPlanFlat = &baseTablePlans.emplace_back(iter->second, flatEstimator);
}
// ---------------------------------------------------------------------------------------------------
void FlatSizes::visitProduce(const Selection&) {
   not_implemented();
}
// ---------------------------------------------------------------------------------------------------
void FlatSizes::visitProduce(const CrossProduct&) {
   not_implemented();
}
// ---------------------------------------------------------------------------------------------------
void FlatSizes::visitProduce(const InnerJoin& join) {
   join.getLeftChild()->produce(*this);
   opt::Plan* leftPlanFlat = generatedPlanFlat;

   join.getRightChild()->produce(*this);

   generatedPlanFlat = &joinPlans.emplace_back(opt::Plan::makeJoin(leftPlanFlat, generatedPlanFlat, flatEstimator, join.getJoinMode()));

   auto& cardinalityFlat = static_cast<const opt::FlatEstimate*>(generatedPlanFlat->getEstimate())->cardinality;
   toPrint.emplace_back(&cardinalityFlat);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------