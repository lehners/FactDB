#pragma once
// ---------------------------------------------------------------------------------------------------
#include <memory>
#include <unordered_map>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
class Expression;
class Database;
class IU;
struct OrderedIUSet;
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra { class Operator; }
namespace factDB::statement { class Statement; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::parser {
// ---------------------------------------------------------------------------------------------------
class AST;
struct CopyTable;
struct CreateTable;
class TableRef;
class Select;
class RelationExpr;
class UnaryExpression;
class BinaryExpression;
class BetweenExpression;
class ColId;
struct Literal;
class Target;
// ---------------------------------------------------------------------------------------------------
struct Scope {
   /// Attributes
   std::unordered_multimap<std::string, const IU*> lookupIUs;

   void addIU(const IU*);
   const IU* findIU(const std::string& iuColumnName, const std::string* tupleVarName) const;
};
// ---------------------------------------------------------------------------------------------------
class SemanticAnalysis {
   private:
   Database& db;
   Scope scope;

   SemanticAnalysis(Database& database);

   std::unique_ptr<statement::Statement> analyseCreateTable(parser::CreateTable&);
   std::unique_ptr<statement::Statement> analyseCopyTable(parser::CopyTable&);
   std::unique_ptr<statement::Statement> analyseSelectStmt(parser::Select&);

   std::unique_ptr<statement::Statement> analyseSelect(parser::Select&);
   std::unique_ptr<algebra::Operator> analyseFrom(std::vector<parser::TableRef*>&);
   std::unique_ptr<algebra::Operator> analyseTableRef(parser::TableRef&);
   std::unique_ptr<algebra::Operator> analyseRelationExpr(parser::RelationExpr&, std::string* alias);
   std::pair<OrderedIUSet, bool> analyseTargetList(std::vector<Target*>&, const OrderedIUSet& allIUs);

   std::unique_ptr<Expression> analyseExpression(parser::AST&);
   std::unique_ptr<Expression> analyseUnaryExpression(parser::UnaryExpression&);
   std::unique_ptr<Expression> analyseBinaryExpression(parser::BinaryExpression&);
   std::unique_ptr<Expression> analyseBetweenExpression(parser::BetweenExpression&);
   std::unique_ptr<Expression> analyseColId(parser::ColId&);
   std::unique_ptr<Expression> analyseLiteral(parser::Literal&);

   static std::string extractName(parser::AST* ast);
   static const std::string* extractNamePtr(parser::AST* ast);

   public:
   static std::unique_ptr<statement::Statement> buildStatement(parser::AST& ast, Database& db);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::parser
// ---------------------------------------------------------------------------------------------------
