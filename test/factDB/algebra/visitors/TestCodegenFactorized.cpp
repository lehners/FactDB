#include "factDB/infra/Setting.hpp"
#include "factDB/newftree/FNode.hpp"
#include "factDB/queryc/ExecutionMode.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "gtest/gtest.h"
#include "test/factDB/util/TestWriter.hpp"
#include "test/factDB/util/TranslationTest.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
using namespace std;
// ---------------------------------------------------------------------------------------------------
namespace factDB::test {
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
using namespace factDB::algebra;
// ---------------------------------------------------------------------------------------------------
void performTest(const defaultQueryTrees::TestCase& tc) {
   auto set = SettingBase::getSetting("codegen.executionMode")->setTemporary(queryc::ExecutionMode::CodegenFactorized);
   auto opt = SettingBase::getSetting<bool>("optimizer.doOptimize")->setTemporaryBool(false);
   auto db = DatabaseLoadUtil::genTestDB();

   const testing::TestInfo* test_info = testing::UnitTest::GetInstance()->current_test_info();
   std::string testPrefix = std::string("TEST_fact") + test_info->test_suite_name() + std::string("_") + test_info->name();

   {
      SettingBase::getSetting<std::string>("queryname")->set(testPrefix + "_Print");
      FNode::resetFNodeId();
      auto [operatorTree, columns] = tc.getTreePrint(db);
      auto result = testGeneratedCode(db, std::move(operatorTree), columns);
      TestWriter::addExpectedValue(tc.resultPrint());
   }
   {
      SettingBase::getSetting<std::string>("queryname")->set(testPrefix + "_Count");
      FNode::resetFNodeId();
      auto [operatorTree, columns] = tc.getTreeCount(db);
      auto result = testGeneratedCode(db, std::move(operatorTree), columns);
      TestWriter::addExpectedValue(tc.resultCount());
   }
}
// ---------------------------------------------------------------------------------------------------
void performTestOnlyPrint(const defaultQueryTrees::TestCase& tc) {
   auto set = SettingBase::getSetting("codegen.executionMode")->setTemporary(queryc::ExecutionMode::CodegenFactorized);
   auto opt = SettingBase::getSetting<bool>("optimizer.doOptimize")->setTemporaryBool(false);
   auto db = DatabaseLoadUtil::genTestDB();

   const testing::TestInfo* test_info = testing::UnitTest::GetInstance()->current_test_info();
   std::string testPrefix = std::string("TEST_fact_") + test_info->test_suite_name() + std::string("_") + test_info->name();

   {
      SettingBase::getSetting<std::string>("queryname")->set(testPrefix + "_Print");
      auto [operatorTree, columns] = tc.getTreePrint(db);
      auto result = testGeneratedCode(db, std::move(operatorTree), columns);
      TestWriter::addExpectedValue(tc.resultPrint());
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, SimpleScan) {
   defaultQueryTrees::TestSimpleScan testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, ReorderColumns) {
   defaultQueryTrees::TestReorderColumns testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, SimpleFilter) {
   defaultQueryTrees::TestSimpleFilter testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, MultipleFilterPredicates) {
   defaultQueryTrees::TestMultipleFilterPredicates testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, SimpleJoinTopInsert) {
   defaultQueryTrees::TestSimpleJoin testCase(JoinMode::TopInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, SimpleJoinBottomInsert) {
   defaultQueryTrees::TestSimpleJoin testCase(JoinMode::BottomInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, SimpleRightSemiJoin) {
   defaultQueryTrees::TestSimpleRightSemiJoin testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, TwoKeyJoinTopInsert) {
   defaultQueryTrees::TestTwoKeyJoin testCase(JoinMode::TopInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, TwoKeyJoinBottomInsert) {
   defaultQueryTrees::TestTwoKeyJoin testCase(JoinMode::BottomInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, TwoHopJoinLeftDeepTopInserts) {
   defaultQueryTrees::TestTwoHopJoinLeftDeep testCase(JoinMode::TopInsert, JoinMode::TopInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, TwoHopJoinLeftDeepBottomInserts) {
   defaultQueryTrees::TestTwoHopJoinLeftDeep testCase(JoinMode::BottomInsert, JoinMode::BottomInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, TwoHopJoinLeftDeepTopFirst) {
   defaultQueryTrees::TestTwoHopJoinLeftDeep testCase(JoinMode::TopInsert, JoinMode::BottomInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, TwoHopJoinLeftDeepBottomFirst) {
   defaultQueryTrees::TestTwoHopJoinLeftDeep testCase(JoinMode::BottomInsert, JoinMode::TopInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, ThreeHopJoin) {
   defaultQueryTrees::TestThreeHopJoin testCase(JoinMode::BottomInsert, JoinMode::BottomInsert, JoinMode::TopInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, ThreeThreeHopJoinBushyTiTiTi) {
   defaultQueryTrees::TestThreeHopJoinBushy testCase(JoinMode::TopInsert, JoinMode::TopInsert, JoinMode::TopInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, ThreeThreeHopJoinBushyTiTiBi) {
   defaultQueryTrees::TestThreeHopJoinBushy testCase(JoinMode::TopInsert, JoinMode::TopInsert, JoinMode::BottomInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, ThreeThreeHopJoinBushyTiBiBi) {
   defaultQueryTrees::TestThreeHopJoinBushy testCase(JoinMode::TopInsert, JoinMode::BottomInsert, JoinMode::BottomInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, ThreeThreeHopJoinBushyTiBiTi) {
   defaultQueryTrees::TestThreeHopJoinBushy testCase(JoinMode::TopInsert, JoinMode::BottomInsert, JoinMode::TopInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, ThreeThreeHopJoinBushyBiTiTi) {
   defaultQueryTrees::TestThreeHopJoinBushy testCase(JoinMode::BottomInsert, JoinMode::TopInsert, JoinMode::TopInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, ThreeThreeHopJoinBushyBiTiBi) {
   defaultQueryTrees::TestThreeHopJoinBushy testCase(JoinMode::BottomInsert, JoinMode::TopInsert, JoinMode::BottomInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, ThreeThreeHopJoinBushyBiBiTi) {
   defaultQueryTrees::TestThreeHopJoinBushy testCase(JoinMode::BottomInsert, JoinMode::BottomInsert, JoinMode::TopInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, ThreeThreeHopJoinBushyBiBiBi) {
   defaultQueryTrees::TestThreeHopJoinBushy testCase(JoinMode::BottomInsert, JoinMode::BottomInsert, JoinMode::BottomInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, TopInsertMultipleRequired) {
   defaultQueryTrees::TopInsertMultipleRequired testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, TwoHopJoinRightDeepTiTi) {
   defaultQueryTrees::TestTwoHopJoinRightDeep testCase(JoinMode::TopInsert, JoinMode::TopInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, TwoHopJoinRightDeepBiBi) {
   defaultQueryTrees::TestTwoHopJoinRightDeep testCase(JoinMode::BottomInsert, JoinMode::BottomInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, TwoHopJoinRightDeepTiBi) {
   defaultQueryTrees::TestTwoHopJoinRightDeep testCase(JoinMode::TopInsert, JoinMode::BottomInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, TwoHopJoinRightDeepBiTi) {
   defaultQueryTrees::TestTwoHopJoinRightDeep testCase(JoinMode::BottomInsert, JoinMode::TopInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, TwoHopSemiJoin) {
   defaultQueryTrees::TestTwoHopSemiJoin testCase(JoinMode::TopInsert, JoinMode::TopInsert);
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_CodegenFactorized, NotTopmostTopInsert) {
   defaultQueryTrees::NotTopmostTopInsert testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTestOnlyPrint);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::test
// ---------------------------------------------------------------------------------------------------