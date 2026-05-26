// ---------------------------------------------------------------------------------------------------
#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/Operator.hpp"
#include "factDB/infra/IUSet.hpp"
#include "factDB/statement/Statement.hpp"
#include <memory>
// ---------------------------------------------------------------------------------------------------
namespace factDB::statement {
// ---------------------------------------------------------------------------------------------------
class QueryStatement : public Statement {
   std::unique_ptr<algebra::Operator> tree;
   OrderedIUSet ius;

   public:
   QueryStatement(std::unique_ptr<algebra::Operator> tree_, OrderedIUSet ius_);
   ~QueryStatement() override = default;

   void runStatement(Database& db) override;

   static QueryStatement* dynCast(Statement* stmt) { return stmt->getType() == Statement::QueryStatement ? static_cast<QueryStatement*>(stmt) : nullptr; }
   static const QueryStatement* dynCast(const Statement* stmt) { return stmt->getType() == Statement::QueryStatement ? static_cast<const QueryStatement*>(stmt) : nullptr; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::statement
// ---------------------------------------------------------------------------------------------------