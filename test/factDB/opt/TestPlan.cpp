// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Reference.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/opt/CardinalityOracle.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/estimators/DummyEstimator.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "gtest/gtest.h"
#include "test/factDB/opt/QueryGraphBuilder.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
using namespace std;
// ---------------------------------------------------------------------------------------------------
template <typename T>
const T* castIgnoreReference(const algebra::Operator* frontOp) {
   while (frontOp->getType() == algebra::Operator::Reference)
      frontOp = &static_cast<const algebra::Reference*>(frontOp)->getChild();
   return T::dynCast(frontOp);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_opt_Plan, SimpleTwoHop) {
   Database db = DatabaseLoadUtil::genUnloadedDB();
   { // left deep
      QueryGraphBuilder qgb(db, 3);
      qgb.addJoin(0, 1);
      qgb.addJoin(1, 2);
      opt::QueryGraph queryGraph = qgb.getQueryGraph();
      opt::DummyEstimator estimator(queryGraph, db);
      ASSERT_TRUE(queryGraph.isGraphConnected());

      opt::BaseTablePlan r0(0), r1(1), r2(2);

      auto j1 = opt::Plan::makeJoin(&r0, &r1, estimator, algebra::JoinMode::TopInsert);
      auto j2 = opt::Plan::makeJoin(&j1, &r2, estimator, algebra::JoinMode::TopInsert);

      auto algebraPlan = j2.generateOperatorPlan(queryGraph, estimator);

      auto algebraJoin1 = castIgnoreReference<algebra::InnerJoin>(algebraPlan.get());
      ASSERT_NE(algebraJoin1, nullptr);
      auto algebraJoin2 = castIgnoreReference<algebra::InnerJoin>(algebraJoin1->getLeftChild().get());
      ASSERT_NE(algebraJoin1, nullptr);
      auto tableScan1 = castIgnoreReference<algebra::TableScan>(algebraJoin2->getLeftChild().get());
      auto tableScan2 = castIgnoreReference<algebra::TableScan>(algebraJoin2->getRightChild().get());
      auto tableScan3 = castIgnoreReference<algebra::TableScan>(algebraJoin1->getRightChild().get());
      ASSERT_NE(tableScan1, nullptr);
      ASSERT_NE(tableScan2, nullptr);
      ASSERT_NE(tableScan3, nullptr);
   }
   { // right deep
      QueryGraphBuilder qgb(db, 3);
      qgb.addJoin(0, 1);
      qgb.addJoin(1, 2);
      opt::QueryGraph queryGraph = qgb.getQueryGraph();
      opt::DummyEstimator estimator(queryGraph, db);
      ASSERT_TRUE(queryGraph.isGraphConnected());

      opt::BaseTablePlan r0(0), r1(1), r2(2);

      auto j1 = opt::Plan::makeJoin(&r1, &r2, estimator, algebra::JoinMode::TopInsert);
      auto j2 = opt::Plan::makeJoin(&r0, &j1, estimator, algebra::JoinMode::TopInsert);
      auto algebraPlan = j2.generateOperatorPlan(queryGraph, estimator);

      auto algebraJoin1 = castIgnoreReference<algebra::InnerJoin>(algebraPlan.get());
      ASSERT_NE(algebraJoin1, nullptr);
      auto algebraJoin2 = castIgnoreReference<algebra::InnerJoin>(algebraJoin1->getRightChild().get());
      ASSERT_NE(algebraJoin1, nullptr);
      auto tableScan1 = castIgnoreReference<algebra::TableScan>(algebraJoin1->getLeftChild().get());
      auto tableScan2 = castIgnoreReference<algebra::TableScan>(algebraJoin2->getLeftChild().get());
      auto tableScan3 = castIgnoreReference<algebra::TableScan>(algebraJoin2->getRightChild().get());
      ASSERT_NE(tableScan1, nullptr);
      ASSERT_NE(tableScan2, nullptr);
      ASSERT_NE(tableScan3, nullptr);
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_opt_Plan, Circle4) {
   Database db = DatabaseLoadUtil::genTestDB();
   QueryGraphBuilder qgb(db, 4);
   qgb.addJoin(0, 1);
   qgb.addJoin(1, 2);
   qgb.addJoin(2, 3);
   qgb.addJoin(3, 0);
   opt::QueryGraph queryGraph = qgb.getQueryGraph();
   opt::DummyEstimator estimator(queryGraph, db);
   ASSERT_TRUE(queryGraph.isGraphConnected());

   opt::BaseTablePlan r0(0), r1(1), r2(2), r3(3);

   auto j1 = opt::Plan::makeJoin(&r0, &r1, estimator, algebra::JoinMode::TopInsert);
   auto j2 = opt::Plan::makeJoin(&j1, &r2, estimator, algebra::JoinMode::TopInsert);
   auto j3 = opt::Plan::makeJoin(&j2, &r3, estimator, algebra::JoinMode::TopInsert);

   auto algebraPlan = j3.generateOperatorPlan(queryGraph, estimator);

   auto algebraJoin1 = castIgnoreReference<algebra::InnerJoin>(algebraPlan.get());
   ASSERT_NE(algebraJoin1, nullptr);
   ASSERT_EQ(algebraJoin1->getJoinCondition().size(), 2);
   auto algebraJoin2 = castIgnoreReference<algebra::InnerJoin>(algebraJoin1->getLeftChild().get());
   ASSERT_NE(algebraJoin2, nullptr);
   ASSERT_EQ(algebraJoin2->getJoinCondition().size(), 1);
   auto algebraJoin3 = castIgnoreReference<algebra::InnerJoin>(algebraJoin2->getLeftChild().get());
   ASSERT_NE(algebraJoin3, nullptr);
   ASSERT_EQ(algebraJoin3->getJoinCondition().size(), 1);
   auto tableScan1 = castIgnoreReference<algebra::TableScan>(algebraJoin3->getLeftChild().get());
   auto tableScan2 = castIgnoreReference<algebra::TableScan>(algebraJoin3->getRightChild().get());
   auto tableScan3 = castIgnoreReference<algebra::TableScan>(algebraJoin2->getRightChild().get());
   auto tableScan4 = castIgnoreReference<algebra::TableScan>(algebraJoin1->getRightChild().get());
   ASSERT_NE(tableScan1, nullptr);
   ASSERT_NE(tableScan2, nullptr);
   ASSERT_NE(tableScan3, nullptr);
   ASSERT_NE(tableScan4, nullptr);
}
// ---------------------------------------------------------------------------------------------------
