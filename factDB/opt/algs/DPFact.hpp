#pragma once
// ---------------------------------------------------------------------------------------------------
#include "OptimizerAlgorithms.hpp"
#include "factDB/opt/algs/OptimizerAlgorithms.hpp"
#include "factDB/opt/estimators/FactorizedEstimator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt::algs {
// ---------------------------------------------------------------------------------------------------
class DPFact : public OptimizerAlgorithm {
   protected:
   std::unordered_map<infra::BitSet64, std::list<Plan*>, infra::BitSet64::hasher> planMap;
   FactorizedEstimator estimator;
   OptimizerAlgorithm::Algorithm mode = OptimizerAlgorithm::DPFactBushy;

   public:
   DPFact(QueryGraph& qg, BaseOracle& cardinalityOracle, Algorithm mode_ = OptimizerAlgorithm::DPFactBushy) : OptimizerAlgorithm(qg), estimator(qg, cardinalityOracle), mode(mode_) {}
   ~DPFact() = default;

   void generateOptimalPlan() override;
   Plan* getOptimalPlan() const override;

   void addParetoEfficientPlans(const infra::BitSet64& leftProblem, const infra::BitSet64& rightProblem, std::list<Plan*>& otherPlans);
   Plan* chooseBetterPlan(Plan* leftPlan, Plan* rightPlan, algebra::JoinMode joinMode, std::list<Plan*>& alternativePlans);
   BaseEstimator& getEstimator() override { return estimator; }

   protected:
   void prepareBaseTables();
   void createJoins(infra::BitSet64& leftProblem, infra::BitSet64& rightProblem);
   infra::BitSet64 getPredecessorRelations(const Plan& plan, const infra::BitSetVar& interestingSets);
   void iteratePlans(std::vector<std::vector<infra::BitSet64>>& sizes, size_t totalSize, size_t leftSize);
};
// ---------------------------------------------------------------------------------------------------
class DPFactLeftDeep : public DPFact {
   public:
   DPFactLeftDeep(QueryGraph& qg, BaseOracle& cardinalityOracle) : DPFact(qg, cardinalityOracle, OptimizerAlgorithm::DPFactLeftDeep) {}
   ~DPFactLeftDeep() = default;

   void generateOptimalPlan() override;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt::algs
// ---------------------------------------------------------------------------------------------------
