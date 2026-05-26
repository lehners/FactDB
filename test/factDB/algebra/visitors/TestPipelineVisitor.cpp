#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/algebra/visitors/PipelineStorage.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "gtest/gtest.h"
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
using namespace factDB::algebra::visitors;
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_PipelineStorage, IdsSinglePipeline) {
   Database db = DatabaseLoadUtil::genTestDB();
   auto scan = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r1");
   auto printer = std::make_unique<algebra::Print>(std::move(scan), "|");

   PipelineStore<size_t> map;
   map[printer] = 15;

   ASSERT_EQ(map[*printer], 15);
   ASSERT_EQ(printer->getChild()->getType(), algebra::Operator::TableScan);
   ASSERT_EQ(map[*printer->getChild()], 15);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_PipelineStorage, IdsSingleJoin) {
   Database db = DatabaseLoadUtil::genTestDB();
   auto r1 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r1");
   auto r2 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r2");
   auto predicates = JoinConditionList::create(r1->collectIUs()[0], std::move(r2->collectIUs()[0]));
   auto join = std::make_unique<algebra::InnerJoin>(std::move(r1), std::move(r2), std::move(predicates));
   auto printer = std::make_unique<algebra::Print>(std::move(join), "|");

   PipelineStore<size_t> map;
   auto& joinRef = static_cast<algebra::InnerJoin&>(*printer->getChild());
   auto& r1Ref = static_cast<algebra::TableScan&>(*joinRef.getLeftChild());
   auto& r2Ref = static_cast<algebra::TableScan&>(*joinRef.getRightChild());

   map[printer] = 404;
   map[joinRef] = 12;
   map[r1Ref] = 1;
   map[r2Ref] = 2;

   ASSERT_EQ(map[printer], 12);
   ASSERT_EQ(map[joinRef], 12);
   ASSERT_EQ(map[r1Ref], 1);
   ASSERT_EQ(map[r2Ref], 2);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_PipelineStorage, IdsMultipleJoins) {
   PipelineStore<size_t> map;
   auto db = DatabaseLoadUtil::genTestDB();
   auto r1 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r1");
   auto r2 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r2");
   auto r3 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r3");

   auto predicates12 = JoinConditionList::create(r1->collectIUs()[1], r2->collectIUs()[0]);
   auto predicates23 = JoinConditionList::create(r2->collectIUs()[1], r3->collectIUs()[0]);

   auto join12 = std::make_unique<algebra::InnerJoin>(std::move(r1), std::move(r2), std::move(predicates12));
   auto join123 = std::make_unique<algebra::InnerJoin>(std::move(join12), std::move(r3), std::move(predicates23));
   auto printer = std::make_unique<algebra::Print>(std::move(join123), "|");

   auto& join123Ref = static_cast<algebra::InnerJoin&>(*printer->getChild());
   auto& join12Ref = static_cast<algebra::InnerJoin&>(*join123Ref.getLeftChild());
   auto& r1Ref = static_cast<algebra::TableScan&>(*join12Ref.getLeftChild());
   auto& r2Ref = static_cast<algebra::TableScan&>(*join12Ref.getRightChild());
   auto& r3Ref = static_cast<algebra::TableScan&>(*join123Ref.getRightChild());

   map[printer] = 404;
   map[join123Ref] = 123;
   map[join12Ref] = 12;
   map[r1Ref] = 1;
   map[r2Ref] = 2;
   map[r3Ref] = 3;

   ASSERT_EQ(map[printer], 123);
   ASSERT_EQ(map[join123Ref], 123);
   ASSERT_EQ(map[join12Ref], 12);
   ASSERT_EQ(map[r1Ref], 1);
   ASSERT_EQ(map[r2Ref], 2);
   ASSERT_EQ(map[r3Ref], 3);
}
// ---------------------------------------------------------------------------------------------------