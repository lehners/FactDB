// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include "factDB/SQLExecution.hpp"
#include "factDB/config.h"
#include "factDB/gen/tables/table.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/queryc/CxxWrapper.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/SchemaCompiler.hpp"
#include "factDB/statement/CopyStatement.hpp"
#include "factDB/statement/CreateTableStatement.hpp"
#include "fmt/format.h"
#include <sstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
Database::Database(const std::string& relationsSQL, bool generateDefault) {
   // load default tables and generate resources for them
   SQLExecutor sqlExecutor(*this);
   if (false && generateDefault) {
      std::fstream schemaFile(CURRENT_SRC_DIR "/tools/schemas.sql");
      sqlExecutor.executeSQL(schemaFile);
   }

   if (!relationsSQL.empty()) {
      std::istringstream relationsSQLStream(relationsSQL);
      sqlExecutor.executeSQL(relationsSQLStream);
   }
}
// ---------------------------------------------------------------------------------------------------
Database::~Database() = default;
// ---------------------------------------------------------------------------------------------------
size_t Database::checkId(size_t relationId) const {
   assert(relationId < schemas.size());
   return relationId;
}
// ---------------------------------------------------------------------------------------------------
bool Database::containsRelation(std::string_view tableName) const {
   auto strTableName = std::string(tableName);
   return tableNames2Id.contains(strTableName);
}
// ---------------------------------------------------------------------------------------------------
uint32_t Database::getRelationID(const std::string_view tableName) const {
   auto strTableName = std::string(tableName);
   assert(tableNames2Id.contains(strTableName));
   return tableNames2Id.find(strTableName)->second;
}
// ---------------------------------------------------------------------------------------------------
std::vector<std::unique_ptr<IU>> Database::getColumns(uint32_t relation, const std::string_view& alias) const {
   assert(relation < schemas.size());

   std::vector<std::unique_ptr<IU>> ius;
   ius.reserve(schemas[relation].columns.size());

   for (const auto& col : schemas[relation].columns)
      ius.emplace_back(std::make_unique<IU>(alias, col.name, col.type));

   return ius;
}
// ---------------------------------------------------------------------------------------------------
void Database::loadRelation(uint32_t id) {
   assert(id < schemas.size());
   auto& [path, separator] = filepaths[id];
   assert(!path.empty() && "path for relation was not set!");
   assert(std::filesystem::exists(path) && "path for relation does not exist!");
   getTable(id).loadRelation(path, separator);
}
// ---------------------------------------------------------------------------------------------------
void Database::unloadRelation(uint32_t tableId) {
   getTable(tableId).unloadRelation();
}
// ---------------------------------------------------------------------------------------------------
bool Database::isRelationLoaded(uint32_t tableId) const {
   return getTable(tableId).loaded;
}
// ---------------------------------------------------------------------------------------------------
void Database::genTblFiles() const {
   for (size_t i = 0; i != filepaths.size(); ++i) {
      auto& [path, separator] = filepaths[i];
      if (path.empty() || !std::filesystem::exists(path)) {
         std::cout << "no csv file for " << schemas[i].name << std::endl;
         continue;
      }
      std::cout << "Gen Tbl File for " << schemas[i].name << " at " << path << std::endl;
      tables[i]->genTblFiles(path, separator);
   }
}
// ---------------------------------------------------------------------------------------------------
const std::string Database::getHppPath(uint32_t tableId) const {
   return schemac::SchemaCompiler::getHeaderIncludePath(getSchema(tableId).name);
}
// ---------------------------------------------------------------------------------------------------
Database DatabaseCreationHelper::loadDatabase() {
   auto tables = factDB::tables::genTableInfos();
   auto schemas = factDB::tables::genSchemas();
   return fromTableSchema(std::move(schemas), std::move(tables));
}
// ---------------------------------------------------------------------------------------------------
Database DatabaseCreationHelper::fromTableSchema(std::vector<schemac::Table> schemas, std::vector<std::unique_ptr<BaseTable>> tables) {
   Database db({}, false);
   db.handle = {};
   db.schemas = std::move(schemas);
   db.tables = std::move(tables);
   db.filepaths.resize(db.tables.size());

   for (size_t idx = 0; auto& schema : db.schemas) {
      db.tableNames2Id[schema.name] = schema.id;
      assert(idx == schema.id);
      ++idx;
   }

   db.tableHppDirectory = {};

   return db;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------