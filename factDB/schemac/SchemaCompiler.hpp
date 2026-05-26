#ifndef H_FACTDB_FACT_DB_SCHEMAC_SCHEMACOMPILER_H_
#define H_FACTDB_FACT_DB_SCHEMAC_SCHEMACOMPILER_H_
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/FileWriter.hpp"
#include <fstream>
#include <unordered_set>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::schemac {
// ---------------------------------------------------------------------------------------------------
struct Table;
// ---------------------------------------------------------------------------------------------------
class SchemaCompiler {
   private:
   std::ofstream headerStream, sourceStream;
   fw::FileWriter header, source;
   size_t tableId;

   std::unordered_set<std::string> generatedTables;

   FWContainer headerGuard(const std::string& header_path);

   void startFiles(const std::string& header_path, const std::vector<std::string>& additionalSrcHeaders);
   void finishFiles();

   void generateColumnsEnum(const schemac::Table& schema);
   void generateDataVectors(const schemac::Table& schema);
   void generateSize(const schemac::Table& schema);
   void generateLoadRelation(const schemac::Table& schema);
   void generateUnloadRelation(const schemac::Table& schema);
   void generateGetValueImpls(const schemac::Table& schema);
   void generateGet(const schemac::Table& schema);
   void generateTableInfos(const schemac::Table& schema);
   void generateTblFiles(const schemac::Table& schema);

   public:
   // Constructor
   SchemaCompiler(const std::vector<std::string>& additionalHeaders = {}, bool headerFullPath = false);
   SchemaCompiler(const std::string& headerPath, const std::string& sourcePath, const std::vector<std::string>& additionalHeaders = {}, bool headerFillPath = false);
   ~SchemaCompiler();

   // generate a table
   void generateTable(const schemac::Table& schema);

   void generateGenTableInfos(const std::vector<schemac::Table>& schemas);

   std::string getSrcPath();
   std::string getHeaderPath();
   std::string getLibraryPath();

   static std::string getSrcPath(const std::string& tableName);
   static std::string getHeaderPath(const std::string& tableName);
   static std::string getHeaderIncludePath(const std::string& tableName);

   static std::string getCreateFunName(const schemac::Table& schema);
   static std::string getOutputFolder();
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::schemac
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACT_DB_SCHEMAC_SCHEMACOMPILER_H_
// ---------------------------------------------------------------------------------------------------
