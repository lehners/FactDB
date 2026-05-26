// ---------------------------------------------------------------------------------------------------
#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/statement/Statement.hpp"
#include <string>
// ---------------------------------------------------------------------------------------------------
namespace factDB::statement {
// ---------------------------------------------------------------------------------------------------
class CopyStatement : public Statement {
   private:
   std::string tableName;
   std::string filename;
   char separator;

   public:
   CopyStatement(std::string table, std::string file, char sep = ',') : Statement(Statement::CopyStatement), tableName(std::move(table)), filename(std::move(file)), separator(sep) {} // NOLINT(bugprone-easily-swappable-parameters)
   ~CopyStatement() override = default;

   void runStatement(Database& db) override;

   static CopyStatement* dynCast(Statement* stmt) { return stmt->getType() == Statement::CopyStatement ? static_cast<CopyStatement*>(stmt) : nullptr; }
   static const CopyStatement* dynCast(const Statement* stmt) { return stmt->getType() == Statement::CopyStatement ? static_cast<const CopyStatement*>(stmt) : nullptr; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::statement
// ---------------------------------------------------------------------------------------------------