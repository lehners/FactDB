#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct DatabaseLoadUtil {
   static void loadTablePaths(Database& db, const std::string& schemaPath);
   static void loadRequiredRelations(Database& db, const std::vector<std::string>& requiredRelations);

   [[nodiscard]] static factDB::Database genEmptyDB(); // returns a Database without relations
   [[nodiscard]] static factDB::Database genUnloadedDB(); // returns a Database with all relations but nothing loaded
   [[nodiscard]] static factDB::Database genTestDB();
   [[nodiscard]] static factDB::Database genSSB(size_t scale_factor, const std::vector<std::string>& requiredRelations = {}, bool needTables = true);
   [[nodiscard]] static factDB::Database genCE(const std::vector<std::string>& requiredRelations = {}, bool needTables = true);
   [[nodiscard]] static factDB::Database genArtificial(const std::string& inPath, const std::vector<std::string>& requiredRelations = {});
   [[nodiscard]] static factDB::Database genById(std::string_view schemaIdentifier, const std::vector<std::string>& requiredRelations = {}, bool needTables = true, const std::string& inPath = "");
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------