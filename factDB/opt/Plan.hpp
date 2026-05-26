#ifndef H_FACTDB_FACTDB_OPT_PLAN_HPP
#define H_FACTDB_FACTDB_OPT_PLAN_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/BitSet.hpp"
#include "factDB/queryc/writer/FileWriter.hpp"
#include <memory>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
class Operator;
enum class JoinMode : std::uint8_t;
enum class JoinType : std::uint8_t;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class QueryGraph;
struct Estimate;
class JoinPlan;
class BaseEstimator;
// ---------------------------------------------------------------------------------------------------
enum PlanType { Uninitialized,
                Join,
                TableScan,
                BaseTable };
// ---------------------------------------------------------------------------------------------------
class Plan {
   size_t planID;

   protected:
   Plan* left = nullptr;
   Plan* right = nullptr;
   PlanType type = Uninitialized;

   infra::BitSet64 coveredRelations;

   Estimate* estimate = nullptr;

   protected:
   Plan(PlanType t, size_t planID_) : planID(planID_), type(t) {}
   // Plan(PlanType t, Plan& leftPlan, Plan& rightPlan) : planID(incPlanId()), left(&leftPlan), right(&rightPlan), type(t), coveredRelations(leftPlan.coveredRelations + rightPlan.coveredRelations) {}
   Plan(PlanType t, Plan& leftPlan, Plan& rightPlan, size_t planID_) : planID(planID_), left(&leftPlan), right(&rightPlan), type(t), coveredRelations(leftPlan.coveredRelations + rightPlan.coveredRelations) {}

   public:
   constexpr Plan() noexcept = default;

   [[nodiscard]] PlanType getType() const { return type; }
   [[nodiscard]] const infra::BitSet64 getCoveredRelations() const { return coveredRelations; }
   [[nodiscard]] const Estimate* getEstimate() const { return estimate; }
   [[nodiscard]] Estimate* getEstimate() { return estimate; }

   [[nodiscard]] bool hasJoinEdgeWith(const Plan& other, const QueryGraph& queryGraph) const;

   [[nodiscard]] std::unique_ptr<algebra::Operator> generateOperatorPlan(const QueryGraph& qg, const BaseEstimator& estimator, bool useCache = true) const;
   [[nodiscard]] std::unique_ptr<algebra::Operator> generateCountPlan(const QueryGraph& qg, const BaseEstimator& estimator, bool useCache = true) const;

   void print(fw::FileWriter& out, const QueryGraph& qg) const;
   std::string str(const QueryGraph& qg) const;
   size_t getPlanID() const { return planID; }

   static JoinPlan makeJoin(Plan* leftPlan, Plan* rightPlan, algebra::JoinMode mode);
   static JoinPlan makeJoin(Plan* leftPlan, Plan* rightPlan, BaseEstimator& estimator, algebra::JoinMode mode);
   static JoinPlan makeJoin(Plan* leftPlan, Plan* rightPlan, BaseEstimator& estimator, algebra::JoinMode mode, size_t previousID);

   protected:
   static size_t incPlanId();
};
// ---------------------------------------------------------------------------------------------------
class JoinPlan : public Plan {
   friend class FactorizedEstimator;
   algebra::JoinMode joinMode = static_cast<algebra::JoinMode>(0); // = algebra::JoinMode::TopInsert;

   public:
   JoinPlan(Plan& leftPlan, Plan& rightPlan, algebra::JoinMode mode) : Plan(PlanType::Join, leftPlan, rightPlan, incPlanId()), joinMode(mode) {}
   JoinPlan(Plan& leftPlan, Plan& rightPlan, algebra::JoinMode mode, size_t planID_) : Plan(PlanType::Join, leftPlan, rightPlan, planID_), joinMode(mode) {}
   JoinPlan() : Plan(PlanType::Join, incPlanId()){};

   [[nodiscard]] Plan* getLeft() const { return left; }
   [[nodiscard]] Plan* getRight() const { return right; }

   [[nodiscard]] algebra::JoinMode getJoinMode() const { return joinMode; }

   void print(fw::FileWriter& out, const QueryGraph& qg) const;

   static const JoinPlan* dynCast(const Plan* plan) { return (plan && (plan->getType() == Join)) ? static_cast<const JoinPlan*>(plan) : nullptr; }
   static JoinPlan* dynCast(Plan* plan) { return (plan && (plan->getType() == Join)) ? static_cast<JoinPlan*>(plan) : nullptr; }
};
// ---------------------------------------------------------------------------------------------------
class BaseTablePlan : public Plan {
   unsigned relation;

   public:
   BaseTablePlan(unsigned relationParam) : Plan(BaseTable, incPlanId()), relation(relationParam) { coveredRelations = infra::BitSet64({relation}); }
   BaseTablePlan(unsigned relationParam, BaseEstimator& estimator);

   [[nodiscard]] unsigned getRelation() const { return relation; }

   void print(fw::FileWriter& out, const QueryGraph& qg) const;

   static const BaseTablePlan* dynCast(const Plan* plan) { return (plan && (plan->getType() == BaseTable)) ? static_cast<const BaseTablePlan*>(plan) : nullptr; }
   static BaseTablePlan* dynCast(Plan* plan) { return (plan && (plan->getType() == BaseTable)) ? static_cast<BaseTablePlan*>(plan) : nullptr; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_OPT_PLAN_HPP
