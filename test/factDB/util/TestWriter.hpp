#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/queryDispatcher/runtime/KnownQueries.hpp"
#include "factDB/queryc/writer/FileWriter.hpp"
#include <fstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class TestWriter {
   std::ofstream out;
   factDB::FileWriter writer;

   public:
   TestWriter();
   ~TestWriter();

   static void addExpectedValue(const std::string expected);
   void addTestCase(const queryc::QueryProperties& qd);

   static void generateCompilingTestCases();
   static std::string runQuery(queryc::ExecutionMode mode, std::string queryname, queryc::QueryFlag flags, bool sortLines);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB