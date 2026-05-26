#ifndef H_FACTDB_FACTDB_OPT_ALGS_DP_HPP
#define H_FACTDB_FACTDB_OPT_ALGS_DP_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/algs/OptimizerAlgorithms.hpp"
#include "factDB/opt/estimators/FlatEstimator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt::algs {
// ---------------------------------------------------------------------------------------------------
class DPSize : public OptimizerAlgorithm {
   protected:
   FlatEstimator estimator;
   std::unordered_map<infra::BitSet64, Plan*, infra::BitSet64::hasher> planMap;

   public:
   DPSize(QueryGraph& qg, BaseOracle& oracle) : OptimizerAlgorithm(qg), estimator(qg, oracle) {}
   ~DPSize() = default;

   void generateOptimalPlan() override;
   Plan* getOptimalPlan() const override;
   BaseEstimator& getEstimator() override { return estimator; }

   void printDPTable(std::vector<std::vector<infra::BitSet64>> sizes) const;

   protected:
   void prepareBaseTables();
   Plan* getPlan(infra::BitSet64& curBitSet) const;
   void createJoin(infra::BitSet64& leftProblem, infra::BitSet64& rightProblem);
   Plan* chooseBetterPlan(infra::BitSet64& leftProblem, infra::BitSet64& rightProblem, Plan* alternativePlan);
};
// ---------------------------------------------------------------------------------------------------
class DPSizeLinear : public DPSize {
   public:
   DPSizeLinear(QueryGraph& qg, BaseOracle& oracle) : DPSize(qg, oracle) {}
   ~DPSizeLinear() = default;

   public:
   void generateOptimalPlan() override;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt::algs
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_OPT_ALGS_DP_HPP
