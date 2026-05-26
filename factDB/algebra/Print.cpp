#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Setting.hpp"
#include <algorithm>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<std::string> algebra_delimiter("algebra.delimiter", "\t");
// ---------------------------------------------------------------------------------------------------
Print::Print(std::unique_ptr<Operator> child_) : Print(std::move(child_), algebra_delimiter.get()) {
}
// ---------------------------------------------------------------------------------------------------
Print::Print(std::unique_ptr<Operator> child_, std::string seperator_)
   : UnaryOperator(OperatorType::Print, std::move(child_)), seperator(std::move(seperator_)) {
}
// ---------------------------------------------------------------------------------------------------
void Print::prepare(visitors::AlgebraVisitor& visitor, const OrderedIUSet& required, [[maybe_unused]] Operator* consumerP) {
   requiredIus = required;
   assert(consumerP == nullptr && "Print cannot have a consumer");

   IUSet requiredDupFree(required.begin(), required.end());
   IUSet requiredDupFreePipeline = requiredDupFree;
   visitor.visitPrepare(visitors::PrepareStage::BeforeCall, *this);
   child->prepare(visitor, requiredDupFreePipeline, this, requiredDupFree);
   visitor.visitPrepare(visitors::PrepareStage::AfterCall, *this);
}
// ---------------------------------------------------------------------------------------------------
void Print::prepare(visitors::AlgebraVisitor&, IUSet&, [[maybe_unused]] Operator* consumerP, const IUSet&) {
   assert(consumerP == nullptr && "Print cannot have a consumer");
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
void Print::produce(visitors::AlgebraVisitor& visitor) const {
   visitor.visitProduce(*this);
}
// ---------------------------------------------------------------------------------------------------
void Print::produce(visitors::AlgebraVisitor& visitor) {
   visitor.visitProduce(*this);
}
// ---------------------------------------------------------------------------------------------------
void Print::consume(const visitors::AlgebraVisitor::ConsumeStage stage, visitors::AlgebraVisitor& visitor, const Operator* caller) const {
   visitor.visitConsume(stage, *this, caller);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------