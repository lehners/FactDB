// ---------------------------------------------------------------------------------------------------
#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/statement/Statement.hpp"
#include <string>
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB::schemac { struct Table; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::statement {
// ---------------------------------------------------------------------------------------------------
class CreateTableStatement : public Statement {
   private:
   const schemac::Table& tableSchema;

   public:
   CreateTableStatement(const schemac::Table& tbl) : Statement(Statement::CreateTableStatement), tableSchema(tbl) {}
   ~CreateTableStatement() override = default;

   virtual void runStatement(Database& db, bool compile);
   void runStatement(Database& db) override { runStatement(db, true); }

   static CreateTableStatement* dynCast(Statement* stmt) { return stmt->getType() == Statement::CreateTableStatement ? static_cast<CreateTableStatement*>(stmt) : nullptr; }
   static const CreateTableStatement* dynCast(const Statement* stmt) { return stmt->getType() == Statement::CreateTableStatement ? static_cast<const CreateTableStatement*>(stmt) : nullptr; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::statement
// ---------------------------------------------------------------------------------------------------