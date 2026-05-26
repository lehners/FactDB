#ifndef H_FACTDB_FACTDB_OPT_ESTIMATORS_CARDINALITY_DUMMYESTIMATOR_HPP
#define H_FACTDB_FACTDB_OPT_ESTIMATORS_CARDINALITY_DUMMYESTIMATOR_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/CardinalityOracle.hpp"
#include "factDB/opt/estimators/BaseEstimator.hpp"
#include <list>
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB { class Database; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class DummyEstimator : public BaseEstimator {
   struct FakeOracle : public BaseOracle {
      FakeOracle(Database& db) : BaseOracle(db) {}
      ~FakeOracle() = default;

      size_t calcEstimate(const BaseTablePlan&, const QueryGraph&, const BaseEstimator&) override { return 0; }
      size_t calcEstimate(const JoinPlan&, const QueryGraph&, const BaseEstimator&) override { return 0; }
   };

   FakeOracle fakeOracle;
   std::list<FlatEstimate> estimates;

   public:
   explicit DummyEstimator(const QueryGraph& qg, Database& db) : BaseEstimator(qg, fakeOracle), fakeOracle(db) {}
   virtual ~DummyEstimator() = default;

   Estimate* estimateJoinPlan(const JoinPlan&) override { return &estimates.emplace_back(0, 0); }
   Estimate* estimateBaseTablePlan(const BaseTablePlan&) override { return &estimates.emplace_back(0, 0); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_OPT_ESTIMATORS_CARDINALITY_DUMMYESTIMATOR_HPP
