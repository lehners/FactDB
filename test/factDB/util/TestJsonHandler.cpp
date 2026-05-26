#include "factDB/infra/Setting.hpp"
#include "factDB/queryc/ExecutionMode.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "gtest/gtest.h"
#include "test/factDB/util/TestWriter.hpp"
#include "test/factDB/util/TranslationTest.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace std;
// ---------------------------------------------------------------------------------------------------
namespace factDB::test {
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
void performTest(const defaultQueryTrees::TestCase& tc) {
   auto set = SettingBase::getSetting("codegen.executionMode")->setTemporary(queryc::ExecutionMode::CodegenFlat);
   auto opt = SettingBase::getSetting<bool>("optimizer.doOptimize")->setTemporaryBool(false);
   auto db = DatabaseLoadUtil::genTestDB();

   const testing::TestInfo* test_info = testing::UnitTest::GetInstance()->current_test_info();
   std::string testPrefix = std::string("TEST_") + test_info->test_suite_name() + std::string("_") + test_info->name();
   {
      SettingBase::getSetting<std::string>("queryname")->set(testPrefix + "_Print");

      auto [operatorTree, columns] = tc.getTreePrint(db);
      auto tree = serializeAndDeserialize(db, operatorTree, columns);
      auto result = testGeneratedCode(db, std::move(tree), columns);
      TestWriter::addExpectedValue(tc.resultPrint());
   }
   {
      SettingBase::getSetting<std::string>("queryname")->set(testPrefix + "_Count");
      auto [operatorTree, columns] = tc.getTreeCount(db);
      auto tree = serializeAndDeserialize(db, operatorTree, columns);
      auto result = testGeneratedCode(db, std::move(tree), columns);
      TestWriter::addExpectedValue(tc.resultCount());
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------------------------------
TEST(factDB_util_JsonHandler, SimpleScan) {
   defaultQueryTrees::TestSimpleScan testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_util_JsonHandler, ReorderColumns) {
   defaultQueryTrees::TestReorderColumns testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_util_JsonHandler, SimpleFilter) {
   defaultQueryTrees::TestSimpleFilter testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_util_JsonHandler, MultipleFilterPredicates) {
   defaultQueryTrees::TestMultipleFilterPredicates testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_util_JsonHandler, SimpleJoin) {
   defaultQueryTrees::TestSimpleJoin testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_util_JsonHandler, TwoKeyJoin) {
   defaultQueryTrees::TestTwoKeyJoin testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_util_JsonHandler, TwoHopJoinLeftDeep) {
   defaultQueryTrees::TestTwoHopJoinLeftDeep testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_util_JsonHandler, TwoHopJoinRightDeep) {
   defaultQueryTrees::TestTwoHopJoinRightDeep testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::test
// ---------------------------------------------------------------------------------------------------
