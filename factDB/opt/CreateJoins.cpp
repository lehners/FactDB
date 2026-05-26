// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/CreateJoins.hpp"
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
std::tuple<ExpressionPtr, std::unique_ptr<JoinConditionList>> genJoinConditionList(ExpressionPtr expr_) {
   std::list<ExpressionPtr> todos, terms;
   std::vector<JoinCondition> joinConditions;
   todos.emplace_back(std::move(expr_));
   while (!todos.empty()) {
      auto expr = std::move(todos.back());
      todos.pop_back();

      if (AndExpression* andExpr = AndExpression::dynCast(expr.get()); andExpr != nullptr) {
         todos.push_back(andExpr->releaseLeft());
         todos.push_back(andExpr->releaseRight());
      } else if (CompareEqual* equality = CompareEqual::dynCast(expr.get()); equality != nullptr) {
         auto left = IURef::dynCast(&equality->get_left());
         auto right = IURef::dynCast(&equality->get_right());

         if (left != nullptr && right != nullptr) { // compare two IUs -> can be used in join
            joinConditions.emplace_back(std::move(*left), std::move(*right));
         } else { // other comaprison, cannot be merged
            terms.emplace_back(std::move(expr));
         }
      } else {
         terms.emplace_back(std::move(expr));
      }
   }

   assert(!terms.empty() || !joinConditions.empty());

   ExpressionPtr termExpr;
   for (auto& term : terms) {
      termExpr = termExpr ? std::make_unique<factDB::AndExpression>(std::move(termExpr), std::move(term)) : std::move(term);
   }
   terms.clear();

   auto jcl = !joinConditions.empty() ? std::make_unique<JoinConditionList>(std::move(joinConditions)) : nullptr;
   return {std::move(termExpr), std::move(jcl)};
}
// ---------------------------------------------------------------------------------------------------
bool CreateJoins::mergeSelectionWithBinaryOperator(algebra::Selection& selection, std::unique_ptr<algebra::Operator>& leftChild, std::unique_ptr<algebra::Operator>& rightChild, JoinConditionList* existingJoinConditions) {
   assert(selection.predicate); // selection predicate is not empty
   auto [selectionPredicate, joinPredicate] = genJoinConditionList(std::move(selection.predicate));

   if (joinPredicate) {
      if (existingJoinConditions != nullptr)
         joinPredicate->insert(*existingJoinConditions);
      selection.child = std::make_unique<algebra::InnerJoin>(std::move(leftChild), std::move(rightChild), std::move(joinPredicate));
      auto& join = static_cast<algebra::InnerJoin&>(*selection.getChild());
      setConsumer(*selection.child, &selection);
      setConsumer(*join.getLeftChild(), &join);
      setConsumer(*join.getRightChild(), &join);
   } else {
      assert(selectionPredicate); // everything must stay in selection predicate
   }

   if (selectionPredicate) { // update filter predicate
      selection.predicate = std::move(selectionPredicate);
      return false;
   } else { // drop empty filter
      setConsumer(*selection.getChild(), selection.getConsumer());
      *parentOperatorPtr = std::move(selection.getChild());
      return true;
   }
}
// ---------------------------------------------------------------------------------------------------
void CreateJoins::visitProduce(algebra::Selection& selection) {
   setConsumer(*selection.getChild(), &selection);

   switch (selection.getChild()->getType()) {
      case algebra::Operator::CrossProduct: {
         auto& crossProd = *algebra::CrossProduct::dynCast(selection.getChild().get());
         bool droppedSelection = mergeSelectionWithBinaryOperator(selection, crossProd.getLeftChild(), crossProd.getRightChild(), nullptr);
         if (!droppedSelection) {
            parentOperatorPtr = &selection.getChild();
            selection.getChild()->produce(*this);
         } else {
            parentOperatorPtr->get()->produce(*this);
         }
      } break;
      case algebra::Operator::InnerJoin: {
         auto& innerJoin = *algebra::InnerJoin::dynCast(selection.getChild().get());
         bool droppedSelection = mergeSelectionWithBinaryOperator(selection, innerJoin.getLeftChild(), innerJoin.getRightChild(), &innerJoin.getJoinCondition());
         if (!droppedSelection) {
            parentOperatorPtr = &selection.getChild();
            selection.getChild()->produce(*this);
         } else {
            parentOperatorPtr->get()->produce(*this);
         }
      } break;
      default:
         break;
   }
}
// ---------------------------------------------------------------------------------------------------
void CreateJoins::visitProduce(algebra::TableScan&) {
   // done, nothing to push down
}
// ---------------------------------------------------------------------------------------------------
void CreateJoins::visitProduce(algebra::Print& print) {
   setConsumer(*print.getChild(), &print);
   parentOperatorPtr = &print.getChild();
   print.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void CreateJoins::visitProduce(algebra::Count& count) {
   setConsumer(*count.getChild(), &count);
   parentOperatorPtr = &count.getChild();
   count.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void CreateJoins::visitProduce(factDB::algebra::InnerJoin& join) {
   setConsumer(*join.getLeftChild(), &join);
   parentOperatorPtr = &join.getLeftChild();
   join.getLeftChild()->produce(*this);

   setConsumer(*join.getRightChild(), &join);
   parentOperatorPtr = &join.getRightChild();
   join.getRightChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void CreateJoins::visitProduce(algebra::CrossProduct& crossProduct) {
   setConsumer(*crossProduct.getLeftChild(), &crossProduct);
   parentOperatorPtr = &crossProduct.getLeftChild();
   crossProduct.getLeftChild()->produce(*this);

   setConsumer(*crossProduct.getRightChild(), &crossProduct);
   parentOperatorPtr = &crossProduct.getRightChild();
   crossProduct.getRightChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void CreateJoins::apply(algebra::Operator& op) {
   CreateJoins ppd;
   op.produce(ppd);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------