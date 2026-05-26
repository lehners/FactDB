// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Operator.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/algebra/visitors/GraphvizQueryPlan.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/estimators/DummyEstimator.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "gtest/gtest.h"
#include "test/factDB/opt/QueryGraphBuilder.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace factDB::opt;
using namespace factDB;
using namespace std;
// ---------------------------------------------------------------------------------------------------
TEST(factDB_opt_CardinalityOracle, Signature) {
   auto tmpSet = SettingBase::getSetting<bool>("algebra.join.checkCrossProduct")->setTemporary(false);
   Database db = DatabaseLoadUtil::genTestDB();
   factDB::QueryGraphBuilder qgb(db, 4);
   [[maybe_unused]] auto [leftIU1, rightIU1] = qgb.addJoin(0, 1);
   [[maybe_unused]] auto [leftIU3, rightIU3] = qgb.addJoin(2, 3);
   auto qgb1 = qgb, qgb2 = qgb; // todo fixme copy-construction of std::vec<std::unique_ptr<>>
   qgb1.addJoin(1, 2); //, rightIU1, leftIU3);
   qgb2.addJoin(3, 0); //, rightIU3, leftIU1);
   QueryGraph qg1 = qgb1.getQueryGraph();
   QueryGraph qg2 = qgb2.getQueryGraph();
   opt::DummyEstimator estimator1(qg1, db), estimator2(qg2, db);

   const auto& relations = qg1.getRelations();
   std::vector<BaseTablePlan> baseTables;
   for (size_t i = 0; i != relations.size(); ++i)
      baseTables.emplace_back(i);

   JoinPlan p01 = Plan::makeJoin(&baseTables[0], &baseTables[1], estimator1, algebra::JoinMode::TopInsert);
   JoinPlan p12 = Plan::makeJoin(&baseTables[1], &baseTables[2], estimator1, algebra::JoinMode::TopInsert);
   JoinPlan p23 = Plan::makeJoin(&baseTables[2], &baseTables[3], estimator1, algebra::JoinMode::TopInsert);
   JoinPlan p30 = Plan::makeJoin(&baseTables[3], &baseTables[0], estimator1, algebra::JoinMode::TopInsert);

   JoinPlan p012a = Plan::makeJoin(&baseTables[0], &p12, estimator1, algebra::JoinMode::TopInsert), p012b = Plan::makeJoin(&p01, &baseTables[2], estimator1, algebra::JoinMode::TopInsert);
   JoinPlan p123a = Plan::makeJoin(&baseTables[1], &p23, estimator1, algebra::JoinMode::TopInsert), p123b = Plan::makeJoin(&p12, &baseTables[3], estimator1, algebra::JoinMode::TopInsert);
   JoinPlan p230a = Plan::makeJoin(&baseTables[2], &p30, estimator1, algebra::JoinMode::TopInsert), p230b = Plan::makeJoin(&p23, &baseTables[0], estimator1, algebra::JoinMode::TopInsert);
   JoinPlan p301a = Plan::makeJoin(&baseTables[3], &p01, estimator1, algebra::JoinMode::TopInsert), p301b = Plan::makeJoin(&p30, &baseTables[1], estimator1, algebra::JoinMode::TopInsert);

   std::vector<JoinPlan> fullPlans;
   fullPlans.push_back(Plan::makeJoin(&p012a, &baseTables[3], estimator1, algebra::JoinMode::TopInsert));
   fullPlans.push_back(Plan::makeJoin(&p012b, &baseTables[3], estimator1, algebra::JoinMode::TopInsert));
   fullPlans.push_back(Plan::makeJoin(&p123a, &baseTables[0], estimator1, algebra::JoinMode::TopInsert));
   fullPlans.push_back(Plan::makeJoin(&p123b, &baseTables[0], estimator1, algebra::JoinMode::TopInsert));
   fullPlans.push_back(Plan::makeJoin(&p230a, &baseTables[1], estimator1, algebra::JoinMode::TopInsert));
   fullPlans.push_back(Plan::makeJoin(&p230b, &baseTables[1], estimator1, algebra::JoinMode::TopInsert));
   fullPlans.push_back(Plan::makeJoin(&p301a, &baseTables[2], estimator1, algebra::JoinMode::TopInsert));
   fullPlans.push_back(Plan::makeJoin(&p301b, &baseTables[2], estimator1, algebra::JoinMode::TopInsert));

   fullPlans.push_back(Plan::makeJoin(&baseTables[3], &p012a, estimator1, algebra::JoinMode::TopInsert));
   fullPlans.push_back(Plan::makeJoin(&baseTables[3], &p012b, estimator1, algebra::JoinMode::TopInsert));
   fullPlans.push_back(Plan::makeJoin(&baseTables[0], &p123a, estimator1, algebra::JoinMode::TopInsert));
   fullPlans.push_back(Plan::makeJoin(&baseTables[0], &p123b, estimator1, algebra::JoinMode::TopInsert));
   fullPlans.push_back(Plan::makeJoin(&baseTables[1], &p230a, estimator1, algebra::JoinMode::TopInsert));
   fullPlans.push_back(Plan::makeJoin(&baseTables[1], &p230b, estimator1, algebra::JoinMode::TopInsert));
   fullPlans.push_back(Plan::makeJoin(&baseTables[2], &p301a, estimator1, algebra::JoinMode::TopInsert));
   fullPlans.push_back(Plan::makeJoin(&baseTables[2], &p301b, estimator1, algebra::JoinMode::TopInsert));

   auto firstTreeQG1 = fullPlans.front().generateCountPlan(qg1, estimator1);
   auto firstTreeQG2 = fullPlans.front().generateCountPlan(qg2, estimator2);
   CardinalityOracle cardinalityOracle(db);
   auto firstResQG1 = cardinalityOracle.generateSignature(firstTreeQG1);
   auto firstResQG2 = cardinalityOracle.generateSignature(firstTreeQG2);

   for (auto& p : fullPlans) {
      {
         auto tree = p.generateCountPlan(qg1, estimator1);
         auto res = cardinalityOracle.generateSignature(tree);
         // ASSERT_EQ(firstResQG1, res); // maybe todo
         ASSERT_NE(firstResQG2, res);
      }

      {
         auto tree = p.generateCountPlan(qg2, estimator2);
         auto res = cardinalityOracle.generateSignature(tree);
         ASSERT_NE(firstResQG1, res);
         // ASSERT_EQ(firstResQG2, res); // maybe todo
      }
   }
   {
      auto tbl0 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r0");
      auto tbl1 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r1");
      auto tbl2 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r2");
      auto tbl3 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r3");
      auto jcl01 = JoinConditionList::create(tbl0->collectIUs()[0], tbl1->collectIUs()[0]);
      auto jcl12 = JoinConditionList::create(tbl1->collectIUs()[1], tbl2->collectIUs()[1]);
      auto jcl23 = JoinConditionList::create(tbl2->collectIUs()[0], tbl3->collectIUs()[0]);

      auto j01 = std::make_unique<algebra::InnerJoin>(std::move(tbl0), std::move(tbl1), std::move(jcl01));
      auto j012 = std::make_unique<algebra::InnerJoin>(std::move(j01), std::move(tbl2), std::move(jcl12));
      std::unique_ptr<algebra::Operator> j0123 = std::make_unique<algebra::InnerJoin>(std::move(j012), std::move(tbl3), std::move(jcl23));
      std::unique_ptr<algebra::Operator> count = std::make_unique<algebra::Count>(std::move(j0123));

      ASSERT_NE(firstResQG1, cardinalityOracle.generateSignature(count));
      ASSERT_NE(firstResQG2, cardinalityOracle.generateSignature(count));
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_opt_CardinalityOracle, SignatureTransitive) {
   Database db = DatabaseLoadUtil::genTestDB();
   factDB::QueryGraphBuilder qgb(db, 4);
   [[maybe_unused]] auto [leftIU1, rightIU1] = qgb.addJoin(0, 1);
   [[maybe_unused]] auto [leftIU3, rightIU3] = qgb.addJoin(2, 3);
   auto qgb1 = qgb, qgb2 = qgb;
   qgb1.addJoin(1, 2, rightIU1, leftIU3);
   qgb2.addJoin(3, 0, rightIU3, leftIU1);
   QueryGraph qg1 = qgb1.getQueryGraph();
   QueryGraph qg2 = qgb2.getQueryGraph();
   opt::DummyEstimator estimator1(qg1, db), estimator2(qg2, db);

   std::vector<BaseTablePlan> baseTables;
   const auto& relations = qg1.getRelations();
   for (size_t i = 0; i != relations.size(); ++i)
      baseTables.emplace_back(i);

   JoinPlan p01 = Plan::makeJoin(&baseTables[0], &baseTables[1], estimator1, algebra::JoinMode::TopInsert);
   JoinPlan p012 = Plan::makeJoin(&p01, &baseTables[2], estimator1, algebra::JoinMode::TopInsert);
   JoinPlan p0123 = Plan::makeJoin(&p012, &baseTables[3], estimator1, algebra::JoinMode::TopInsert);

   auto g1 = p0123.generateCountPlan(qg1, estimator1);
   auto g2 = p0123.generateCountPlan(qg2, estimator2);

   algebra::visitors::GraphvizQueryPlan::plotSubPlan(db, *g1);
   std::cout << "------------------------" << std::endl;
   algebra::visitors::GraphvizQueryPlan::plotSubPlan(db, *g2);

   CardinalityOracle co(db);
   ASSERT_EQ(co.generateSignature(g1), co.generateSignature(g2));
}
// ---------------------------------------------------------------------------------------------------
