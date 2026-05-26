#include "factDB/infra/Setting.hpp"
#include "factDB/queryc/ExecutionMode.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "gtest/gtest.h"
#include "test/factDB/util/TranslationTest.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace std;
// ---------------------------------------------------------------------------------------------------
namespace factDB::test {
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
void performTest(const defaultQueryTrees::TestCase& tc) {
   auto set = SettingBase::getSetting("codegen.executionMode")->setTemporary(queryc::ExecutionMode::PushModel);
   auto opt = SettingBase::getSetting<bool>("optimizer.doOptimize")->setTemporaryBool(false);
   auto db = DatabaseLoadUtil::genTestDB();
   {
      auto [operatorTree, columns] = tc.getTreePrint(db);
      auto result = testGeneratedCode(db, std::move(operatorTree), columns);
      ASSERT_EQ(result, tc.resultPrint());
   }
   {
      auto [operatorTree, columns] = tc.getTreeCount(db);
      auto result = testGeneratedCode(db, std::move(operatorTree), columns);
      ASSERT_EQ(result, tc.resultCount());
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_PushModelExecution, SimpleScan) {
   defaultQueryTrees::TestSimpleScan testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_PushModelExecution, ReorderColumns) {
   defaultQueryTrees::TestReorderColumns testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_PushModelExecution, SimpleFilter) {
   defaultQueryTrees::TestSimpleFilter testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_PushModelExecution, MultipleFilterPredicates) {
   defaultQueryTrees::TestMultipleFilterPredicates testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_PushModelExecution, SimpleJoin) {
   defaultQueryTrees::TestSimpleJoin testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_PushModelExecution, SimpleRightSemiJoin) {
   defaultQueryTrees::TestSimpleRightSemiJoin testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_PushModelExecution, TwoKeyJoin) {
   defaultQueryTrees::TestTwoKeyJoin testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_PushModelExecution, TwoHopJoinLeftDeep) {
   defaultQueryTrees::TestTwoHopJoinLeftDeep testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_PushModelExecution, TwoHopJoinRightDeep) {
   auto set = SettingBase::getSetting("codegen.executionMode")->setTemporary(queryc::ExecutionMode::CodegenFlat);
   defaultQueryTrees::TestTwoHopJoinRightDeep testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_algebra_visitors_PushModelExecution, TwoHopSemiJoin) {
   defaultQueryTrees::TestTwoHopSemiJoin testCase;
   defaultQueryTrees::TestCase::performTest(testCase, performTest);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::test
// ---------------------------------------------------------------------------------------------------