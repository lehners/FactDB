// ---------------------------------------------------------------------------------------------------
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "factDB/SQLExecution.hpp"
#include "factDB/config.h"
#include "factDB/infra/Setting.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "fmt/format.h"
#include <iostream>
#include <sstream>
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
void DatabaseLoadUtil::loadRequiredRelations(Database& db, const std::vector<std::string>& requiredRelations) {
   for (auto b : requiredRelations) {
      std::cout << "load relation: " << b << std::endl;
      db.loadRelation(b);
   }
}
// ---------------------------------------------------------------------------------------------------
void DatabaseLoadUtil::loadTablePaths(Database& db, const std::string& schemaPath) {
   SQLExecutor sqlExecutor(db);
   std::fstream schema(schemaPath, std::ios_base::in);
   if (!schema) {
      throw std::runtime_error("Load SQL file was not opened succesfully");
   }
   sqlExecutor.executeSQL(schema);
   schema.close();
}
// ---------------------------------------------------------------------------------------------------
[[nodiscard]] factDB::Database DatabaseLoadUtil::genEmptyDB() {
   Database db({}, false);
   return db;
}
// ---------------------------------------------------------------------------------------------------
[[nodiscard]] factDB::Database DatabaseLoadUtil::genUnloadedDB() {
   auto db = DatabaseCreationHelper::loadDatabase();
   return db;
}
// ---------------------------------------------------------------------------------------------------
[[nodiscard]] factDB::Database DatabaseLoadUtil::genTestDB() {
   SettingBase::getSetting<std::string>("schema.key")->set("testdb");
   auto db = DatabaseCreationHelper::loadDatabase();

   SQLExecutor sqlExecutor(db);
   sqlExecutor.executeSQL(fmt::format("COPY {} FROM '{}/test/factDB/kTestRelation.csv' DELIMITER ','", testdb::KRelationTest, CURRENT_SRC_DIR));
   loadRequiredRelations(db, {"KRelationTest"});

   return db;
}
// ---------------------------------------------------------------------------------------------------
Database DatabaseLoadUtil::genSSB(size_t scale_factor, const std::vector<std::string>& requiredRelations, bool needTables) {
   auto db = DatabaseCreationHelper::loadDatabase();
   if (needTables)
      loadTablePaths(db, fmt::format("{}/bench/ssb/sf{}/load.sql", CURRENT_SRC_DIR, scale_factor));
   loadRequiredRelations(db, requiredRelations);
   return db;
}
// ---------------------------------------------------------------------------------------------------
Database DatabaseLoadUtil::genCE(const std::vector<std::string>& requiredRelations, bool needTables) {
   auto db = DatabaseCreationHelper::loadDatabase();
   if (needTables)
      loadTablePaths(db, CURRENT_SRC_DIR "/bench/ce/load.sql");
   loadRequiredRelations(db, requiredRelations);
   return db;
}
// ---------------------------------------------------------------------------------------------------
Database DatabaseLoadUtil::genArtificial(const std::string& inPath, const std::vector<std::string>& requiredRelations) {
   auto db = DatabaseCreationHelper::loadDatabase();
   loadTablePaths(db, inPath);
   loadRequiredRelations(db, requiredRelations);
   return db;
}
// ---------------------------------------------------------------------------------------------------
Database DatabaseLoadUtil::genById(std::string_view schemaIdentifier, const std::vector<std::string>& requiredRelations, bool needTables, const std::string& inPath) {
   if (schemaIdentifier == "testdb") {
      return genTestDB();
   } else if (schemaIdentifier == "ce") {
      return genCE(requiredRelations, needTables);
   } else if (schemaIdentifier == "artificial") {
      if (inPath.empty()) throw std::runtime_error("inPath not set for artificial benchmark");
      return genArtificial(inPath, requiredRelations);
   } else if (schemaIdentifier == "ssb_sf1") {
      return genSSB(1, requiredRelations, needTables);
   } else if (schemaIdentifier == "ssb_sf10") {
      return genSSB(10, requiredRelations, needTables);
   } else {
      std::cout << "unknown Schema Identifier: \"" << schemaIdentifier << "\"" << std::endl;
      throw std::runtime_error("unkown schema!");
   }
}
// ---------------------------------------------------------------------------------------------------
