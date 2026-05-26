#ifndef H_FACTDB_FACTDB_ALGEBRA_COUNT_HPP
#define H_FACTDB_FACTDB_ALGEBRA_COUNT_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/UnaryOperator.hpp"
#include "factDB/infra/IUSet.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
class Count : public UnaryOperator {
   protected:
   /// Required ius
   IUSet requiredIus;

   public:
   Count(std::unique_ptr<Operator> child_) : UnaryOperator(OperatorType::Count, std::move(child_)) {}

   void prepare(visitors::AlgebraVisitor& visitor, const IUSet& required, Operator* consumerP);
   void prepare(visitors::AlgebraVisitor& visitor, const OrderedIUSet& required, Operator* consumerP);
   void prepare(visitors::AlgebraVisitor& visitor, IUSet& requiredPipeline, Operator* consumer, const IUSet& required) override;

   void produce(visitors::AlgebraVisitor& visitor) const override;
   void produce(visitors::AlgebraVisitor& visitor) override;

   void consume(const visitors::ConsumeStage stage, visitors::AlgebraVisitor& visitor, const Operator* caller) const override;

   [[nodiscard]] inline const IUSet& getRequiredIus() const { return requiredIus; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_ALGEBRA_SELECTION_HPP