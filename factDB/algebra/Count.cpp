// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/infra/Config.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
void Count::prepare(visitors::AlgebraVisitor& visitor, const factDB::OrderedIUSet& required, factDB::algebra::Operator* consumerP) {
   IUSet requiredDupFree(required.begin(), required.end());
   prepare(visitor, requiredDupFree, consumerP);
}
// ---------------------------------------------------------------------------------------------------
void Count::prepare(visitors::AlgebraVisitor& visitor, const IUSet&, [[maybe_unused]] Operator* consumerP) {
   requiredIus = {};
   assert(requiredIus.empty());
   assert(consumerP == nullptr && "Print cannot have a consumer");
   consumer = consumerP;
   visitor.visitPrepare(visitors::PrepareStage::BeforeCall, *this);
   IUSet requiredPipeline = {};
   child->prepare(visitor, requiredPipeline, this, requiredIus);
   visitor.visitPrepare(visitors::PrepareStage::AfterCall, *this);
}
// ---------------------------------------------------------------------------------------------------
void Count::prepare(visitors::AlgebraVisitor&, IUSet&, [[maybe_unused]] Operator* consumerP, const IUSet&) {
   assert(consumerP == nullptr && "Print cannot have a consumer");
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
void Count::produce(visitors::AlgebraVisitor& visitor) const {
   visitor.visitProduce(*this);
}
// ---------------------------------------------------------------------------------------------------
void Count::produce(visitors::AlgebraVisitor& visitor) {
   visitor.visitProduce(*this);
}
// ---------------------------------------------------------------------------------------------------
void Count::consume(const visitors::AlgebraVisitor::ConsumeStage stage, visitors::AlgebraVisitor& visitor, const Operator* caller) const {
   visitor.visitConsume(stage, *this, caller);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
