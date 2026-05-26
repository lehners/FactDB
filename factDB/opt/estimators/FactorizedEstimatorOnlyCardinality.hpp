#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/estimators/FactorizedEstimator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class FactorizedEstimatorOnlyCardinality : public opt::FactorizedEstimator {
   std::list<opt::FlatEstimate> estimates;

   public:
   explicit FactorizedEstimatorOnlyCardinality(const opt::QueryGraph& qg, opt::BaseOracle& cardinalityOracle) : FactorizedEstimator(qg, cardinalityOracle) {}
   ~FactorizedEstimatorOnlyCardinality() override = default;

   opt::Estimate* estimateBaseTablePlan(const opt::BaseTablePlan&) override;
   opt::Estimate* estimateJoinPlan(const opt::JoinPlan& plan) override;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
