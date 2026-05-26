// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/FlattenJoins.hpp"
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
void FlattenJoins::visitProduce(algebra::Selection& selection) {
   setConsumer(*selection.getChild(), &selection);
   parentOperatorPtr = &selection.getChild();
   selection.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FlattenJoins::visitProduce(algebra::TableScan&) {
   // done, nothing to do
}
// ---------------------------------------------------------------------------------------------------
void FlattenJoins::visitProduce(algebra::Print& print) {
   setConsumer(*print.getChild(), &print);
   parentOperatorPtr = &print.getChild();
   print.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FlattenJoins::visitProduce(algebra::Count& count) {
   setConsumer(*count.getChild(), &count);
   parentOperatorPtr = &count.getChild();
   count.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FlattenJoins::visitProduce(factDB::algebra::InnerJoin& join) {
   if (join.getJoinMode() == algebra::JoinMode::BottomInsert) {
      auto* oldJoin = static_cast<algebra::InnerJoin*>(parentOperatorPtr->get());
      oldJoin->setJoinMode(algebra::JoinMode::TopInsert);
   }

   setConsumer(*join.getLeftChild(), &join);
   parentOperatorPtr = &join.getLeftChild();
   join.getLeftChild()->produce(*this);

   setConsumer(*join.getRightChild(), &join);
   parentOperatorPtr = &join.getRightChild();
   join.getRightChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FlattenJoins::visitProduce(algebra::CrossProduct& crossProduct) {
   setConsumer(*crossProduct.getLeftChild(), &crossProduct);
   parentOperatorPtr = &crossProduct.getLeftChild();
   crossProduct.getLeftChild()->produce(*this);

   setConsumer(*crossProduct.getRightChild(), &crossProduct);
   parentOperatorPtr = &crossProduct.getRightChild();
   crossProduct.getRightChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FlattenJoins::apply(algebra::Operator& op) {
   FlattenJoins ppd;
   op.produce(ppd);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------