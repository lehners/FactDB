// ---------------------------------------------------------------------------------------------------
#include "factDB/parser/SemanticAnalysis.hpp"
#include "factDB/Database.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/RuntimeException.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/parser/AST.hpp"
#include "factDB/statement/CopyStatement.hpp"
#include "factDB/statement/CreateTableStatement.hpp"
#include "fmt/format.h"
// ---------------------------------------------------------------------------------------------------
namespace factDB::parser {
// ---------------------------------------------------------------------------------------------------
void Scope::addIU(const factDB::IU* iu) {
   lookupIUs.insert({std::string(iu->column), iu});
}
// ---------------------------------------------------------------------------------------------------
const IU* Scope::findIU(const std::string& iuColumnName, const std::string* tupleVarName) const {
   auto fmtColumn = [&]() {
      return (tupleVarName == nullptr) ? iuColumnName : fmt::format("{}.{}", *tupleVarName, iuColumnName);
   };

   auto equalRange = lookupIUs.equal_range(iuColumnName);
   if (equalRange.first == equalRange.second)
      throw RuntimeException(RuntimeError, fmt::format("column {} not found", fmtColumn()));
   const IU* matchingIU = nullptr;
   for (auto& iter = equalRange.first; iter != equalRange.second; ++iter) {
      if (tupleVarName != nullptr && *tupleVarName != iter->second->table)
         continue;

      if (matchingIU)
         throw RuntimeException(RuntimeError, fmt::format("column reference {} is ambiguous", fmtColumn()));
      assert(iter->second->column == iuColumnName);
      matchingIU = iter->second;
   }

   if (matchingIU == nullptr)
      throw RuntimeException(RuntimeError, fmt::format("column {} not found", fmtColumn()));
   return matchingIU;
}
// ---------------------------------------------------------------------------------------------------
std::string SemanticAnalysis::extractName(parser::AST* ast) {
   Identifier* identifier = Identifier::dynCast(ast);
   if (identifier != nullptr) {
      return std::string(identifier->getIdentifierValue());
   }
   return "?column?";
}
// ---------------------------------------------------------------------------------------------------
const std::string* SemanticAnalysis::extractNamePtr(parser::AST* ast) {
   if (!ast) return nullptr;
   auto* identifier = Identifier::dynCast(ast);
   assert(identifier != nullptr && "Something ain't right");
   return identifier->getIdentifierPtr();
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<statement::Statement> SemanticAnalysis::analyseSelectStmt(parser::Select& select) {
   return analyseSelect(select);
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<statement::Statement> SemanticAnalysis::analyseCopyTable(parser::CopyTable& copyTable) {
   if (!db.containsRelation(copyTable.table))
      throw RuntimeException(RuntimeError, fmt::format("table {} does not exist", copyTable.table));
   if (copyTable.delimiter.size() != 1)
      throw RuntimeException(RuntimeError, "currently only char separators are supported");

   return std::make_unique<statement::CopyStatement>(copyTable.table, copyTable.file, copyTable.delimiter[0]);
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<statement::Statement> SemanticAnalysis::analyseCreateTable(parser::CreateTable& table) {
   if (db.containsRelation(table.getTable().name))
      throw RuntimeException(RuntimeError, fmt::format("table {} already exists", table.getTable().name));

   return std::make_unique<statement::CreateTableStatement>(table.getTable());
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<statement::Statement> SemanticAnalysis::buildStatement(parser::AST& ast, factDB::Database& db) {
   SemanticAnalysis semana(db);
   switch (ast.getASTType()) {
      case AST::SelectType: return semana.analyseSelectStmt(*Select::dynCast(&ast));
      case AST::CopyTableType: return semana.analyseCopyTable(*CopyTable::dynCast(&ast));
      case AST::CreateTableType: {
         auto castRes = CreateTable::dynCast(&ast);
         return semana.analyseCreateTable(*castRes);
      }

      default: unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::parser
// ---------------------------------------------------------------------------------------------------
