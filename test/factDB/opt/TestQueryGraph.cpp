// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/estimators/DummyEstimator.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "gtest/gtest.h"
#include "test/factDB/opt/QueryGraphBuilder.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
using namespace std;
// ---------------------------------------------------------------------------------------------------
TEST(factDB_opt_QueryGraph, EquivalenceClasses) {
   auto db = DatabaseLoadUtil::genTestDB();
   auto r1 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r1");
   auto r2 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r2");
   auto r3 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r3");
   auto r4 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r4");
   auto r5 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r5");

   auto r1IUs = r1->collectIUs();
   auto r2IUs = r2->collectIUs();
   auto r3IUs = r3->collectIUs();
   auto r4IUs = r4->collectIUs();
   auto r5IUs = r5->collectIUs();

   auto jc1 = JoinCondition::create(r1IUs[0], r2IUs[0]);
   auto jc2 = JoinCondition::create(r1IUs[1], r4IUs[0]);
   auto jc3 = JoinCondition::create(r1IUs[1], r2IUs[0]);
   auto jc4 = JoinCondition::create(r2IUs[1], r3IUs[0]);
   auto jc5 = JoinCondition::create(r3IUs[1], r4IUs[1]);
   auto jc6 = JoinCondition::create(r4IUs[1], r5IUs[0]);

   auto jcl12 = JoinConditionList::create({std::move(jc1), std::move(jc3)});
   auto jcl123 = JoinConditionList::create({std::move(jc4)});
   auto jcl1234 = JoinConditionList::create({std::move(jc2), std::move(jc5)});
   auto jcl12345 = JoinConditionList::create({std::move(jc6)});

   auto j1 = std::make_unique<algebra::InnerJoin>(std::move(r1), std::move(r2), std::move(jcl12));
   auto j2 = std::make_unique<algebra::InnerJoin>(std::move(j1), std::move(r3), std::move(jcl123));
   auto j3 = std::make_unique<algebra::InnerJoin>(std::move(j2), std::move(r4), std::move(jcl1234));
   auto j4 = std::make_unique<algebra::InnerJoin>(std::move(j3), std::move(r5), std::move(jcl12345));

   auto allIUs = j4->collectIUs();
   std::unique_ptr<algebra::Operator> tree = std::make_unique<algebra::Count>(std::move(j4));

   std::vector<std::vector<const IU*>> should{
      {r1IUs[0], r1IUs[1], r4IUs[0], r2IUs[0]},
      {r2IUs[1], r3IUs[0]},
      {r3IUs[1], r4IUs[1], r5IUs[0]},
      {r5IUs[1]}};

   auto queryGraph = opt::QueryGraph::generateQueryGraph(db, *tree);

   std::unordered_map<const IU*, unsigned> iu2class;
   std::vector<unsigned> elementsPerClass(queryGraph.maxEquivalenceClasses(), 0);
   for (const IU* iu : tree->collectIUs()) {
      unsigned equivalenceClass = queryGraph.getEquivalenceClass(*iu);
      ASSERT_LT(equivalenceClass, elementsPerClass.size());
      iu2class[iu] = equivalenceClass;
      ++elementsPerClass[equivalenceClass];
   }

   ASSERT_EQ(iu2class.size(), 10);
   for (auto& equivalenceClass : should) {
      ASSERT_FALSE(equivalenceClass.empty());
      unsigned firstElementsClass = iu2class[equivalenceClass.front()];
      ASSERT_EQ(equivalenceClass.size(), elementsPerClass[firstElementsClass]);

      for (size_t innerIdx = 1; innerIdx < equivalenceClass.size(); ++innerIdx) {
         ASSERT_TRUE(iu2class.contains(equivalenceClass[innerIdx]));
         ASSERT_EQ(firstElementsClass, iu2class[equivalenceClass[innerIdx]]);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_opt_QueryGraph, isGraphConnected) {
   Database db = DatabaseLoadUtil::genTestDB();
   {
      QueryGraphBuilder qgb(db, 4);
      qgb.addJoin(0, 1);
      qgb.addJoin(1, 2);
      opt::QueryGraph queryGraph = qgb.getQueryGraph();
      // not connected, since 4. relation is not joined
      ASSERT_FALSE(queryGraph.isGraphConnected());
   }
   {
      QueryGraphBuilder qgb(db, 4);
      qgb.addJoin(2, 3);
      qgb.addJoin(0, 1);
      qgb.addJoin(1, 2);
      qgb.addJoin(3, 0);
      opt::QueryGraph queryGraph = qgb.getQueryGraph();
      ASSERT_TRUE(queryGraph.isGraphConnected());
   }
   {
      QueryGraphBuilder qgb(db, 4);
      qgb.addJoin(2, 3);
      qgb.addJoin(0, 1);
      opt::QueryGraph queryGraph = qgb.getQueryGraph();
      ASSERT_FALSE(queryGraph.isGraphConnected());
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_opt_QueryGraph, getConnectingJoins) {
   Database db = DatabaseLoadUtil::genTestDB();
   std::unique_ptr<algebra::Operator> fakeLeftOp, fakeRightOp;

   auto contains = [](const auto& vec, const JoinCondition& join) {
      for (const auto& elem : vec) {
         auto elemCondition = elem.condition;
         if (elemCondition.get_left() == join.get_left() && elemCondition.get_right() == join.get_right())
            return true;
         if (elemCondition.get_left() == join.get_right() && elemCondition.get_right() == join.get_left()) // switched sides
            return true;
      }
      return false;
   };

   auto containesAll = [&](const auto& vec, const std::vector<opt::OptimizerJoin>& joins, factDB::opt::QueryGraph& qg) {
      for (auto j : joins) {
         auto& jcl = *JoinConditionList::dynCast(qg.getJoinInfo(j).joinCondition);
         for (const JoinCondition& jc : jcl.get_conditions())
            if (!contains(vec, jc))
               return false;
      }
      return true;
   };

   auto containesExactlyOne = [&contains](const auto& vec, const std::vector<JoinCondition>& joins) {
      bool matchFound = false;
      for (auto& join : joins) {
         if (!matchFound && contains(vec, join))
            matchFound = true;
         else if (matchFound && contains(vec, join))
            return false;
      }
      return matchFound;
   };

   auto getPossibleJoins = [](std::vector<const IU*> iu, const std::vector<uint32_t>& leftRelations, const std::vector<uint32_t>& rightRelations) { // NOLINT(bugprone-easily-swappable-parameters)
      std::vector<JoinCondition> ret;
      for (auto leftRel : leftRelations) {
         for (auto rightRel : rightRelations) {
            ret.emplace_back(JoinCondition::create(iu[leftRel], iu[rightRel]));
         }
      }
      return ret;
   };

   auto getFirstCondition = [](opt::QueryGraph& queryGraph, const factDB::opt::OptimizerJoin& j) {
      return JoinConditionList::dynCast(queryGraph.getJoinInfo(j).joinCondition)->get_conditions().front();
   };

   { // chain, different ius
      QueryGraphBuilder qgb(db, 4);
      qgb.addJoin(0, 1);
      qgb.addJoin(1, 2);
      qgb.addJoin(2, 3);
      opt::QueryGraph queryGraph = qgb.getQueryGraph();
      opt::DummyEstimator estimator(queryGraph, db);
      ASSERT_TRUE(queryGraph.isGraphConnected());

      auto conn1 = queryGraph.getConnectingJoins(infra::BitSet64({0, 1}), infra::BitSet64({2, 3}), fakeLeftOp, fakeRightOp, estimator);
      ASSERT_EQ(conn1.size(), 1);
      ASSERT_EQ(conn1.front().condition, getFirstCondition(queryGraph, qgb.joins[1]));

      auto conn2 = queryGraph.getConnectingJoins(infra::BitSet64({1}), infra::BitSet64({2}), fakeLeftOp, fakeRightOp, estimator);
      ASSERT_EQ(conn2.size(), 1);
      ASSERT_EQ(conn2.front().condition, getFirstCondition(queryGraph, qgb.joins[1]));

      // no connection
      auto conn3 = queryGraph.getConnectingJoins(infra::BitSet64({0}), infra::BitSet64({3}), fakeLeftOp, fakeRightOp, estimator);
      ASSERT_TRUE(conn3.empty());
   }

   { // chain, same ius
      QueryGraphBuilder qgb(db, 4);
      [[maybe_unused]] auto [leftIU1, rightIU1] = qgb.addJoin(0, 1);
      [[maybe_unused]] auto [leftIU2, rightIU2] = qgb.addJoin(1, 2, rightIU1);
      [[maybe_unused]] auto [leftIU3, rightIU3] = qgb.addJoin(2, 3, rightIU2);

      opt::QueryGraph queryGraph = qgb.getQueryGraph();
      opt::DummyEstimator estimator(queryGraph, db);
      ASSERT_TRUE(queryGraph.isGraphConnected());

      auto conn1 = queryGraph.getConnectingJoins(infra::BitSet64({0, 1}), infra::BitSet64({2, 3}), fakeLeftOp, fakeRightOp, estimator);
      ASSERT_EQ(conn1.size(), 1);
      ASSERT_TRUE(containesExactlyOne(conn1, getPossibleJoins({leftIU1, rightIU1, rightIU2, rightIU3}, {0, 1}, {2, 3})));

      auto conn2 = queryGraph.getConnectingJoins(infra::BitSet64({1}), infra::BitSet64({2}), fakeLeftOp, fakeRightOp, estimator);
      ASSERT_EQ(conn2.size(), 1);
      ASSERT_TRUE(containesExactlyOne(conn2, getPossibleJoins({leftIU1, rightIU1, rightIU2, rightIU3}, {1}, {2})));

      // no connection
      auto conn3 = queryGraph.getConnectingJoins(infra::BitSet64({0}), infra::BitSet64({3}), fakeLeftOp, fakeRightOp, estimator);
      ASSERT_EQ(conn3.size(), 1);
      ASSERT_TRUE(containesExactlyOne(conn3, getPossibleJoins({leftIU1, rightIU1, rightIU2, rightIU3}, {0}, {3})));
   }

   { // cycle disjunct IUs
      QueryGraphBuilder qgb(db, 4);
      qgb.addJoin(0, 1);
      qgb.addJoin(1, 2);
      qgb.addJoin(2, 3);
      qgb.addJoin(3, 0);
      opt::QueryGraph queryGraph = qgb.getQueryGraph();
      opt::DummyEstimator estimator(queryGraph, db);
      ASSERT_TRUE(queryGraph.isGraphConnected());

      for (auto& j : qgb.joins) {
         auto conn1 = queryGraph.getConnectingJoins(infra::BitSet64({j.leftRelation}), infra::BitSet64({j.rightRelation}), fakeLeftOp, fakeRightOp, estimator);
         ASSERT_EQ(conn1.size(), 1);
         ASSERT_EQ(conn1.front().condition, getFirstCondition(queryGraph, j));

         auto conn2 = queryGraph.getConnectingJoins(infra::BitSet64({j.leftRelation}), infra::BitSet64({j.rightRelation}), fakeLeftOp, fakeRightOp, estimator);
         ASSERT_EQ(conn2.size(), 1);
         ASSERT_EQ(conn2.front().condition, getFirstCondition(queryGraph, j));
      }

      auto conn1 = queryGraph.getConnectingJoins(infra::BitSet64({0, 1}), infra::BitSet64({2, 3}), fakeLeftOp, fakeRightOp, estimator);
      auto conn2 = queryGraph.getConnectingJoins(infra::BitSet64({1, 2}), infra::BitSet64({3, 0}), fakeLeftOp, fakeRightOp, estimator);
      auto conn3 = queryGraph.getConnectingJoins(infra::BitSet64({2, 3}), infra::BitSet64({0, 1}), fakeLeftOp, fakeRightOp, estimator);
      auto conn4 = queryGraph.getConnectingJoins(infra::BitSet64({3, 0}), infra::BitSet64({1, 2}), fakeLeftOp, fakeRightOp, estimator);

      ASSERT_EQ(conn1.size(), 2);
      ASSERT_EQ(conn2.size(), 2);
      ASSERT_EQ(conn3.size(), 2);
      ASSERT_EQ(conn4.size(), 2);

      ASSERT_TRUE(containesAll(conn1, {qgb.joins[1], qgb.joins[3]}, queryGraph));
      ASSERT_TRUE(containesAll(conn2, {qgb.joins[0], qgb.joins[2]}, queryGraph));
      ASSERT_TRUE(containesAll(conn3, {qgb.joins[1], qgb.joins[3]}, queryGraph));
      ASSERT_TRUE(containesAll(conn4, {qgb.joins[0], qgb.joins[2]}, queryGraph));
   }

   { // cycle same IUs
      QueryGraphBuilder qgb(db, 4);
      [[maybe_unused]] auto [leftIU1, rightIU1] = qgb.addJoin(0, 1);
      [[maybe_unused]] auto [leftIU2, rightIU2] = qgb.addJoin(1, 2, rightIU1);
      [[maybe_unused]] auto [leftIU3, rightIU3] = qgb.addJoin(2, 3, rightIU2);
      [[maybe_unused]] auto [leftIU4, rightIU4] = qgb.addJoin(3, 0, rightIU3, leftIU1);
      opt::QueryGraph queryGraph = qgb.getQueryGraph();
      opt::DummyEstimator estimator(queryGraph, db);
      ASSERT_TRUE(queryGraph.isGraphConnected());

      for (auto& j : qgb.joins) { // for two relations there is only a single choice
         auto conn1 = queryGraph.getConnectingJoins(infra::BitSet64{j.leftRelation}, infra::BitSet64{j.rightRelation}, fakeLeftOp, fakeRightOp, estimator);
         ASSERT_EQ(conn1.size(), 1);
         ASSERT_EQ(conn1.front().condition, getFirstCondition(queryGraph, j));

         auto conn2 = queryGraph.getConnectingJoins(infra::BitSet64{j.leftRelation}, infra::BitSet64{j.rightRelation}, fakeLeftOp, fakeRightOp, estimator);
         ASSERT_EQ(conn2.size(), 1);
         ASSERT_EQ(conn2.front().condition, getFirstCondition(queryGraph, j));
      }

      {
         std::initializer_list<unsigned> leftSet = {0, 1}, rightSet = {2, 3};
         auto conn = queryGraph.getConnectingJoins(infra::BitSet64(leftSet), infra::BitSet64{rightSet}, fakeLeftOp, fakeRightOp, estimator);
         ASSERT_EQ(conn.size(), 1);
         ASSERT_TRUE(containesExactlyOne(conn, getPossibleJoins({leftIU1, rightIU1, rightIU2, rightIU3}, leftSet, rightSet)));
         ASSERT_FALSE(containesExactlyOne(conn, getPossibleJoins({leftIU1, rightIU1, rightIU2, rightIU3}, {0}, {1})));
         ASSERT_FALSE(containesExactlyOne(conn, getPossibleJoins({leftIU1, rightIU1, rightIU2, rightIU3}, {2}, {3})));
      }
      {
         std::initializer_list<unsigned> leftSet = {1, 2}, rightSet = {3, 0};
         auto conn = queryGraph.getConnectingJoins(infra::BitSet64(leftSet), infra::BitSet64{rightSet}, fakeLeftOp, fakeRightOp, estimator);
         ASSERT_EQ(conn.size(), 1);
         ASSERT_TRUE(containesExactlyOne(conn, getPossibleJoins({leftIU1, rightIU1, rightIU2, rightIU3}, leftSet, rightSet)));
      }
      {
         std::initializer_list<unsigned> leftSet = {2, 3}, rightSet = {0, 1};
         auto conn = queryGraph.getConnectingJoins(infra::BitSet64(leftSet), infra::BitSet64{rightSet}, fakeLeftOp, fakeRightOp, estimator);
         ASSERT_EQ(conn.size(), 1);
         ASSERT_TRUE(containesExactlyOne(conn, getPossibleJoins({leftIU1, rightIU1, rightIU2, rightIU3}, leftSet, rightSet)));
      }
      {
         std::initializer_list<unsigned> leftSet = {3, 0}, rightSet = {1, 2};
         auto conn = queryGraph.getConnectingJoins(infra::BitSet64(leftSet), infra::BitSet64{rightSet}, fakeLeftOp, fakeRightOp, estimator);
         ASSERT_EQ(conn.size(), 1);
         ASSERT_TRUE(containesExactlyOne(conn, getPossibleJoins({leftIU1, rightIU1, rightIU2, rightIU3}, leftSet, rightSet)));
      }
   }
}
// ---------------------------------------------------------------------------------------------------
