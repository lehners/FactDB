#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/IUSet.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
Selection::Selection(std::unique_ptr<Operator> child_, std::unique_ptr<Expression> predicates_)
   : UnaryOperator(OperatorType::Selection, std::move(child_)), predicate(std::move(predicates_)) {
}
// ---------------------------------------------------------------------------------------------------
void Selection::prepare(visitors::AlgebraVisitor& visitor, IUSet& requiredPipeline, Operator* consumerP, const IUSet& required) {
   consumer = consumerP;
   requiredPipeline = requiredPipeline.merge(predicate->collectIUs());
   visitor.visitPrepare(visitors::PrepareStage::BeforeCall, *this);
   child->prepare(visitor, requiredPipeline, this, required);
   visitor.visitPrepare(visitors::PrepareStage::AfterCall, *this);
}
// ---------------------------------------------------------------------------------------------------
void Selection::produce(visitors::AlgebraVisitor& visitor) const {
   visitor.visitProduce(*this);
}
// ---------------------------------------------------------------------------------------------------
void Selection::produce(visitors::AlgebraVisitor& visitor) {
   visitor.visitProduce(*this);
}
// ---------------------------------------------------------------------------------------------------
void Selection::consume(const visitors::AlgebraVisitor::ConsumeStage stage, visitors::AlgebraVisitor& visitor, const Operator* caller) const {
   visitor.visitConsume(stage, *this, caller);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
