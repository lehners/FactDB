// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/CrossProduct.hpp"
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/infra/Expression.hpp"
#include "fmt/format.h"
#include <algorithm>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
CrossProduct::CrossProduct(std::unique_ptr<Operator> leftChild_, std::unique_ptr<Operator> rightChild_)
   : BinaryOperator(OperatorType::CrossProduct, std::move(leftChild_), std::move(rightChild_)) {
   auto leftIus = leftChild->collectIUs();
   auto rightIus = rightChild->collectIUs();
}
// ---------------------------------------------------------------------------------------------------
void CrossProduct::prepare(visitors::AlgebraVisitor& visitor, IUSet& requiredPipeline, Operator* consumerP, const IUSet& required) {
   consumer = consumerP;

   parentPipelineRequired = required;

   auto leftIUs = leftChild->collectIUs();
   leftPipelineRequired = required.intersect(leftIUs) // all required ius in parent
                             .merge(requiredPipeline.intersect(leftIUs)); // all in parent pipe required from left

   rightPipelineRequired = required.difference(leftIUs);

   visitor.visitPrepare(visitors::PrepareStage::BeforeCall, *this);
   leftChild->prepare(visitor, leftPipelineRequired, this, leftPipelineRequired);
   rightChild->prepare(visitor, rightPipelineRequired, this, rightPipelineRequired);
   visitor.visitPrepare(visitors::PrepareStage::AfterCall, *this);
}
// ---------------------------------------------------------------------------------------------------
void CrossProduct::produce(visitors::AlgebraVisitor& visitor) const {
   visitor.visitProduce(*this);
}
// ---------------------------------------------------------------------------------------------------
void CrossProduct::produce(visitors::AlgebraVisitor& visitor) {
   visitor.visitProduce(*this);
}
// ---------------------------------------------------------------------------------------------------
void CrossProduct::consume(const visitors::AlgebraVisitor::ConsumeStage stage, visitors::AlgebraVisitor& visitor, const Operator* caller) const {
   visitor.visitConsume(stage, *this, caller);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------