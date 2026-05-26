// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/Plan.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/JoinMode.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/estimators/BaseEstimator.hpp"
#include <sstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
size_t planIDCtr = 0;
// ---------------------------------------------------------------------------------------------------
size_t Plan::incPlanId() {
   return planIDCtr++;
}
// ---------------------------------------------------------------------------------------------------
struct PlanGenerator {
   struct IntermediateInfo {
      infra::BitSet64 usedRelations;
      std::unique_ptr<algebra::Operator> operatorPlan;
      IUSet containedIus;
   };

   const QueryGraph& queryGraph;
   const BaseEstimator& estimator;
   const bool useCache = true;

   private:
   IntermediateInfo buildJoin(const JoinPlan& plan);
   IntermediateInfo buildBaseTable(const BaseTablePlan& plan);

   public:
   explicit PlanGenerator(const QueryGraph& qg, const BaseEstimator& estimator_, bool useCache_) : queryGraph(qg), estimator(estimator_), useCache(useCache_) {}

   IntermediateInfo build(const Plan* plan);
};
// ---------------------------------------------------------------------------------------------------
PlanGenerator::IntermediateInfo PlanGenerator::buildBaseTable(const BaseTablePlan& plan) {
   const auto& optimizerRelation = queryGraph.getRelations()[plan.getRelation()];

   std::unique_ptr<algebra::Operator> tableScan = std::make_unique<algebra::TableScan>(queryGraph.getDatabase(), optimizerRelation.relation, std::string(optimizerRelation.alias));

   // generate all filter predicates applied to the table
   for (auto* p : queryGraph.getFilterPredicates(plan.getRelation())) {
      auto expr = p->copy(tableScan->collectIUs());
      tableScan = std::make_unique<algebra::Selection>(std::move(tableScan), std::move(expr));
   }

   IUSet ius{tableScan->collectIUs()};
   return {infra::BitSet64{plan.getRelation()}, std::move(tableScan), ius};
}
// ---------------------------------------------------------------------------------------------------
PlanGenerator::IntermediateInfo PlanGenerator::buildJoin(const JoinPlan& plan) {
   assert(plan.getLeft() != nullptr && plan.getRight() != nullptr);
   auto leftInfo = build(plan.getLeft());
   auto rightInfo = build(plan.getRight());

   assert(leftInfo.usedRelations == plan.getLeft()->getCoveredRelations());
   assert(rightInfo.usedRelations == plan.getRight()->getCoveredRelations());

   auto connectingJoinsRaw = queryGraph.getConnectingJoins(leftInfo.usedRelations, rightInfo.usedRelations, leftInfo.operatorPlan, rightInfo.operatorPlan, estimator);

   std::vector<JoinCondition> joinConditions;
   joinConditions.reserve(connectingJoinsRaw.size());
   [[maybe_unused]] auto checkCrossProduct = SettingBase::getSetting<bool>("algebra.join.checkCrossProduct");
   assert(!checkCrossProduct->get() || !connectingJoinsRaw.empty());

   if constexpr (debugMode) {
      if (!connectingJoinsRaw.empty()) {
         [[maybe_unused]] auto joinType = connectingJoinsRaw.front().joinType;
         for (auto iter = ++connectingJoinsRaw.begin(); iter != connectingJoinsRaw.end(); ++iter) {
            assert(iter->joinType == joinType && "detected different join types, not supported");
         }
      }
   }

   // create the join predicates
   for (auto& cjRaw : connectingJoinsRaw) {
      if (!leftInfo.containedIus.contains(&cjRaw.condition.get_left()))
         cjRaw.condition.swap();

      assert(leftInfo.containedIus.contains(&cjRaw.condition.get_left()));
      assert(rightInfo.containedIus.contains(&cjRaw.condition.get_right()));

      const IU* correspondingLeftIu = *leftInfo.containedIus.find(&cjRaw.condition.get_left());
      const IU* correspondingRightIu = *rightInfo.containedIus.find(&cjRaw.condition.get_right());

      joinConditions.emplace_back(JoinCondition::create(correspondingLeftIu, correspondingRightIu));
   }

   auto join = std::make_unique<algebra::InnerJoin>(std::move(leftInfo.operatorPlan), std::move(rightInfo.operatorPlan), std::make_unique<JoinConditionList>(joinConditions), plan.getJoinMode());
   join->setJoinType(connectingJoinsRaw.empty() ? algebra::JoinType::Inner : connectingJoinsRaw.front().joinType);
   return {leftInfo.usedRelations + rightInfo.usedRelations, std::move(join), leftInfo.containedIus + rightInfo.containedIus};
}
// ---------------------------------------------------------------------------------------------------
PlanGenerator::IntermediateInfo PlanGenerator::build(const Plan* plan) {
   if (auto operatorPlan = queryGraph.getOperatorPlan(*plan); useCache && operatorPlan) {
      auto ius = operatorPlan->collectIUs();
      return {plan->getCoveredRelations(), std::move(operatorPlan), ius};
   }

   switch (plan->getType()) {
      case Join: {
         auto intermediateInfo = buildJoin(static_cast<const JoinPlan&>(*plan));
         if (useCache) queryGraph.insertOperatorPlan(*plan, intermediateInfo.operatorPlan);
         return intermediateInfo;
      }
      case BaseTable: {
         auto intermediateInfo = buildBaseTable(static_cast<const BaseTablePlan&>(*plan));
         if (useCache) queryGraph.insertOperatorPlan(*plan, intermediateInfo.operatorPlan);
         return intermediateInfo;
      }
      case TableScan:
      case Uninitialized:
         unreachable();
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<algebra::Operator> Plan::generateOperatorPlan(const factDB::opt::QueryGraph& queryGraph, const BaseEstimator& estimator, bool useCache) const {
   PlanGenerator planGenerator(queryGraph, estimator, useCache);
   auto [_, operatorPlan, iuSet] = planGenerator.build(this);
   return std::move(operatorPlan);
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<algebra::Operator> Plan::generateCountPlan(const factDB::opt::QueryGraph& queryGraph, const BaseEstimator& estimator, bool useCache) const {
   auto op = generateOperatorPlan(queryGraph, estimator, useCache);
   return std::make_unique<algebra::Count>(std::move(op));
}
// ---------------------------------------------------------------------------------------------------
JoinPlan Plan::makeJoin(factDB::opt::Plan* leftPlan, factDB::opt::Plan* rightPlan, algebra::JoinMode mode) {
   assert(!!leftPlan && !!rightPlan);
   JoinPlan p(*leftPlan, *rightPlan, mode);
   return p;
}
// ---------------------------------------------------------------------------------------------------
JoinPlan Plan::makeJoin(factDB::opt::Plan* leftPlan, factDB::opt::Plan* rightPlan, factDB::opt::BaseEstimator& estimator, algebra::JoinMode mode) {
   return makeJoin(leftPlan, rightPlan, estimator, mode, incPlanId());
}
// ---------------------------------------------------------------------------------------------------
JoinPlan Plan::makeJoin(factDB::opt::Plan* leftPlan, factDB::opt::Plan* rightPlan, factDB::opt::BaseEstimator& estimator, algebra::JoinMode mode, size_t id) {
   assert(!!leftPlan && !!rightPlan);
   JoinPlan p(*leftPlan, *rightPlan, mode);
   p.estimate = estimator.estimatePlan(&p);
   p.planID = id;
   return p;
}
// ---------------------------------------------------------------------------------------------------
bool Plan::hasJoinEdgeWith(const factDB::opt::Plan& other, const factDB::opt::QueryGraph& queryGraph) const {
   return queryGraph.hasJoinEdge(getCoveredRelations(), other.getCoveredRelations());
}
// ---------------------------------------------------------------------------------------------------
BaseTablePlan::BaseTablePlan(unsigned relationParam, BaseEstimator& estimator)
   : Plan(BaseTable, incPlanId()), relation(relationParam) {
   coveredRelations = infra::BitSet64({relation});
   estimate = estimator.estimatePlan(this);
}
// ---------------------------------------------------------------------------------------------------
void BaseTablePlan::print(fw::FileWriter& out, const QueryGraph& qg) const {
   auto& rel = qg.getRelation(relation);
   out << rel.relation << " " << rel.alias;
}
// ---------------------------------------------------------------------------------------------------
std::string getJoinModeString(algebra::JoinMode mode) {
   switch (mode) {
      case algebra::JoinMode::TopInsert: return "TI";
      case algebra::JoinMode::BottomInsert: return "BI";
      default: return "";
   }
}
// ---------------------------------------------------------------------------------------------------
void JoinPlan::print(fw::FileWriter& out, const QueryGraph& qg) const {
   out << "(";
   getLeft()->print(out, qg);
   out << ")" << getJoinModeString(getJoinMode()) << "(";
   getRight()->print(out, qg);
   out << ")";
}
// ---------------------------------------------------------------------------------------------------
void Plan::print(fw::FileWriter& out, const QueryGraph& qg) const {
   if (getType() == BaseTable)
      static_cast<const BaseTablePlan*>(this)->print(out, qg);
   else {
      assert(getType() == Join);
      static_cast<const JoinPlan*>(this)->print(out, qg);
   }
}
// ---------------------------------------------------------------------------------------------------
std::string Plan::str(const QueryGraph& qg) const {
   std::stringstream ss;
   FileWriter out(ss, FileWriter::NoFinalNewline);
   print(out, qg);
   return ss.str();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------