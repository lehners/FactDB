#include "test/factDB/util/TranslationTest.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/util/LineSortingStream.hpp"
#include "factDB/queryc/QueryParseContext.hpp"
#include "factDB/util/JsonHandler.hpp"
#include <numeric>
// ---------------------------------------------------------------------------------------------------
namespace factDB::test {
// ---------------------------------------------------------------------------------------------------
std::string testGeneratedCode(factDB::Database& db, std::unique_ptr<factDB::algebra::Operator> printer, const factDB::OrderedIUSet& required, bool sortLines) {
   auto pregen = SettingBase::getSetting<bool>("codegen.pregenerateQueries")->setTemporaryBool(true);

   LineSortingStream lss;
   factDB::queryc::QueryParseContext context(db);
   context.compileAndExecute(std::move(printer), required, db, lss);
   if (sortLines)
      lss.sortLines();
   return lss.concatLines();
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<factDB::algebra::Operator> serializeAndDeserialize(const Database& db, std::unique_ptr<factDB::algebra::Operator>& printer, const factDB::OrderedIUSet& required) {
   factDB::util::json::printJson(db, printer, required, "out.json");
   auto deserialized = factDB::util::json::readJson(db, "out.json");
   return deserialized;
}
// ---------------------------------------------------------------------------------------------------
namespace defaultQueryTrees {
// ---------------------------------------------------------------------------------------------------
using namespace factDB::algebra;
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TestCase::getTreePrint(const Database& db) const {
   auto [innerTree, columns] = getInnerTree(db);
   assert(innerTree->getType() != Operator::Print && innerTree->getType() != Operator::Count);
   std::unique_ptr<Operator> printer = std::make_unique<Print>(std::move(innerTree), "|");
   return {std::move(printer), columns};
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TestCase::getTreeCount(const Database& db) const {
   auto [innerTree, columns] = getInnerTree(db);
   assert(innerTree->getType() != Operator::Print && innerTree->getType() != Operator::Count);
   std::unique_ptr<Operator> printer = std::make_unique<Count>(std::move(innerTree));
   return {std::move(printer), columns};
}
// ---------------------------------------------------------------------------------------------------
std::string TestCase::resultCount() const {
   auto s = resultPrint();
   auto newline_count = std::accumulate(s.cbegin(), s.cend(), 0,
                                        [](int prev, char c) { return c == '\n' ? (prev + 1) : prev; });
   return genTupleString<int>({{newline_count}});
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TestSimpleScan::getInnerTree(const Database& db) const {
   auto scan = std::make_unique<TableScan>(db, testdb::KRelationTest, "r1");
   return {std::move(scan), scan->collectIUs()};
}
// ---------------------------------------------------------------------------------------------------
std::string TestSimpleScan::resultPrint() const {
   return genTupleString<int, int>({{1, 2}, {1, 2}, {2, 3}, {2, 4}, {3, 5}});
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TestReorderColumns::getInnerTree(const Database& db) const {
   auto scan = std::make_unique<TableScan>(db, testdb::KRelationTest, "r1");
   auto columns = scan->collectIUs();
   auto outputColumns = {columns[1], columns[1], columns[0]};
   return {std::move(scan), outputColumns};
}
// ---------------------------------------------------------------------------------------------------
std::string TestReorderColumns::resultPrint() const {
   return genTupleString<int, int, int>({{2, 2, 1}, {2, 2, 1}, {3, 3, 2}, {4, 4, 2}, {5, 5, 3}});
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TestSimpleFilter::getInnerTree(const Database& db) const {
   auto scan = std::make_unique<TableScan>(db, testdb::KRelationTest, "r1");
   auto columns = scan->collectIUs();
   auto filter = std::make_unique<Selection>(std::move(scan), CompareEqual::create(columns[0], 1));
   return {std::move(filter), columns};
}
// ---------------------------------------------------------------------------------------------------
std::string TestSimpleFilter::resultPrint() const {
   return genTupleString<int, int>({{1, 2}, {1, 2}});
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TestMultipleFilterPredicates::getInnerTree(const Database& db) const {
   auto scan = std::make_unique<TableScan>(db, testdb::KRelationTest, "r1");
   auto columns = scan->collectIUs();

   auto predicates = AndExpression::create(CompareEqual::create(columns[0], 2), CompareEqual::create(columns[1], 3));
   auto filter = std::make_unique<Selection>(std::move(scan), std::move(predicates));
   return {std::move(filter), columns};
}
// ---------------------------------------------------------------------------------------------------
std::string TestMultipleFilterPredicates::resultPrint() const {
   return genTupleString<int, int>({{2, 3}});
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TestSimpleJoin::getInnerTree(const Database& db) const {
   auto r1 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r1");
   auto r2 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r2");

   auto columns = r1->collectIUs().merge(r2->collectIUs());
   auto predicates = JoinConditionList::create(r1->collectIUs()[0], std::move(r2->collectIUs()[0]));
   auto join = std::make_unique<InnerJoin>(std::move(r1), std::move(r2), std::move(predicates), joinMode);
   return {std::move(join), columns};
}
// ---------------------------------------------------------------------------------------------------
std::string TestSimpleJoin::resultPrint() const {
   return genTupleString<int, int, int, int>({{1, 2, 1, 2},
                                              {1, 2, 1, 2},
                                              {1, 2, 1, 2},
                                              {1, 2, 1, 2},
                                              {2, 3, 2, 3},
                                              {2, 3, 2, 4},
                                              {2, 4, 2, 3},
                                              {2, 4, 2, 4},
                                              {3, 5, 3, 5}});
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TestSimpleRightSemiJoin::getInnerTree(const Database& db) const {
   auto r1 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r1");
   auto r2 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r2");

   auto predicates = JoinConditionList::create(r1->collectIUs()[1], std::move(r2->collectIUs()[0]));
   auto join = std::make_unique<InnerJoin>(std::move(r1), std::move(r2), std::move(predicates), joinMode);
   join->setJoinType(algebra::JoinType::RightSemi);
   return {std::move(join), join->collectIUs()};
}
// ---------------------------------------------------------------------------------------------------
std::string TestSimpleRightSemiJoin::resultPrint() const {
   return genTupleString<int, int>({{2, 3},
                                    {2, 4},
                                    {3, 5}});
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TestTwoKeyJoin::getInnerTree(const Database& db) const {
   auto r1 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r1");
   auto r2 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r2");

   auto columns = r1->collectIUs().merge(r2->collectIUs());
   auto predicates = JoinConditionList::create({JoinCondition::create(r1->collectIUs()[0], std::move(r2->collectIUs()[0])),
                                                JoinCondition::create(r1->collectIUs()[1], std::move(r2->collectIUs()[1]))});

   auto join = std::make_unique<InnerJoin>(std::move(r1), std::move(r2), std::move(predicates), joinMode);
   return {std::move(join), columns};
}
// ---------------------------------------------------------------------------------------------------
std::string TestTwoKeyJoin::resultPrint() const {
   return genTupleString<int, int, int, int>({{1, 2, 1, 2},
                                              {1, 2, 1, 2},
                                              {1, 2, 1, 2},
                                              {1, 2, 1, 2},
                                              {2, 3, 2, 3},
                                              {2, 4, 2, 4},
                                              {3, 5, 3, 5}});
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TestTwoHopJoinLeftDeep::getInnerTree(const Database& db) const {
   auto r1 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r1");
   auto r2 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r2");
   auto r3 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r3");

   auto columns = r1->collectIUs().merge(r2->collectIUs()).merge(r3->collectIUs());
   auto predicates12 = JoinConditionList::create(r1->collectIUs()[1], r2->collectIUs()[0]);
   auto predicates23 = JoinConditionList::create(r2->collectIUs()[1], r3->collectIUs()[0]);

   auto join12 = std::make_unique<InnerJoin>(std::move(r1), std::move(r2), std::move(predicates12), joinMode1);
   auto join123 = std::make_unique<InnerJoin>(std::move(join12), std::move(r3), std::move(predicates23), joinMode2);
   return {std::move(join123), columns};
}

std::string TestTwoHopJoinLeftDeep::resultPrint() const {
   return genTupleString<int, int, int, int, int, int>({{1, 2, 2, 3, 3, 5}, {1, 2, 2, 3, 3, 5}});
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TestTwoHopJoinRightDeep::getInnerTree(const Database& db) const {
   auto r1 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r1");
   auto r2 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r2");
   auto r3 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r3");

   auto columns = r1->collectIUs().merge(r2->collectIUs()).merge(r3->collectIUs());
   auto predicates12 = JoinConditionList::create(r1->collectIUs()[1], r2->collectIUs()[0]);
   auto predicates23 = JoinConditionList::create(r2->collectIUs()[1], r3->collectIUs()[0]);

   auto join23 = std::make_unique<InnerJoin>(std::move(r2), std::move(r3), std::move(predicates23), joinMode1);
   auto join123 = std::make_unique<InnerJoin>(std::move(r1), std::move(join23), std::move(predicates12), joinMode2);
   return {std::move(join123), columns};
}
// ---------------------------------------------------------------------------------------------------
std::string TestTwoHopJoinRightDeep::resultPrint() const {
   return genTupleString<int, int, int, int, int, int>({{1, 2, 2, 3, 3, 5}, {1, 2, 2, 3, 3, 5}});
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TestTwoHopSemiJoin::getInnerTree(const Database& db) const {
   auto r1 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r1");
   auto r2 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r2");
   auto r3 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r3");

   auto predicates12 = JoinConditionList::create(r1->collectIUs()[1], r2->collectIUs()[0]);
   auto predicates23 = JoinConditionList::create(r2->collectIUs()[1], r3->collectIUs()[0]);

   auto join12 = std::make_unique<InnerJoin>(std::move(r1), std::move(r2), std::move(predicates12), joinMode1);
   join12->setJoinType(JoinType::RightSemi);
   auto join123 = std::make_unique<InnerJoin>(std::move(join12), std::move(r3), std::move(predicates23), joinMode2);
   join123->setJoinType(JoinType::RightSemi);
   auto ius = join123->collectIUs();
   return {std::move(join123), ius};
}
// ---------------------------------------------------------------------------------------------------
std::string TestTwoHopSemiJoin::resultPrint() const {
   return genTupleString<int, int>({{3, 5}});
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TestThreeHopJoinBushy::getInnerTree(const Database& db) const {
   auto r1 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r1");
   auto r2 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r2");
   auto r3 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r3");
   auto r4 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r4");

   auto columns = r1->collectIUs().merge(r2->collectIUs()).merge(r3->collectIUs()).merge(r4->collectIUs());
   auto predicates12 = JoinConditionList::create(r1->collectIUs()[1], r2->collectIUs()[0]);
   auto predicates23 = JoinConditionList::create(r2->collectIUs()[1], r3->collectIUs()[0]);
   auto predicates34 = JoinConditionList::create(r3->collectIUs()[1], r4->collectIUs()[0]);

   auto join23 = std::make_unique<InnerJoin>(std::move(r2), std::move(r3), std::move(predicates23), joinMode1);
   auto join123 = std::make_unique<InnerJoin>(std::move(r1), std::move(join23), std::move(predicates12), joinMode2);
   auto join1234 = std::make_unique<InnerJoin>(std::move(join123), std::move(r4), std::move(predicates34), joinMode3);

   return {std::move(join1234), columns};
}
// ---------------------------------------------------------------------------------------------------
std::string TestThreeHopJoinBushy::resultPrint() const {
   return "";
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TestThreeHopJoin::getInnerTree(const Database& db) const {
   auto r1 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r1");
   auto r2 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r2");
   auto r3 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r3");
   auto r4 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r4");

   auto columns = r1->collectIUs().merge(r2->collectIUs()).merge(r3->collectIUs()).merge(r4->collectIUs());
   auto predicates12 = JoinConditionList::create(r1->collectIUs()[1], r2->collectIUs()[0]);
   auto predicates23 = JoinConditionList::create(r2->collectIUs()[1], r3->collectIUs()[0]);
   auto predicates34 = JoinConditionList::create(r3->collectIUs()[1], r4->collectIUs()[0]);

   auto join34 = std::make_unique<InnerJoin>(std::move(r4), std::move(r3), std::move(predicates34), joinMode1);
   auto join234 = std::make_unique<InnerJoin>(std::move(join34), std::move(r2), std::move(predicates23), joinMode2);
   auto join1234 = std::make_unique<InnerJoin>(std::move(join234), std::move(r1), std::move(predicates12), joinMode3);
   return {std::move(join1234), columns};
}
// ---------------------------------------------------------------------------------------------------
std::string TestThreeHopJoin::resultPrint() const {
   return "";
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure TopInsertMultipleRequired::getInnerTree(const Database& db) const {
   auto r1 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r1");
   auto r2 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r2");
   auto r3 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r3");
   auto r4 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r4");

   auto columns = r1->collectIUs().merge(r2->collectIUs()).merge(r3->collectIUs()).merge(r4->collectIUs());
   auto predicates12 = JoinConditionList::create(r1->collectIUs()[1], r2->collectIUs()[0]);
   auto predicates234 = JoinConditionList::create({JoinCondition::create(r2->collectIUs()[1], r3->collectIUs()[1]),
                                                   JoinCondition::create(r2->collectIUs()[1], r4->collectIUs()[0])});
   auto predicates34 = JoinConditionList::create(r3->collectIUs()[1], r4->collectIUs()[0]);

   auto join34 = std::make_unique<InnerJoin>(std::move(r4), std::move(r3), std::move(predicates34), JoinMode::TopInsert);
   auto join234 = std::make_unique<InnerJoin>(std::move(join34), std::move(r2), std::move(predicates234), JoinMode::TopInsert);
   auto join1234 = std::make_unique<InnerJoin>(std::move(join234), std::move(r1), std::move(predicates12), JoinMode::TopInsert);
   return {std::move(join1234), columns};
}
// ---------------------------------------------------------------------------------------------------
std::string TopInsertMultipleRequired::resultPrint() const {
   return genTupleString<int, int, int, int, int, int, int, int>({{1, 2, 2, 3, 2, 3, 3, 5},
                                                                  {1, 2, 2, 3, 2, 3, 3, 5}});
}
// ---------------------------------------------------------------------------------------------------
TestCase::TreeStructure NotTopmostTopInsert::getInnerTree(const Database& db) const {
   auto r1 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r1");
   auto r2 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r2");
   auto r3 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r3");
   auto r4 = std::make_unique<TableScan>(db, testdb::KRelationTest, "r4");

   auto columns = r1->collectIUs().merge(r2->collectIUs()).merge(r3->collectIUs()).merge(r4->collectIUs());
   auto predicates12 = JoinConditionList::create(r1->collectIUs()[1], r2->collectIUs()[0]);
   auto predicates13 = JoinConditionList::create(r1->collectIUs()[1], r3->collectIUs()[1]);
   auto predicates34 = JoinConditionList::create(r3->collectIUs()[1], r4->collectIUs()[0]);

   auto join12 = std::make_unique<InnerJoin>(std::move(r1), std::move(r2), std::move(predicates12), JoinMode::TopInsert);
   auto join123 = std::make_unique<InnerJoin>(std::move(join12), std::move(r3), std::move(predicates13), JoinMode::TopInsert);
   auto join1234 = std::make_unique<InnerJoin>(std::move(join123), std::move(r4), std::move(predicates34), JoinMode::BottomInsert);
   return {std::move(join1234), columns};
}
// ---------------------------------------------------------------------------------------------------
std::string NotTopmostTopInsert::resultPrint() const {
   return genTupleString<int, int, int, int, int, int, int, int>({{1, 2, 2, 3, 1, 2, 2, 3}, {1, 2, 2, 3, 1, 2, 2, 3}, {1, 2, 2, 3, 1, 2, 2, 3}, {1, 2, 2, 3, 1, 2, 2, 3}, {1, 2, 2, 4, 1, 2, 2, 3}, {1, 2, 2, 4, 1, 2, 2, 3}, {1, 2, 2, 4, 1, 2, 2, 3}, {1, 2, 2, 4, 1, 2, 2, 3}, {1, 2, 2, 3, 1, 2, 2, 4}, {1, 2, 2, 3, 1, 2, 2, 4}, {1, 2, 2, 3, 1, 2, 2, 4}, {1, 2, 2, 3, 1, 2, 2, 4}, {1, 2, 2, 4, 1, 2, 2, 4}, {1, 2, 2, 4, 1, 2, 2, 4}, {1, 2, 2, 4, 1, 2, 2, 4}, {1, 2, 2, 4, 1, 2, 2, 4}, {2, 3, 3, 5, 2, 3, 3, 5}});
}
// ---------------------------------------------------------------------------------------------------
} // namespace defaultQueryTrees
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::test
// ---------------------------------------------------------------------------------------------------
