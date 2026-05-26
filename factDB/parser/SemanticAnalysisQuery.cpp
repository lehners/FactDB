// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/CrossProduct.hpp"
#include "factDB/algebra/Operator.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/IUSet.hpp"
#include "factDB/infra/RuntimeException.hpp"
#include "factDB/parser/AST.hpp"
#include "factDB/parser/SemanticAnalysis.hpp"
#include "factDB/statement/QueryStatement.hpp"
#include "factDB/statement/Statement.hpp"
#include "fmt/format.h"
// ---------------------------------------------------------------------------------------------------
namespace factDB::parser {
// ---------------------------------------------------------------------------------------------------
SemanticAnalysis::SemanticAnalysis(factDB::Database& database) : db(database) {}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<algebra::Operator> SemanticAnalysis::analyseRelationExpr(parser::RelationExpr& input, std::string* alias) {
   auto tableName = extractName(input.getInput());
   if (!db.containsRelation(tableName))
      throw RuntimeException(RuntimeError, fmt::format("table {} not known", tableName));

   std::unique_ptr<algebra::Operator> tree;
   if (alias == nullptr)
      tree = std::make_unique<algebra::TableScan>(db, db.getRelationID(tableName));
   else
      tree = std::make_unique<algebra::TableScan>(db, db.getRelationID(tableName), std::move(*alias));

   for (auto& iu : tree->collectIUs()) {
      scope.addIU(iu);
   }
   return tree;
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<algebra::Operator> SemanticAnalysis::analyseTableRef(parser::TableRef& tableRef) {
   auto alias = Alias::dynCast(tableRef.getAlias());
   auto aliasName = alias ? std::string(alias->getAliasValue()) : "";

   std::unique_ptr<algebra::Operator> op;
   switch (tableRef.getSubType()) {
      case TableRef::SubType::Relation:
         op = analyseRelationExpr(*RelationExpr::dynCast(tableRef.getInput()), alias ? &aliasName : nullptr);
         break;
      case TableRef::SubType::Joined:
      case TableRef::SubType::Subselect:
         not_implemented();
   }

   return op;
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<algebra::Operator> SemanticAnalysis::analyseFrom(std::vector<parser::TableRef*>& tableRefs) {
   std::unique_ptr<algebra::Operator> tree;

   for (auto* table : tableRefs) {
      assert(table != nullptr && "dynCast for TableRef probably failed");
      auto tableOperator = analyseTableRef(*table);
      if (!tree)
         tree = std::move(tableOperator);
      else
         tree = std::make_unique<algebra::CrossProduct>(std::move(tree), std::move(tableOperator));
   }

   return tree;
}
// ---------------------------------------------------------------------------------------------------
std::pair<OrderedIUSet, bool> SemanticAnalysis::analyseTargetList(std::vector<Target*>& targets, const OrderedIUSet& allIUs) {
   OrderedIUSet output;
   bool isCountStar = false;
   for (auto& target : targets) {
      if (!target->getExpression()) { // select *
         output.merge(allIUs);
      } else if (auto* countStar = CountStar::dynCast(target->getExpression()); countStar != nullptr) {
         isCountStar = true;
      } else {
         auto expr = analyseExpression(*target->getExpression());
         // ignore targets alias for now & only use IURefs
         if (expr->get_type() != Expression::IURef)
            throw RuntimeException(RuntimeError, "Selecting other expressions than IURef not implemented yet");
         output.push_back(&static_cast<IURef*>(expr.get())->get_iu());
      }
   }
   if (isCountStar && !output.empty())
      throw RuntimeException(RuntimeError, "Cannot aggregate and output other IUs for now.");
   return {output, isCountStar};
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<statement::Statement> SemanticAnalysis::analyseSelect(parser::Select& select) {
   if (select.getFrom().empty())
      throw RuntimeException(RuntimeError, "Currently empty FROM clause is not allowed");
   auto fromList = select.getFrom();
   auto tree = analyseFrom(fromList);

   if (select.getWhere()) {
      auto expr = analyseExpression(*select.getWhere());
      tree = std::make_unique<algebra::Selection>(std::move(tree), std::move(expr));
   }

   auto targets = select.getTargets();
   auto [outputIUs, countStar] = analyseTargetList(targets, tree->collectIUs());

   if (countStar) {
      tree = std::make_unique<algebra::Count>(std::move(tree));
   } else {
      tree = std::make_unique<algebra::Print>(std::move(tree));
   }

   return std::make_unique<statement::QueryStatement>(std::move(tree), std::move(outputIUs));
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::parser
// ---------------------------------------------------------------------------------------------------