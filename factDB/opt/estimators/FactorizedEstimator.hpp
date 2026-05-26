#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/estimators/BaseEstimator.hpp"
#include <list>
#include <unordered_map>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra {
// ---------------------------------------------------------------------------------------------------
class BitSetVar;
class BitSet64;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB::factorized_table_generator { class FactorizedStructure; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class FactorizedEstimator : public BaseEstimator {
   private:
   std::list<FactorizedEstimate> estimates;

   protected:
   [[nodiscard]] bool comparable(const Plan& p1, const Plan& p2) const;
   uint64_t executeSemiJoinPlan(const Plan& currentPlan, const infra::BitSet64& requiredRelations, const infra::BitSet64& semiJoinedRelations);

   public:
   explicit FactorizedEstimator(const QueryGraph& qg, BaseOracle& cardinalityOracle) : BaseEstimator(qg, cardinalityOracle) {}
   ~FactorizedEstimator() override = default;

   [[nodiscard]] infra::BitSetVar getInterestingClasses(const infra::BitSet64& relations) const;
   [[nodiscard]] std::vector<infra::BitSet64> getEquivalenceClass2Relations(const Plan& plan) const;

   Estimate* estimateBaseTablePlan(const BaseTablePlan& baseTablePlan) override;
   Estimate* estimateJoinPlan(const JoinPlan& plan) override;

   JoinCondition optimizeJoiningRelations(std::unique_ptr<algebra::Operator>& leftOperator, std::unique_ptr<algebra::Operator>& rightOperator, const IUSet& leftIUs, const IUSet& rightIUs) const override;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------