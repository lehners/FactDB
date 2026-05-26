#ifndef H_FACTDB_FACTDB_OPT_ESTIMATORS_CARDINALITY_ESTIMATOR_HPP
#define H_FACTDB_FACTDB_OPT_ESTIMATORS_CARDINALITY_ESTIMATOR_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Expression.hpp"
#include <cstdint>
#include <memory>
#include <unordered_map>
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB { struct IUSet; }
namespace factDB::algebra { class Operator; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class QueryGraph;
class BaseTablePlan;
class JoinPlan;
class Plan;
class BaseOracle;
struct OptimizerJoin;
// ---------------------------------------------------------------------------------------------------
struct Estimate {
};
// ---------------------------------------------------------------------------------------------------
struct FlatEstimate : public Estimate {
   uint64_t cardinality = 0;
   uint64_t cost = 0;

   FlatEstimate(uint64_t cardinality_, uint64_t cost_) : cardinality(cardinality_), cost(cost_) {} // NOLINT(bugprone-easily-swappable-parameters)
   FlatEstimate() = default;

   static const FlatEstimate* cast(const Estimate* estimate) { return static_cast<const FlatEstimate*>(estimate); }
};
// ---------------------------------------------------------------------------------------------------
struct FactorizedEstimate : public Estimate {
   std::unordered_map<std::uint32_t, std::size_t> estimates;
   size_t previousInsertedTuples = 0; // tuples inserted in previous operators
   size_t currentlyInsertedTuples = 0; // tuples inserted in current operator
   uint64_t cost = 0;

   static const FactorizedEstimate* cast(const Estimate* estimate) { return static_cast<const FactorizedEstimate*>(estimate); }
};
// ---------------------------------------------------------------------------------------------------
class BaseEstimator {
   protected:
   const QueryGraph& queryGraph;
   BaseOracle& oracle;

   public:
   constexpr explicit BaseEstimator(const QueryGraph& qg, BaseOracle& cardinalityOracle) : queryGraph(qg), oracle(cardinalityOracle) {}
   virtual ~BaseEstimator() = default;

   virtual Estimate* estimatePlan(const Plan* plan);
   virtual Estimate* estimateBaseTablePlan(const BaseTablePlan& baseTablePlan) = 0;
   virtual Estimate* estimateJoinPlan(const JoinPlan& leftPlan) = 0;
   BaseOracle& getOracle() const { return oracle; }

   // select the best 2 IUs connecting the left and the right plan. The ius must belong to the same equivalence class.
   virtual JoinCondition optimizeJoiningRelations(std::unique_ptr<algebra::Operator>& leftOperator, std::unique_ptr<algebra::Operator>& rightOperator, const IUSet& leftIUs, const IUSet& rightIUs) const;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_OPT_ESTIMATORS_CARDINALITY_ESTIMATOR_HPP
