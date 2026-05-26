// ---------------------------------------------------------------------------------------------------
#pragma once
// ---------------------------------------------------------------------------------------------------
#include <cstdint>
#include <ostream>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class Database;
// ---------------------------------------------------------------------------------------------------
namespace statement {
// ---------------------------------------------------------------------------------------------------
class Statement {
   public:
   enum StatementType {
      CreateTableStatement,
      CopyStatement,
      QueryStatement
   };

   private:
   StatementType statementType;

   protected:
   std::ostream* outStream = nullptr;

   public:
   Statement(StatementType type) : statementType(type) {}
   virtual ~Statement() = default;

   [[nodiscard]] StatementType getType() const { return statementType; }
   void setOutStream(std::ostream& stream) { outStream = &stream; }

   virtual void runStatement(Database& db) = 0;
};
// ---------------------------------------------------------------------------------------------------
} // namespace statement
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------