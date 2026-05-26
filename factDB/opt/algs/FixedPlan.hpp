#ifndef H_FACTDB_FACTDB_OPT_ALGS_FIXED_HPP
#define H_FACTDB_FACTDB_OPT_ALGS_FIXED_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/algs/OptimizerAlgorithms.hpp"
#include "factDB/opt/estimators/FactorizedEstimator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt::algs {
// ---------------------------------------------------------------------------------------------------
class FixedPlan : public OptimizerAlgorithm {
   protected:
   Plan* optimalPlan = nullptr;
   FactorizedEstimator estimator;
   size_t pos = 0;
   std::string planStr;

   private:
   bool peek(char c) const;
   void expect(char c);
   void skipSpaces();
   uint32_t parseUInt32();
   algebra::JoinMode parseJoinMode();
   std::string parseAlias();
   opt::Plan* parseExpr();

   public:
   FixedPlan(QueryGraph& qg, BaseOracle& oracle) : OptimizerAlgorithm(qg), estimator(queryGraph, oracle) {}
   BaseEstimator& getEstimator() override { return estimator; }
   ~FixedPlan() override = default;

   void generateOptimalPlan() override;
   Plan* getOptimalPlan() const override;

   void setPlan(std::string cachedPlan) { planStr = std::move(cachedPlan); }

   protected:
   Plan* getPlan(infra::BitSet64& curBitSet) const;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt::algs
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_OPT_ALGS_FIXED_HPP
