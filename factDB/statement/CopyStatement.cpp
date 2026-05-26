// ---------------------------------------------------------------------------------------------------
#include "factDB/statement/CopyStatement.hpp"
#include "factDB/Database.hpp"
#include "factDB/config.h"
#include "factDB/infra/RuntimeException.hpp"
#include "factDB/infra/Setting.hpp"
#include "fmt/format.h"
#include <filesystem>
// ---------------------------------------------------------------------------------------------------
namespace factDB::statement {
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<std::string> copyFileInputPrefix("stmt.copy.inputPrefix", "");
// ---------------------------------------------------------------------------------------------------
void CopyStatement::runStatement(factDB::Database& db) {
   std::string fullFilename;
   if (filename.starts_with("/"))
      fullFilename = filename;
   else
      fullFilename = CURRENT_SRC_DIR "/" + copyFileInputPrefix.get() + filename;
   if (!std::filesystem::exists(fullFilename))
      throw RuntimeException(RuntimeError, fmt::format("Input file {} does not exist", fullFilename));

   auto relationId = db.getRelationID(tableName);
   db.filepaths[relationId] = {fullFilename, separator};
   db.unloadRelation(relationId); // content may change, reload necessary
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::statement
// ---------------------------------------------------------------------------------------------------