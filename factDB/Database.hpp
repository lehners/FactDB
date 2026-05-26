#pragma once

#include "factDB/infra/Types.hpp"
#include "factDB/queryc/QueryHandle.hpp"
#include "factDB/schemac/Table.hpp"
#include <cstdint>
#include <istream>
#include <unordered_map>
#include <vector>

namespace factDB {

namespace testdb {
inline constexpr std::string_view KRelationTest = "KRelationTest";
inline constexpr std::string_view KRelationTestThree = "KRelationTestThree";
inline constexpr std::string_view KPersonKnowsPerson = "KPersonKnowsPerson";
inline constexpr std::string_view KPerson = "KPerson";
} // namespace testdb

namespace winf {
inline constexpr std::string_view WINF_A = "WINF_A";
inline constexpr std::string_view WINF_B = "WINF_B";
inline constexpr std::string_view WINF_C = "WINF_C";
inline constexpr std::string_view WINF_D = "WINF_D";
} // namespace winf

namespace statement {
class CreateTableStatement;
class CopyStatement;
} // namespace statement

class IU;
struct BaseTable;

class Database {
   friend class statement::CreateTableStatement;
   friend class statement::CopyStatement;
   friend struct DatabaseCreationHelper;

   /// the handle for the generated & compiled tables
   factDB::queryc::RawHandle handle; // make sure that code for destructor is still loaded at the end
   /// the table schemas
   std::vector<schemac::Table> schemas;
   /// the loaded tables (empty if no data is loaded)
   std::vector<std::unique_ptr<BaseTable>> tables;
   /// filepath per table for lazy loading of tables
   std::vector<std::tuple<std::string, char>> filepaths;
   /// map from table name to the index for faster search
   std::unordered_map<std::string, uint32_t> tableNames2Id;
   /// include path for the generated tables.
   std::string tableHppDirectory;

   private:
   size_t checkId(size_t relationId) const;

   public:
   explicit Database(const std::string& relationSQL = "", bool generateDefault = true);
   ~Database();

   Database(const Database&) noexcept = delete;
   Database& operator=(const Database&) noexcept = delete;
   Database(Database&&) noexcept = default;
   Database& operator=(Database&& other) noexcept = default;

   void loadRelation(uint32_t tableId);
   bool isRelationLoaded(uint32_t tableId) const;
   void unloadRelation(uint32_t tableId);
   BaseTable& getTable(size_t tableId) { return *tables[checkId(tableId)]; }

   void genTblFiles() const;

   bool containsRelation(const std::string_view tableName) const;
   uint32_t getRelationID(const std::string_view tableName) const;
   const BaseTable& getTable(size_t tableId) const { return *tables[checkId(tableId)]; }
   const schemac::Table& getSchema(uint32_t tableId) const { return schemas[checkId(tableId)]; }
   const std::string getHppPath(uint32_t tableId) const;
   std::vector<std::unique_ptr<IU>> getColumns(uint32_t relation, const std::string_view& alias) const;

   BaseTable& getTable(const std::string_view tableName) { return getTable(getRelationID(tableName)); }
   void loadRelation(const std::string_view tableName) { return loadRelation(getRelationID(tableName)); }
};

struct DatabaseCreationHelper {
   static Database loadDatabase();
   static Database fromTableSchema(std::vector<schemac::Table> schemas, std::vector<std::unique_ptr<BaseTable>> tables);
};

} // namespace factDB
