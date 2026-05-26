// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/PredicatePushDown.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/CrossProduct.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Expression.hpp"
#include <list>
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
bool PredicatePushDown::pushDownBinaryOperator(algebra::Selection& selection, std::unique_ptr<algebra::Operator>& childsLeftOperator, std::unique_ptr<algebra::Operator>& childsRightOperator) {
   // split selection at AND predicates
   std::list<ExpressionPtr> todos, terms;
   todos.emplace_back(std::move(selection.predicate));
   while (!todos.empty()) {
      auto expr = std::move(todos.back());
      todos.pop_back();

      if (AndExpression* andExpr = AndExpression::dynCast(expr.get()); andExpr != nullptr) {
         todos.push_back(andExpr->releaseLeft());
         todos.push_back(andExpr->releaseRight());
      } else {
         terms.emplace_back(std::move(expr));
      }
   }

   auto leftChildIUs = childsLeftOperator->collectIUs();
   auto rightChildIUs = childsRightOperator->collectIUs();
   assert(leftChildIUs.intersect(rightChildIUs).empty());

   // create the pushed down expressions
   ExpressionPtr left, right, parent;
   for (auto& expr : terms) {
      auto requiredIUs = expr->collectIUs();

      if (rightChildIUs.intersect(requiredIUs).empty()) { // all required IUs are from left side
         left = left ? AndExpression::create(std::move(left), std::move(expr)) : std::move(expr);
      } else if (leftChildIUs.intersect(requiredIUs).empty()) { // all required IUs are from left side
         right = right ? AndExpression::create(std::move(right), std::move(expr)) : std::move(expr);
      } else {
         parent = parent ? AndExpression::create(std::move(parent), std::move(expr)) : std::move(expr);
      }
   }

   // terms does no longer contain any expressions
   terms.clear();

   bool removedSelection = parent == nullptr;
   if (parent) {
      selection.predicate = std::move(parent);
   } else { // selection can be dropped
      setConsumer(*selection.getChild(), selection.getConsumer());
      *parentOperatorPtr = std::move(selection.child);
   }
   // selection is destroyed from here

   if (left) { // create selection as left child of childsLeftOperator
      childsLeftOperator = std::make_unique<algebra::Selection>(std::move(childsLeftOperator), std::move(left));
      auto& selectionOperator = static_cast<algebra::Selection&>(*childsLeftOperator);
      setConsumer(selectionOperator, selectionOperator.getChild()->getConsumer());
      setConsumer(*selectionOperator.getChild(), &selectionOperator);
   }

   if (right) { // create selection as right child of childsRightOperator
      childsRightOperator = std::make_unique<algebra::Selection>(std::move(childsRightOperator), std::move(right));
      auto& selectionOperator = static_cast<algebra::Selection&>(*childsRightOperator);
      setConsumer(selectionOperator, selectionOperator.getChild()->getConsumer());
      setConsumer(*selectionOperator.getChild(), &selectionOperator);
   }
   return removedSelection;
}
// ---------------------------------------------------------------------------------------------------
void PredicatePushDown::visitProduce(algebra::Selection& selection) {
   setConsumer(*selection.getChild(), &selection);
   switch (selection.getChild()->getType()) {
      case algebra::Operator::Print:
      case algebra::Operator::Count:
         unreachable();
      case algebra::Operator::Selection: {
         // merge this selection with selection below
         auto& childSelection = *algebra::Selection::dynCast(selection.getChild().get());
         childSelection.predicate = AndExpression::create(std::move(childSelection.predicate), std::move(selection.predicate));
         // drop this selection operator
         auto parent = selection.getConsumer();
         assert(parent != nullptr && "Expected parent for Selection");
         setConsumer(childSelection, parent);
         selection = std::move(childSelection); // override this selection with the child selection
         selection.getChild()->produce(*this);
      } break;
      case algebra::Operator::InnerJoin: {
         algebra::InnerJoin& join = *algebra::InnerJoin::dynCast(selection.getChild().get());
         auto removedSelection = pushDownBinaryOperator(selection, join.getLeftChild(), join.getRightChild());
         if (!removedSelection) {
            parentOperatorPtr = &selection.getChild();
            selection.produce(*this);
         } else {
            parentOperatorPtr->get()->produce(*this);
         }
      } break;
      case algebra::Operator::CrossProduct: {
         algebra::CrossProduct& crossProduct = *algebra::CrossProduct::dynCast(selection.getChild().get());
         auto removedSelection = pushDownBinaryOperator(selection, crossProduct.getLeftChild(), crossProduct.getRightChild());
         if (!removedSelection) {
            parentOperatorPtr = &selection.getChild();
            selection.getChild()->produce(*this);
         } else {
            (*parentOperatorPtr)->produce(*this);
         }
      } break;
      case algebra::Operator::Reference:
         not_implemented();
      case algebra::Operator::TableScan:
         // nothing to do here since this is already the last operator
         break;
   }
}
// ---------------------------------------------------------------------------------------------------
void PredicatePushDown::visitProduce(algebra::TableScan&) {
   // done, nothing to push down
}
// ---------------------------------------------------------------------------------------------------
void PredicatePushDown::visitProduce(algebra::Print& print) {
   setConsumer(*print.getChild(), &print);
   parentOperatorPtr = &print.getChild();
   print.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void PredicatePushDown::visitProduce(algebra::Count& count) {
   setConsumer(*count.getChild(), &count);
   parentOperatorPtr = &count.getChild();
   count.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void PredicatePushDown::visitProduce(factDB::algebra::InnerJoin& join) {
   setConsumer(*join.getLeftChild(), &join);
   parentOperatorPtr = &join.getLeftChild();
   join.getLeftChild()->produce(*this);

   setConsumer(*join.getRightChild(), &join);
   parentOperatorPtr = &join.getRightChild();
   join.getRightChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void PredicatePushDown::visitProduce(algebra::CrossProduct& crossProduct) {
   setConsumer(*crossProduct.getLeftChild(), &crossProduct);
   parentOperatorPtr = &crossProduct.getLeftChild();
   crossProduct.getLeftChild()->produce(*this);

   setConsumer(*crossProduct.getRightChild(), &crossProduct);
   parentOperatorPtr = &crossProduct.getRightChild();
   crossProduct.getRightChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void PredicatePushDown::apply(algebra::Operator& op) {
   PredicatePushDown ppd;
   op.produce(ppd);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------