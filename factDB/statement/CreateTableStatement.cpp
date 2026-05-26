// ---------------------------------------------------------------------------------------------------
#include "factDB/statement/CreateTableStatement.hpp"
#include "factDB/Database.hpp"
#include "factDB/config.h"
#include "factDB/queryc/CxxWrapper.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/SchemaCompiler.hpp"
#include "factDB/schemac/Table.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::statement {
// ---------------------------------------------------------------------------------------------------
void CreateTableStatement::runStatement(factDB::Database& db, bool /*compile*/) {
   // check if table already exists
   if (db.containsRelation(tableSchema.name)) {
      std::cerr << "you want to add an exisiting table \"" << tableSchema.name << "\"" << std::endl;
      exit(1);
   }

   // add the new table schema and adjust other structs
   auto curSchemaId = db.schemas.size();
   db.tableNames2Id[tableSchema.name] = curSchemaId;
   auto& schema = db.schemas.emplace_back(tableSchema);
   schema.id = curSchemaId;
   db.filepaths.emplace_back();

   { // generate the table
      auto tableCppPath = schemac::SchemaCompiler::getSrcPath(schema.name);
      auto tableHppPath = schemac::SchemaCompiler::getHeaderPath(schema.name);
      schemac::SchemaCompiler schemaCompiler(tableHppPath, tableCppPath);
      schemaCompiler.generateTable(schema);
   }

   constexpr auto prefixLength = std::string(CURRENT_SRC_DIR "/").size();
   std::string schemaPath;
   {
      std::vector<std::string> additionalHeaders;
      std::unordered_set<std::string> seenFiles;
      for (auto& s : db.schemas) {
         auto tableHppPath = schemac::SchemaCompiler::getHeaderPath(s.name);
         if (seenFiles.contains(tableHppPath)) continue;
         additionalHeaders.emplace_back(tableHppPath.substr(prefixLength));
         seenFiles.insert(tableHppPath);
      }
      schemac::SchemaCompiler schemaCompiler(additionalHeaders, false);

      schemaCompiler.generateGenTableInfos(db.schemas);
      schemaPath = schemaCompiler.getSrcPath();
   }

   {
      auto cmakeFile = schemac::SchemaCompiler::getOutputFolder() + "/CMakeLists.txt";
      auto cmakeStream = std::ofstream(cmakeFile, std::ofstream::trunc);

      FileWriter cmakeWriter(cmakeStream, FileWriter::Silent);
      cmakeWriter << "set(TABLE_FILES" << fw::endl();

      cmakeWriter << schemaPath.substr(prefixLength) << fw::endl();
      std::unordered_set<std::string> seenFiles;
      for (auto& s : db.schemas) {
         auto tableCppPath = schemac::SchemaCompiler::getSrcPath(s.name);
         if (seenFiles.contains(tableCppPath)) continue;
         cmakeWriter << tableCppPath.substr(prefixLength) << fw::endl();
         seenFiles.insert(tableCppPath);
      }

      cmakeWriter << "PARENT_SCOPE" << fw::endl()
                  << ")" << fw::endl();
      cmakeStream.close();
   }
#if 0
   db.handle = compiler.compile(generatedCpps, libraryPath, true, true, queryc::CxxCompiler::RawHandleTag);

   // load new code for tables
   for (const auto& tbl : db.schemas) {
      auto createFun = db.handle.loadFunction<factDB::BaseTable* (*) ()>(schemac::SchemaCompiler::getCreateFunName(tbl));
      db.tables.emplace_back(createFun());
   }
#endif
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::statement
// ---------------------------------------------------------------------------------------------------