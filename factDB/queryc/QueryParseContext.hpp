#ifndef H_FACTDB_FACTDB_QUERYC_QUERYPARSECONTEXT_HPP
#define H_FACTDB_FACTDB_QUERYC_QUERYPARSECONTEXT_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/Operator.hpp"
#include "factDB/algebra/visitors/CodegenExpanded.hpp"
#include "factDB/config.h"
#include "factDB/infra/SettingParser.hpp"
#include "factDB/queryc/CxxWrapper.hpp"
#include "factDB/queryc/ExecutionMode.hpp"
#include "factDB/queryc/NewFileWriter.hpp"
#include "factDB/queryc/PerformanceRecord.hpp"
#include "factDB/queryc/QueryCompiler.hpp"
#include <fstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::queryc {
// ---------------------------------------------------------------------------------------------------
struct QueryProperties;
// ---------------------------------------------------------------------------------------------------
class QueryParseContext {
   Database& database;
   unsigned queryIdx;
   CxxCompiler cxxCompiler;
   std::string source;
   std::ofstream stream;
   PerformanceRecord record;
   QueryCompiler compiler;

   FileWriter getFileWriter();

   public:
   static unsigned idx;
   explicit QueryParseContext(Database& db, const std::string& suffix = "cpp");
   ~QueryParseContext();

   void generateCodeCodegenVisitor(std::unique_ptr<algebra::Operator> tree, const OrderedIUSet& requiredColumns, bool optimize);
   void generateCode(const std::function<void(FileWriter&)>& query);

   RawHandle compile(bool useGtest = false);

   void prepareCode(std::unique_ptr<algebra::Operator> tree, const OrderedIUSet& requiredColumns, bool optimize = true);
   void compileAndExecute(std::unique_ptr<algebra::Operator> tree, const OrderedIUSet& requiredColumns, factDB::Database& db, std::ostream& outStream, bool optimize = true, bool cacheRelations = false);

   private:
   void executeVisitor(std::unique_ptr<algebra::Operator> tree, const OrderedIUSet& requiredColumns, factDB::Database& db, std::ostream& outStream, bool optimize);
   void compileAndExecuteVisitor(std::unique_ptr<algebra::Operator> tree, const OrderedIUSet& requiredColumns, factDB::Database& db, std::ostream& outStream, bool optimize);

   public:
   static std::unique_ptr<factDB::algebra::visitors::CodegenExpanded> getCodegenVisitor(const Database& db, FileWriter& stream);
   static std::string getOutputFolder();
   static std::string getPathPregenerated(const QueryProperties& qp, const std::string& suffix);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::queryc
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_QUERYC_QUERYPARSECONTEXT_HPP
// ---------------------------------------------------------------------------------------------------
