// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/JoinMode.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Reference.hpp"
#include "factDB/algebra/visitors/CodegenFactorized.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/opt/CardinalityOracle.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/algs/OptimizerAlgorithms.hpp"
#include "factDB/opt/estimators/DummyEstimator.hpp"
#include "factDB/opt/estimators/FactorizedEstimator.hpp"
#include "factDB/queryc/ExecutionMode.hpp"
#include "factDB/queryc/QueryParseContext.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "gtest/gtest.h"
#include "test/factDB/opt/QueryGraphBuilder.hpp"
#include <iostream>
#include <unordered_map>
// ---------------------------------------------------------------------------------------------------
using namespace std;
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
[[maybe_unused]] void printPlan(const Plan& plan, QueryGraph& qg, Database& database) {
   auto set = SettingBase::getSetting("codegen.executionMode")->setTemporary(queryc::ExecutionMode::Graphviz);
   opt::DummyEstimator estimator(qg, database);
   std::unique_ptr<algebra::Operator> algebraPlan = plan.generateOperatorPlan(qg, estimator);
   factDB::queryc::QueryParseContext context(database);
   auto printer = std::make_unique<algebra::Print>(std::move(algebraPlan));

   std::stringstream ss;
   FileWriter fw(ss);
   auto codegenFactorized = std::make_unique<algebra::visitors::CodegenFactorized>(database, fw);
   auto allIUs = printer->collectIUs();
   printer->prepare(*codegenFactorized, allIUs, nullptr); // need all IUs since we want see the position of each IU, which is possibly used later. todo we can only look at required filtered by currently available ius
   // auto& factorizedTree = codegenFactorized->getFactorizedTree(*printer);
   // factorizedTree.genGraphviz(false);

   context.compileAndExecute(std::move(printer), {}, database, std::cout, false, true);
}
// ---------------------------------------------------------------------------------------------------
const algebra::InnerJoin* castInnerJoinIgnoreReference(const algebra::Operator* frontOp) {
   while (frontOp->getType() == algebra::Operator::Reference)
      frontOp = &static_cast<const algebra::Reference*>(frontOp)->getChild();
   return algebra::InnerJoin::dynCast(frontOp);
}
// ---------------------------------------------------------------------------------------------------
struct TestOracleCreator : public opt::BaseOracle {
   std::vector<size_t> relationSizes;
   std::unordered_map<std::string_view, size_t> relation2idx;

   struct HashTuple {
      size_t operator()(const std::tuple<size_t, size_t>& x) const { return get<0>(x) ^ get<1>(x); }
   };

   std::unordered_map<std::tuple<size_t, size_t>, double, HashTuple> selectivities;

   TestOracleCreator(Database& db, std::vector<size_t> relationSizes_) : BaseOracle(db), relationSizes(std::move(relationSizes_)) {}
   ~TestOracleCreator() = default;

   TestOracleCreator& addSelectivity(size_t rel1, size_t rel2, double selectivity) { // NOLINT(bugprone-easily-swappable-parameters)
      if (rel1 > rel2) std::swap(rel1, rel2);
      assert(rel1 <= rel2 && rel2 < relationSizes.size());
      auto relationTuple = std::make_tuple(rel1, rel2);
      assert(!selectivities.contains(relationTuple));
      selectivities[relationTuple] = selectivity;
      return *this;
   }

   void registerQueryGraph(QueryGraph& queryGraph) {
      assert(queryGraph.getRelations().size() == relationSizes.size());
      for (size_t idx = 0; idx != queryGraph.getRelations().size(); ++idx) {
         auto& rel = queryGraph.getRelation(idx);
         assert(!relation2idx.contains(rel.alias));
         relation2idx[rel.alias] = idx;
      }
   }

   double selectivity(size_t rel1, size_t rel2) {
      if (rel1 > rel2) std::swap(rel1, rel2);
      assert(rel1 <= rel2 && rel2 < relationSizes.size());

      auto relationTuple = std::make_tuple(rel1, rel2);
      if (selectivities.contains(relationTuple))
         return selectivities[relationTuple];
      else
         return 1.;
   }

   size_t calcEstimate(const BaseTablePlan& plan, const QueryGraph&, const BaseEstimator&) override {
      assert(plan.getRelation() < relationSizes.size());
      return relationSizes[plan.getRelation()];
   }

   size_t calcEstimate(const JoinPlan& plan, const QueryGraph& queryGraph, const BaseEstimator& estimator) override {
      // todo fix query graph input vs implicitly assumed query graph
      double totalSize = 1.;
      infra::BitSet64 seenRelations;
      for (auto relationQG : plan.getCoveredRelations()) {
         auto relation = relation2idx[queryGraph.getRelation(relationQG).alias];
         totalSize *= relationSizes[relation]; // NOLINT
         for (auto otherRel : seenRelations) {
            totalSize *= selectivity(relation, otherRel);
         }
         seenRelations.insert(relation);
      }

      auto opPlan = plan.generateOperatorPlan(queryGraph, estimator);
      std::list<std::pair<const algebra::Operator*, const Plan*>> todoOperators;
      todoOperators.emplace_back(opPlan.get(), &plan);
      while (!todoOperators.empty()) {
         auto joinOp = castInnerJoinIgnoreReference(todoOperators.front().first);
         auto joinPlan = JoinPlan::dynCast(todoOperators.front().second);
         assert((joinOp == nullptr) == (joinPlan == nullptr));
         todoOperators.pop_front();

         if (joinOp != nullptr && joinOp->getJoinType() == algebra::JoinType::RightSemi) {
            for (auto relationQG : joinPlan->getLeft()->getCoveredRelations()) {
               auto relation = relation2idx[queryGraph.getRelation(relationQG).alias];
               assert(relation < relationSizes.size());
               totalSize /= relationSizes[relation]; // NOLINT
            }
            todoOperators.emplace_back(joinOp->getRightChild().get(), joinPlan->getRight());
         } else if (joinOp != nullptr) {
            todoOperators.emplace_back(joinOp->getLeftChild().get(), joinPlan->getLeft());
            todoOperators.emplace_back(joinOp->getRightChild().get(), joinPlan->getRight());
         } // else nothing to do for base table plans
      }

      return totalSize; // NOLINT
   }

   size_t getMinCostLin(QueryGraph& qg) {
      FactorizedEstimator estimator(qg, *this);
      uint64_t minCost = std::numeric_limits<uint64_t>::max();
      std::vector<BaseTablePlan> baseTablePlan;
      std::list<JoinPlan> joinPlans;
      baseTablePlan.reserve(relationSizes.size());

      auto baseTableLess = [](const BaseTablePlan* bt1, const BaseTablePlan* bt2) {
         return bt1->getRelation() < bt2->getRelation();
      };

      std::vector<BaseTablePlan*> baseTablePermutation;
      for (size_t i = 0; i != relationSizes.size(); ++i) {
         auto* curPtr = &baseTablePlan.emplace_back(i, estimator);
         baseTablePermutation.emplace_back(curPtr);
      }

      do { // generate estimate for all possible left deep plans, therefore generate all possible permutations of the relations
         // generate linear, left deep plan on these permutations & estimate the cost of the plans
         infra::BitSet64 insertModes;
         do {
            Plan* curLeft = baseTablePermutation.front();
            for (size_t idx = 1; idx != baseTablePermutation.size(); ++idx) {
               auto curInsertMode = insertModes.contains(idx - 1) ? algebra::JoinMode::BottomInsert : algebra::JoinMode::TopInsert;
               if (idx == baseTablePermutation.size() - 1) { assert(curInsertMode == algebra::JoinMode::TopInsert && "in last join Bottom Insert does not make sense (for now)"); }
               if (!curLeft->hasJoinEdgeWith(*baseTablePermutation[idx], qg))
                  break;
               auto newPlan = Plan::makeJoin(curLeft, baseTablePermutation[idx], estimator, curInsertMode);
               curLeft = &joinPlans.emplace_back(std::move(newPlan));
            }

            if (joinPlans.empty() || joinPlans.back().getCoveredRelations() != infra::BitSet64(0, relationSizes.size(), infra::BitSet64::RangeEnum::Range)) // not able to generate the final plan
               break;

            minCost = std::min(minCost, static_cast<const FactorizedEstimate*>(joinPlans.back().getEstimate())->cost);

            joinPlans.clear();
            insertModes = insertModes.nextPermutation(relationSizes.size() - 2);
         } while (!insertModes.empty());
      } while (std::next_permutation(baseTablePermutation.begin(), baseTablePermutation.end(), baseTableLess));

      return minCost;
   }
};
// ---------------------------------------------------------------------------------------------------
std::tuple<TestOracleCreator, QueryGraphBuilder> genOracleGDBExample(Database& db) {
   TestOracleCreator oracle(db, {100, 1000, 10});
   oracle.addSelectivity(0, 1, 1. / 100)
      .addSelectivity(0, 2, 1. / 100)
      .addSelectivity(1, 2, 1. / 10);
   QueryGraphBuilder qgb(db, 3);
   [[maybe_unused]] auto [ra, sa] = qgb.addJoin(0, 1);
   [[maybe_unused]] auto [sb, tb] = qgb.addJoin(1, 2);
   [[maybe_unused]] auto [ra2, tc] = qgb.addJoin(0, 2, ra);
   return {std::move(oracle), std::move(qgb)};
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_opt_DP, GDBExample) {
   Database db = DatabaseLoadUtil::genTestDB();
   auto [oracle, qgb] = genOracleGDBExample(db);
   auto qg = qgb.getQueryGraph();
   oracle.registerQueryGraph(qg);

   auto optimizer = OptimizerAlgorithm::executeAlgorithm(OptimizerAlgorithm::DPSizeLinear, qg, oracle);
   const auto& optPlan = *optimizer->getOptimalPlan();

   ASSERT_EQ(optPlan.getType(), PlanType::Join);
   ASSERT_EQ(FlatEstimate::cast(optPlan.getEstimate())->cost, 20);
   const auto& joinPlan1 = *JoinPlan::dynCast(&optPlan);
   auto leftPlan = JoinPlan::dynCast(joinPlan1.getLeft());
   auto rightPlan = BaseTablePlan::dynCast(joinPlan1.getRight());
   ASSERT_NE(JoinPlan::dynCast(joinPlan1.getLeft()), nullptr);
   ASSERT_NE(BaseTablePlan::dynCast(joinPlan1.getRight()), nullptr);
   ASSERT_EQ(rightPlan->getRelation(), 1);

   ASSERT_NE(BaseTablePlan::dynCast(leftPlan->getLeft()), nullptr);
   ASSERT_NE(BaseTablePlan::dynCast(leftPlan->getRight()), nullptr);

   auto leftRel = BaseTablePlan::dynCast(leftPlan->getLeft())->getRelation();
   auto rightRel = BaseTablePlan::dynCast(leftPlan->getRight())->getRelation();
   ASSERT_TRUE(leftRel == 0 || rightRel == 0);
   ASSERT_TRUE(leftRel == 2 || rightRel == 2);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_opt_DP, RightSemi) {
   Database db = DatabaseLoadUtil::genTestDB();
   TestOracleCreator oracle(db, {100, 100, 100});
   oracle.addSelectivity(0, 1, 1. / 10)
      .addSelectivity(1, 2, 1. / 10);
   QueryGraphBuilder qgb(db, 3);
   qgb.addJoin(1, 0, infra::BitSet64{1, 2}, infra::BitSet64{0}, algebra::JoinType::RightSemi);
   qgb.addJoin(1, 2);
   auto qg = qgb.getQueryGraph();
   qg.printQueryGraph(fw::nullStream);
   oracle.registerQueryGraph(qg);

   auto optimizer = OptimizerAlgorithm::executeAlgorithm(OptimizerAlgorithm::DPSizeLinear, qg, oracle);
   const auto& optPlan = *optimizer->getOptimalPlan();

   ASSERT_EQ(optPlan.getType(), PlanType::Join);
   DummyEstimator estimator(qg, db);
   ASSERT_NO_THROW(optPlan.generateOperatorPlan(qg, estimator)); // NOLINT(clang-diagnostic-unused-result)

   ASSERT_EQ(FlatEstimate::cast(optPlan.getEstimate())->cost, 1001);
   const auto& joinPlan1 = *JoinPlan::dynCast(&optPlan);
   auto leftPlan = JoinPlan::dynCast(joinPlan1.getLeft());
   auto rightPlan = BaseTablePlan::dynCast(joinPlan1.getRight());
   ASSERT_NE(JoinPlan::dynCast(joinPlan1.getLeft()), nullptr);
   ASSERT_NE(BaseTablePlan::dynCast(joinPlan1.getRight()), nullptr);
   ASSERT_EQ(rightPlan->getRelation(), 0);

   ASSERT_NE(BaseTablePlan::dynCast(leftPlan->getLeft()), nullptr);
   ASSERT_NE(BaseTablePlan::dynCast(leftPlan->getRight()), nullptr);

   auto leftRel = BaseTablePlan::dynCast(leftPlan->getLeft())->getRelation();
   auto rightRel = BaseTablePlan::dynCast(leftPlan->getRight())->getRelation();
   ASSERT_TRUE(leftRel == 1 || rightRel == 1);
   ASSERT_TRUE(leftRel == 2 || rightRel == 2);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_opt_DPFact, GDBExample) {
   Database db = DatabaseLoadUtil::genTestDB();
   auto [oracle, qgb] = genOracleGDBExample(db);
   auto qg = qgb.getQueryGraph();
   oracle.registerQueryGraph(qg);

   auto optimizer = OptimizerAlgorithm::executeAlgorithm(OptimizerAlgorithm::DPFactLeftDeep, qg, oracle);
   const auto& optPlan = *optimizer->getOptimalPlan();

   ASSERT_EQ(oracle.getMinCostLin(qg), static_cast<const FactorizedEstimate*>(optPlan.getEstimate())->cost);

   ASSERT_EQ(optPlan.getType(), PlanType::Join);
   const auto& joinPlan1 = *JoinPlan::dynCast(&optPlan);
   auto leftPlan = JoinPlan::dynCast(joinPlan1.getLeft());
   auto rightPlan = BaseTablePlan::dynCast(joinPlan1.getRight());
   ASSERT_NE(JoinPlan::dynCast(joinPlan1.getLeft()), nullptr);
   ASSERT_NE(BaseTablePlan::dynCast(joinPlan1.getRight()), nullptr);
   ASSERT_EQ(rightPlan->getRelation(), 1);

   ASSERT_NE(BaseTablePlan::dynCast(leftPlan->getLeft()), nullptr);
   ASSERT_NE(BaseTablePlan::dynCast(leftPlan->getRight()), nullptr);

   auto leftRel = BaseTablePlan::dynCast(leftPlan->getLeft())->getRelation();
   auto rightRel = BaseTablePlan::dynCast(leftPlan->getRight())->getRelation();
   ASSERT_TRUE(leftRel == 0 || rightRel == 0);
   ASSERT_TRUE(leftRel == 2 || rightRel == 2);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_opt_DPFact, Chain2Hop) {
   Database db = DatabaseLoadUtil::genTestDB();
   TestOracleCreator oracle(db, {100, 1000, 10});
   oracle.addSelectivity(0, 1, 1. / 1000)
      .addSelectivity(1, 2, 1. / 10);
   QueryGraphBuilder qgb(db, 3);
   [[maybe_unused]] auto [ra, sa] = qgb.addJoin(0, 1);
   [[maybe_unused]] auto [sb, tb] = qgb.addJoin(1, 2);
   auto qg = qgb.getQueryGraph();
   oracle.registerQueryGraph(qg);

   auto optimizer = OptimizerAlgorithm::executeAlgorithm(OptimizerAlgorithm::DPFactLeftDeep, qg, oracle);
   const auto& optPlan = *optimizer->getOptimalPlan();

   ASSERT_EQ(oracle.getMinCostLin(qg), static_cast<const FactorizedEstimate*>(optPlan.getEstimate())->cost);

   ASSERT_EQ(optPlan.getType(), PlanType::Join);
   const auto& joinPlan1 = *JoinPlan::dynCast(&optPlan);
   auto leftPlan = JoinPlan::dynCast(joinPlan1.getLeft());
   auto rightPlan = BaseTablePlan::dynCast(joinPlan1.getRight());
   ASSERT_NE(JoinPlan::dynCast(joinPlan1.getLeft()), nullptr);
   ASSERT_NE(BaseTablePlan::dynCast(joinPlan1.getRight()), nullptr);
   ASSERT_EQ(rightPlan->getRelation(), 2);

   ASSERT_NE(BaseTablePlan::dynCast(leftPlan->getLeft()), nullptr);
   ASSERT_NE(BaseTablePlan::dynCast(leftPlan->getRight()), nullptr);

   auto leftRel = BaseTablePlan::dynCast(leftPlan->getLeft())->getRelation();
   auto rightRel = BaseTablePlan::dynCast(leftPlan->getRight())->getRelation();
   ASSERT_TRUE(leftRel == 0 || rightRel == 0);
   ASSERT_TRUE(leftRel == 1 || rightRel == 1);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_opt_DPFact, Chain2HopSym) {
   Database db = DatabaseLoadUtil::genTestDB();
   TestOracleCreator oracle(db, {10, 1000, 100});
   oracle.addSelectivity(0, 1, 1. / 10)
      .addSelectivity(1, 2, 1. / 1000);
   QueryGraphBuilder qgb(db, 3);
   [[maybe_unused]] auto [ra, sa] = qgb.addJoin(0, 1);
   [[maybe_unused]] auto [sb, tb] = qgb.addJoin(1, 2);
   auto qg = qgb.getQueryGraph();
   oracle.registerQueryGraph(qg);

   auto optimizer = OptimizerAlgorithm::executeAlgorithm(OptimizerAlgorithm::DPFactLeftDeep, qg, oracle);
   const auto& optPlan = *optimizer->getOptimalPlan();

   ASSERT_EQ(oracle.getMinCostLin(qg), static_cast<const FactorizedEstimate*>(optPlan.getEstimate())->cost);

   ASSERT_EQ(optPlan.getType(), PlanType::Join);
   const auto& joinPlan1 = *JoinPlan::dynCast(&optPlan);
   auto leftPlan = JoinPlan::dynCast(joinPlan1.getLeft());
   auto rightPlan = BaseTablePlan::dynCast(joinPlan1.getRight());
   ASSERT_NE(JoinPlan::dynCast(joinPlan1.getLeft()), nullptr);
   ASSERT_NE(BaseTablePlan::dynCast(joinPlan1.getRight()), nullptr);
   ASSERT_EQ(rightPlan->getRelation(), 0);

   ASSERT_NE(BaseTablePlan::dynCast(leftPlan->getLeft()), nullptr);
   ASSERT_NE(BaseTablePlan::dynCast(leftPlan->getRight()), nullptr);

   auto leftRel = BaseTablePlan::dynCast(leftPlan->getLeft())->getRelation();
   auto rightRel = BaseTablePlan::dynCast(leftPlan->getRight())->getRelation();
   ASSERT_TRUE(leftRel == 1 || rightRel == 1);
   ASSERT_TRUE(leftRel == 2 || rightRel == 2);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_opt_DPFact, EnforceBottomInsert) {
   GTEST_SKIP();
   Database db = DatabaseLoadUtil::genTestDB();
   TestOracleCreator oracle(db, {10000, 10, 10, 100000});
   oracle.addSelectivity(0, 1, 1 - 1. / 100000)
      .addSelectivity(1, 2, 1. / 100)
      .addSelectivity(2, 3, 1 - 1. / 1000000);
   QueryGraphBuilder qgb(db, 4);
   qgb.addJoin(0, 1);
   qgb.addJoin(1, 2);
   qgb.addJoin(2, 3);
   auto qg = qgb.getQueryGraph();
   oracle.registerQueryGraph(qg);

   auto optimizer = OptimizerAlgorithm::executeAlgorithm(OptimizerAlgorithm::DPFactLeftDeep, qg, oracle);
   const auto& optPlan = *optimizer->getOptimalPlan();

   FactorizedEstimate nullEst;
   FactorizedEstimator estimator(qg, oracle);
   BaseTablePlan bt0(0, estimator), bt1(1, estimator), bt2(2, estimator), bt3(3, estimator);
   auto j1 = JoinPlan::makeJoin(&bt1, &bt2, estimator, algebra::JoinMode::TopInsert);
   auto j2 = JoinPlan::makeJoin(&j1, &bt3, estimator, algebra::JoinMode::BottomInsert);
   auto j3 = JoinPlan::makeJoin(&j2, &bt0, estimator, algebra::JoinMode::BottomInsert);

   auto* j1Plan = JoinPlan::dynCast(&optPlan);

   ASSERT_EQ(oracle.getMinCostLin(qg), static_cast<const FactorizedEstimate*>(j3.getEstimate())->cost);

   ASSERT_EQ(static_cast<const FactorizedEstimate*>(optPlan.getEstimate())->cost,
             static_cast<const FactorizedEstimate*>(j3.getEstimate())->cost);

   ASSERT_NE(j1Plan, nullptr);
   auto* j2Plan = JoinPlan::dynCast(j1Plan->getLeft());
   ASSERT_NE(j2Plan, nullptr);

   auto* j3Plan = JoinPlan::dynCast(j2Plan->getLeft());
   ASSERT_NE(j2Plan, nullptr);

   auto* t0Plan = BaseTablePlan::dynCast(j2Plan->getRight());
   auto* t1Plan = BaseTablePlan::dynCast(j3Plan->getLeft());
   auto* t2Plan = BaseTablePlan::dynCast(j3Plan->getRight());
   auto* t3Plan = BaseTablePlan::dynCast(j1Plan->getRight());

   ASSERT_NE(t0Plan, nullptr);
   ASSERT_NE(t1Plan, nullptr);
   ASSERT_NE(t2Plan, nullptr);
   ASSERT_NE(t3Plan, nullptr);
   ASSERT_EQ(t0Plan->getRelation(), 3);
   ASSERT_EQ(t1Plan->getRelation(), 1);
   ASSERT_EQ(t2Plan->getRelation(), 2);
   ASSERT_EQ(t3Plan->getRelation(), 0);

   ASSERT_EQ(j1Plan->getJoinMode(), algebra::JoinMode::TopInsert);
   ASSERT_EQ(j2Plan->getJoinMode(), algebra::JoinMode::BottomInsert);
   ASSERT_EQ(j3Plan->getJoinMode(), algebra::JoinMode::TopInsert);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_opt_DPFact, JoinCondition) {
   Database db = DatabaseLoadUtil::genTestDB();
   TestOracleCreator oracle(db, {10000, 10, 10, 100000});
   oracle.addSelectivity(0, 1, 0.5)
      .addSelectivity(1, 2, 0.5)
      .addSelectivity(2, 3, 0.5);
   QueryGraphBuilder qgb(db, 4);
   [[maybe_unused]] auto [iu0, iu1] = qgb.addJoin(0, 1);
   [[maybe_unused]] auto [iu1b, iu2] = qgb.addJoin(1, 2, iu1);
   [[maybe_unused]] auto [iu2b, iu3] = qgb.addJoin(2, 3, iu2);
   auto qg = qgb.getQueryGraph();
   oracle.registerQueryGraph(qg);
   FactorizedEstimator estimator(qg, oracle);

   std::vector<BaseTablePlan> baseTablePlans;
   for (size_t i = 0; i < 4; ++i)
      baseTablePlans.emplace_back(i, estimator);

   auto checkPlan = [&baseTablePlans, &estimator, &qg, iu2](JoinPlan& jp, const IU* shouldLeftIU) {
      JoinPlan topJoin = Plan::makeJoin(&jp, &baseTablePlans[2], estimator, algebra::JoinMode::TopInsert);
      auto operatorPlan = topJoin.generateOperatorPlan(qg, estimator);
      auto innerJoin = castInnerJoinIgnoreReference(operatorPlan.get());
      ASSERT_NE(innerJoin, nullptr);
      ASSERT_EQ(innerJoin->getJoinCondition().size(), 1);
      auto firstCondition = innerJoin->getJoinCondition().get(0);

      if (firstCondition.get_left() != *shouldLeftIU) firstCondition.swap();
      ASSERT_EQ(firstCondition.get_left(), *shouldLeftIU);
      ASSERT_EQ(firstCondition.get_right(), *iu2); // NOLINT
   };

   {
      JoinPlan jp = Plan::makeJoin(&baseTablePlans[0], &baseTablePlans[1], estimator, algebra::JoinMode::TopInsert);
      checkPlan(jp, iu1);
   }
   {
      JoinPlan jp = Plan::makeJoin(&baseTablePlans[0], &baseTablePlans[1], estimator, algebra::JoinMode::BottomInsert);
      checkPlan(jp, iu0);
   }
   {
      JoinPlan jp = Plan::makeJoin(&baseTablePlans[1], &baseTablePlans[0], estimator, algebra::JoinMode::TopInsert);
      checkPlan(jp, iu0);
   }
   {
      JoinPlan jp = Plan::makeJoin(&baseTablePlans[1], &baseTablePlans[0], estimator, algebra::JoinMode::BottomInsert);
      checkPlan(jp, iu1);
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------