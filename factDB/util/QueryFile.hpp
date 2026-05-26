#ifndef H_FACTDB_FACTDB_UTIL_QUERYFILE_HPP
#define H_FACTDB_FACTDB_UTIL_QUERYFILE_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/config.h"
#include "factDB/infra/Setting.hpp"
#include "factDB/queryc/CxxWrapper.hpp"
#include "factDB/queryc/NewFileWriter.hpp"
#include "factDB/queryc/QueryHandle.hpp"
#include "factDB/queryc/QueryParseContext.hpp"
#include <chrono>
#include <fstream>
#include <fmt/format.h>
// ---------------------------------------------------------------------------------------------------
namespace factDB::util {
// ---------------------------------------------------------------------------------------------------
inline void generateCodeQuery(Database& db, const std::function<void(FileWriter&)>& query) {
   auto set = SettingBase::getSetting("codegen.executionMode")->setTemporary(queryc::ExecutionMode::CodegenFlat);
   factDB::queryc::QueryParseContext parseContext(db, ".cpp");
   parseContext.generateCode(query);
   auto handle = parseContext.compile(true);
   auto queryHandle = handle.loadQueryHandle();
   queryHandle(db, std::cout, queryc::PerformanceRecorderExecution{});
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
T generateCodeIterateReturning(Database& db, void* arg, const std::function<void(FileWriter&)>& query, double& runtime, double& compiletime) { // NOLINT
   auto startCompile = std::chrono::steady_clock::now();

   factDB::queryc::QueryParseContext parseContext(db, ".cpp");
   parseContext.generateCode(query);
   auto rawHandle = parseContext.compile(true);
   auto handle = rawHandle.loadFunction<T (*)(void*)>("iterate");

   auto startRuntime = std::chrono::steady_clock::now();
   auto ret = handle(arg);
   auto stopRuntime = std::chrono::steady_clock::now();

   runtime = std::chrono::duration<double>(stopRuntime - startRuntime).count();
   compiletime = std::chrono::duration<double>(startRuntime - startCompile).count();
   return ret;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::util
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_UTIL_QUERYFILE_HPP
