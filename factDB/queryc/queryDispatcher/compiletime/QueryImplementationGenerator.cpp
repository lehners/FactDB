#include "factDB/queryc/queryDispatcher/compiletime/QueryImplementationGenerator.hpp"
#include "factDB/config.h"
#include "factDB/gen/QueryHandler.hpp"
#include "factDB/queryc/ExecutionMode.hpp"
#include "factDB/queryc/NewFileWriter.hpp"
#include "factDB/queryc/QueryParseContext.hpp"
#include "factDB/queryc/queryDispatcher/runtime/KnownQueries.hpp"
#include "factDB/schemac/SchemaCompiler.hpp"
#include "factDB/util/DoOnDestruction.hpp"
#include <unordered_set>
// ---------------------------------------------------------------------------------------------------
namespace factDB::queryc {
// ---------------------------------------------------------------------------------------------------
static KnownQueries knownQueries;
// ---------------------------------------------------------------------------------------------------
void QueryImplementationGenerator::addQuery(const QueryProperties& qp) {
   knownQueries.addQuery(qp);
}
// ---------------------------------------------------------------------------------------------------
const KnownQueries& QueryImplementationGenerator::getKnownQueries() {
   return knownQueries;
}
// ---------------------------------------------------------------------------------------------------
void QueryImplementationGenerator::eliminateDuplicates() {
   knownQueries.eliminateDuplicates();
}
// ---------------------------------------------------------------------------------------------------
QueryImplementationGenerator::QueryImplementationGenerator()
   : headerStream(CURRENT_SRC_DIR "/factDB/gen/QueryHandler.hpp", std::ofstream::trunc),
     sourceStream(CURRENT_SRC_DIR "/factDB/gen/QueryImplementationBenchmark.cpp", std::ofstream::trunc),
     cmakeStream(CURRENT_SRC_DIR "/factDB/gen/query/CMakeLists.txt", std::ofstream::trunc),
     header(headerStream), source(sourceStream), cmake(cmakeStream, fw::FileWriter::Silent) {
   if (headerStream.fail() || cmakeStream.fail())
      throw std::runtime_error("failed to open ostreams in QueryImplementationGenerator");

   knownQueries.eliminateDuplicates();
   genQueryHandler();
   genCMake();
   genDispatcher();
}
// ---------------------------------------------------------------------------------------------------
void QueryImplementationGenerator::genQueryHandler() {
   header << "#pragma once" << fw::endl()
          << fw::endl()
          << "#include \"factDB/queryc/queryDispatcher/compiletime/QueryImplementationsMacro.hpp\"" << fw::endl()
          << fw::endl()
          << "namespace factDB::generated {" << fw::endl()
          << "enum Query {" << fw::endl()
          << fw::iter(knownQueries.querynames(), ",", true) << fw::endl()
          << "};" << fw::endl()
          << fw::endl()
          << "template <factDB::queryc::ExecutionMode execution, Query q>" << fw::endl()
          << "struct QuerySelector;" << fw::endl()
          << fw::endl();
   for (const auto& qp : knownQueries) {
      header << fw::fmt("// QUERY_IMPLEMENTATION({}, {});", executionModeParser.output(qp.mode) + qp.genModeString(), qp.queryname) << fw::endl();
   }
   header << fw::endl()
          << "inline std::vector<std::tuple<factDB::queryc::ExecutionMode, std::string, bool>> queries = {" << fw::endl();
   for (const auto& qp : knownQueries) {
      header << fw::fmt("{{ queryc::{}, \"{}\", {} }},", executionModeParser.output(qp.mode), qp.queryname, qp.getFlagUInt()) << fw::endl();
   }
   header << "};" << fw::endl()
          << fw::endl()
          << "} // namespace" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void QueryImplementationGenerator::genCMake() {
   cmake << "set(QUERY_FILES" << fw::endl();
   for (const auto& qp : knownQueries) {
      cmake << QueryParseContext::getPathPregenerated(qp, ".cpp") << fw::endl();
   }
   cmake << "   # factDB/gen/QueryImplementationBenchmark.cpp" << fw::endl()
         << "   PARENT_SCOPE" << fw::endl()
         << ")" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void QueryImplementationGenerator::genDispatcher() {
   source << "#include \"factDB/queryc/queryDispatcher/compiletime/QueryImplementationBenchmark.hpp\"" << fw::endl()
          << fw::endl()
          << "namespace factDB::generated {" << fw::endl()
          << fw::endl()
          << "void runBenchmarks(queryc::ExecutionMode execution, Database& db) {" << fw::endl()
          << "auto qp = QueryProperties::fromCurrentSettings();" << fw::endl();
   for (const auto& qp : knownQueries)
      source << fw::fmt("if (execution == queryc::{0} && qp.getFlagUInt() == {2}) benchmarkQuery<queryc::{0}{3}, {1}>(db);", executionModeParser.output(qp.mode), qp.queryname, qp.getFlagUInt(), qp.genModeString()) << fw::endl();
   source << "}" << fw::endl()
          << "} // namespace factDB::generated" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
QueryImplementationGenerator::~QueryImplementationGenerator() {
   headerStream.close();
   sourceStream.close();
   cmakeStream.close();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::queryc
// ---------------------------------------------------------------------------------------------------
