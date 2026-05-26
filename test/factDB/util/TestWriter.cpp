#include "test/factDB/util/TestWriter.hpp"
#include "factDB/config.h"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/util/LineSortingStream.hpp"
#include "factDB/queryc/PerformanceRecord.hpp"
#include "factDB/queryc/QueryParseContext.hpp"
#include "factDB/queryc/queryDispatcher/compiletime/QueryImplementationGenerator.cpp"
#include "factDB/queryc/queryDispatcher/runtime/QueryDispatcher.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "gtest/gtest.h"
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
static constexpr auto testFilePath = CURRENT_SRC_DIR "/test/factDB/GeneratedTests.cpp";
static constexpr auto cmakeFilePath = CURRENT_SRC_DIR "/test/factDB/CMakeLists.txt";
// ---------------------------------------------------------------------------------------------------
TestWriter::TestWriter() : out(testFilePath), writer(out) {
   if (!out) {
      throw std::runtime_error("Failed to open file for writing: " + std::string(testFilePath));
   }
   writer.addInclude("gtest/gtest.h")
         .addInclude("test/factDB/util/TestWriter.hpp")
      << fw::context_switch() << fw::endl()
      << "using namespace factDB;" << fw::endl()
      << fw::context_switch() << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
TestWriter::~TestWriter() {
   out.close();
}
// ---------------------------------------------------------------------------------------------------
static std::unordered_map<std::string, std::string> query2expected;
// ---------------------------------------------------------------------------------------------------
void TestWriter::addExpectedValue(const std::string expected) {
   auto queryname = SettingBase::getSetting<std::string>("queryname")->get();
   assert(!query2expected.contains(queryname));
   query2expected[queryname] = expected;
}
// ---------------------------------------------------------------------------------------------------
void TestWriter::addTestCase(const queryc::QueryProperties& qd) {
   std::string should = query2expected[qd.queryname];

   writer << fw::fmt("TEST(GENERATED_factDB, {}) {{", qd.queryname) << fw::endl()
          << fw::fmt("  auto res = TestWriter::runQuery(queryc::ExecutionMode::{}, \"{}\", static_cast<queryc::QueryFlag>({}), true);", queryc::executionModeParser.output(qd.mode), qd.queryname, static_cast<uint8_t>(qd.flags)) << fw::endl()
          << fw::fmt("  ASSERT_EQ(res, {});", fw::str(should)) << fw::endl()
          << fw::fmt("}}") << fw::endl()
          << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void TestWriter::generateCompilingTestCases() {
   if (query2expected.empty())
      return;
   TestWriter tw;
   std::ofstream cmakeFile(cmakeFilePath);
   FileWriter cmake(cmakeFile, FileWriter::Silent);
   if (!cmakeFilePath)
      throw std::runtime_error("Failed to open file for writing: " + std::string(testFilePath));

   cmake << "# !WARNING! This file is generated, do not hand-edit this file!" << fw::endl()
         << "set(TEST_QUERY_FILES" << fw::endl();

   queryc::QueryImplementationGenerator::eliminateDuplicates();
   for (auto& desc : queryc::QueryImplementationGenerator::getKnownQueries()) {
      if (query2expected.contains(desc.queryname)) {
         tw.addTestCase(desc);
         cmake << queryc::QueryParseContext::getPathPregenerated(desc, ".cpp") << fw::endl();
      }
   }

   cmake << "  PARENT_SCOPE" << fw::endl()
         << ")" << fw::endl();
   cmakeFile.close();
}
// ---------------------------------------------------------------------------------------------------
std::string TestWriter::runQuery(queryc::ExecutionMode mode, std::string queryname, queryc::QueryFlag flags, bool sortLines) {
   LineSortingStream lss;
   auto desc = factDB::queryc::QueryDispatcher::getQueryDescriptor({mode, queryname, flags});
   auto db = DatabaseLoadUtil::genTestDB();
   desc.query(db, lss, queryc::PerformanceRecorderExecution{});
   if (sortLines)
      lss.sortLines();
   return lss.concatLines();
}
// ---------------------------------------------------------------------------------------------------
