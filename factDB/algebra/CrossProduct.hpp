#ifndef H_factdb_algebra_crossProduct
#define H_factdb_algebra_crossProduct
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/BinaryOperator.hpp"
#include "factDB/infra/IUSet.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
class CrossProduct : public BinaryOperator {
   /// Implements the cross product operator, currently only for parsing & optimizer, should no more exist in codegen
   public:
   CrossProduct(std::unique_ptr<Operator> leftChild_, std::unique_ptr<Operator> rightChild_);

   void prepare(visitors::AlgebraVisitor& visitor, IUSet& requiredPipeline, Operator* consumer, const IUSet& required) override;
   void produce(visitors::AlgebraVisitor& visitor) const override;
   void produce(visitors::AlgebraVisitor& visitor) override;
   void consume(const visitors::ConsumeStage, visitors::AlgebraVisitor& visitor, const Operator* caller) const override;

   public:
   static auto* dynCast(Operator* op) { return dynCastTemplate<Operator::CrossProduct, CrossProduct>(op); }
   static const auto* dynCast(const Operator* op) { return dynCastTemplate<Operator::CrossProduct, const CrossProduct>(op); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_algebra_crossProduct
