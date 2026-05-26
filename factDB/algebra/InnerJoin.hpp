#ifndef H_factdb_algebra_innerJoin
#define H_factdb_algebra_innerJoin
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/BinaryOperator.hpp"
#include "factDB/algebra/JoinMode.hpp"
#include "factDB/infra/IUSet.hpp"
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB { class JoinConditionList; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
class InnerJoin : public BinaryOperator {
   private:
   // The join condition
   std::unique_ptr<JoinConditionList> joinCondition;

   // IUs required in the current pipe
   IUSet parentPipelineRequiredPipeline;
   IUSet leftPipelineRequiredPipeline; // should be equivalent to HT key from left, but no order
   IUSet rightPipelineRequiredPipeline; // depends on join mode, since bottom insert is full pipeline breaker

   JoinType joinType = JoinType::Inner;
   JoinMode joinMode = JoinMode::TopInsert;

   public:
   InnerJoin(std::unique_ptr<Operator> leftChild, std::unique_ptr<Operator> rightChild, std::unique_ptr<JoinConditionList> joinCondition, JoinMode mode = JoinMode::BottomInsert);

   void prepare(visitors::AlgebraVisitor& visitor, IUSet& requiredPipeline, Operator* consumer, const IUSet& required) override;
   void produce(visitors::AlgebraVisitor& visitor) const override;
   void produce(visitors::AlgebraVisitor& visitor) override;
   void consume(const visitors::ConsumeStage, visitors::AlgebraVisitor& visitor, const Operator* caller) const override;

   [[nodiscard]] OrderedIUSet collectIUs() const override;

   [[nodiscard]] inline const JoinConditionList& getJoinCondition() const { return *joinCondition.get(); }
   [[nodiscard]] inline JoinConditionList& getJoinCondition() { return *joinCondition.get(); }

   [[nodiscard]] inline const IUSet& getParentRequiredPipeline() const { return parentPipelineRequiredPipeline; }
   [[nodiscard]] inline const IUSet& getLeftRequiredPipeline() const { return leftPipelineRequiredPipeline; }
   [[nodiscard]] inline const IUSet& getRightRequiredPipeline() const { return rightPipelineRequiredPipeline; }

   void setJoinMode(JoinMode mode) { joinMode = mode; }
   void setJoinType(JoinType type) { joinType = type; }
   [[nodiscard]] JoinMode getJoinMode() const { return joinMode; }
   [[nodiscard]] JoinType getJoinType() const { return joinType; }

   static InnerJoin* dynCast(Operator* op) { return dynCastTemplate<Operator::InnerJoin, InnerJoin>(op); }
   static const InnerJoin* dynCast(const Operator* op) { return dynCastTemplate<Operator::InnerJoin, const InnerJoin>(op); }

   static std::string_view toString(const JoinMode mode);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_algebra_innerJoin
