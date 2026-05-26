#ifndef H_FACTDB_FACTDB_ALGEBRA_PRINT_HPP
#define H_FACTDB_FACTDB_ALGEBRA_PRINT_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/UnaryOperator.hpp"
#include "factDB/infra/IUSet.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
class Print : public UnaryOperator {
   private:
   // Required ius
   OrderedIUSet requiredIus;

   // The seperator
   std::string seperator;

   public:
   Print(std::unique_ptr<Operator> child);
   Print(std::unique_ptr<Operator> child, std::string seperator);

   void prepare(visitors::AlgebraVisitor& visitor, const OrderedIUSet& required, Operator* consumer);
   void prepare(visitors::AlgebraVisitor& visitor, IUSet& requiredPipeline, Operator* consumer, const IUSet& required) override;

   void produce(visitors::AlgebraVisitor& visitor) const override;
   void produce(visitors::AlgebraVisitor& visitor) override;

   void consume(const visitors::ConsumeStage stage, visitors::AlgebraVisitor& visitor, const Operator* caller) const override;

   [[nodiscard]] inline const std::string& getSeperator() const { return seperator; }
   [[nodiscard]] inline const OrderedIUSet& getRequiredIus() const { return requiredIus; }

   static Print* dynCast(Operator* op) { return dynCastTemplate<Operator::Print, Print>(op); }
   static const Print* dynCast(const Operator* op) { return dynCastTemplate<Operator::Print, const Print>(op); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_ALGEBRA_PRINT_HPP
