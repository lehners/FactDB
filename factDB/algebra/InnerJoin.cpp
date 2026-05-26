// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/Setting.hpp"
#include "fmt/format.h"
#include <algorithm>
#include <cassert>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<bool> checkCrossProduct("algebra.join.checkCrossProduct", true);
// ---------------------------------------------------------------------------------------------------
size_t Operator::globalOperatorId = 0;
// ---------------------------------------------------------------------------------------------------
InnerJoin::InnerJoin(std::unique_ptr<Operator> leftChild_, std::unique_ptr<Operator> rightChild_, std::unique_ptr<JoinConditionList> joinCondition_, JoinMode mode)
   : BinaryOperator(OperatorType::InnerJoin, std::move(leftChild_), std::move(rightChild_)), joinCondition(std::move(joinCondition_)), joinMode(mode) {
   auto leftIus = leftChild->collectIUs();
   auto rightIus = rightChild->collectIUs();

   // currently only and with IURefs is supported
   assert((!checkCrossProduct.get() || !joinCondition->get_conditions().empty()) && "You wanna execute a join without a join condition, i.e. a cross product.");

   for (JoinCondition& condition : joinCondition->get_conditions()) {
      // check that each iu is either from left or right side and not from both sides.
      assert(leftIus.contains(condition.get_left()) != rightIus.contains(condition.get_left()));
      assert(leftIus.contains(condition.get_right()) != rightIus.contains(condition.get_right()));
      // check that only one of iu1 or iu2 is from the left side. -> other iu is from other side
      assert(leftIus.contains(condition.get_left()) != leftIus.contains(condition.get_right()));

      // swap all conditions such that the iu from the left table is first
      if (leftIus.contains(condition.get_right())) condition.swap();
   }
}
// ---------------------------------------------------------------------------------------------------
OrderedIUSet InnerJoin::collectIUs() const {
   auto leftIus = leftChild->collectIUs();
   auto rightIus = rightChild->collectIUs();
   switch (joinType) {
      case JoinType::Inner: return leftIus.merge(rightIus);
      case JoinType::RightSemi: return rightIus;
      default: unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
void InnerJoin::prepare(visitors::AlgebraVisitor& visitor, IUSet& requiredPipeline, Operator* consumerP, const IUSet& required) {
   consumer = consumerP;

   IUSet requiredForJoin = joinCondition->collectIUs();

   parentPipelineRequired = required;
   parentPipelineRequiredPipeline = requiredPipeline;

   auto leftIUs = leftChild->collectIUs();
   leftPipelineRequiredPipeline = requiredForJoin.intersect(leftIUs);
   leftPipelineRequired = required.intersect(leftIUs) // all required ius in parent
                             .merge(requiredPipeline.intersect(leftIUs)) // all in parent pipe required from left
                             .merge(leftPipelineRequiredPipeline); // all required for join from left

   rightPipelineRequired = required.difference(leftIUs)
                              .merge(requiredPipeline.difference(leftIUs))
                              .merge(rightPipelineRequiredPipeline);
   switch (getJoinMode()) {
      case JoinMode::BottomInsert:
         rightPipelineRequiredPipeline = requiredForJoin.difference(leftIUs);
         break;
      case JoinMode::TopInsert:
         rightPipelineRequiredPipeline = requiredForJoin.difference(leftIUs).merge(parentPipelineRequiredPipeline.difference(leftIUs));
         break;
      default:
         unreachable();
   }

   visitor.visitPrepare(visitors::PrepareStage::BeforeCall, *this);
   leftChild->prepare(visitor, leftPipelineRequiredPipeline, this, leftPipelineRequired);
   rightChild->prepare(visitor, rightPipelineRequiredPipeline, this, rightPipelineRequired);
   visitor.visitPrepare(visitors::PrepareStage::AfterCall, *this);
}
// ---------------------------------------------------------------------------------------------------
void InnerJoin::produce(visitors::AlgebraVisitor& visitor) const {
   visitor.visitProduce(*this);
}
// ---------------------------------------------------------------------------------------------------
void InnerJoin::produce(visitors::AlgebraVisitor& visitor) {
   visitor.visitProduce(*this);
}
// ---------------------------------------------------------------------------------------------------
void InnerJoin::consume(const visitors::AlgebraVisitor::ConsumeStage stage, visitors::AlgebraVisitor& visitor, const Operator* caller) const {
   visitor.visitConsume(stage, *this, caller);
}
// ---------------------------------------------------------------------------------------------------
std::string_view InnerJoin::toString(const JoinMode mode) {
   switch (mode) {
      case JoinMode::TopInsert: return "TopInsert";
      case JoinMode::BottomInsert: return "BottomInsert";
      default: unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------