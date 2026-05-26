#ifndef H_FACTDB_FACTDB_OPT_ALGS_ANYPLAN_HPP
#define H_FACTDB_FACTDB_OPT_ALGS_ANYPLAN_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/algs/OptimizerAlgorithms.hpp"
#include "factDB/opt/estimators/DummyEstimator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt::algs {
// ---------------------------------------------------------------------------------------------------
class AnyPlan : public OptimizerAlgorithm {
   protected:
   DummyEstimator estimator;
   std::unordered_map<infra::BitSet64, Plan*, infra::BitSet64::hasher> planMap;

   public:
   AnyPlan(QueryGraph& qg, BaseOracle& oracle) : OptimizerAlgorithm(qg), estimator(qg, oracle.getDatabase()) {}
   ~AnyPlan() = default;

   void generateOptimalPlan() override;

   [[nodiscard]] inline BaseEstimator& getEstimator() override { return estimator; }
   [[nodiscard]] inline Plan* getOptimalPlan() const override {
      assert(planMap.size() == 1);
      return planMap.begin()->second;
   }

   protected:
   inline void prepareBaseTables();
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt::algs
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_OPT_ALGS_ANYPLAN_HPP
