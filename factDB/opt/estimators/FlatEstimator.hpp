#ifndef H_FACTDB_FACTDB_OPT_ESTIMATORS_CARDINALITY_FLATESTIMATOR_HPP
#define H_FACTDB_FACTDB_OPT_ESTIMATORS_CARDINALITY_FLATESTIMATOR_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/estimators/BaseEstimator.hpp"
#include <list>
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class BaseOracle;
// ---------------------------------------------------------------------------------------------------
class FlatEstimator : public BaseEstimator {
   private:
   std::list<FlatEstimate> estimates;

   public:
   explicit FlatEstimator(const QueryGraph& qg, BaseOracle& cardinalityOracle) : BaseEstimator(qg, cardinalityOracle) {}
   ~FlatEstimator() override = default;

   Estimate* estimateBaseTablePlan(const BaseTablePlan& baseTablePlan) override;
   Estimate* estimateJoinPlan(const JoinPlan& plan) override;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_OPT_ESTIMATORS_CARDINALITY_FLATESTIMATOR_HPP
