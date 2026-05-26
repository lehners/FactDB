#ifndef H_FACTDB_FACTDB_ALGEBRA_UNARY_OPERATOR_HPP
#define H_FACTDB_FACTDB_ALGEBRA_UNARY_OPERATOR_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/Operator.hpp"
#include "factDB/infra/Expression.hpp"
#include <memory>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
class UnaryOperator : public Operator {
   protected:
   /// Child operator
   std::unique_ptr<Operator> child;

   public:
   UnaryOperator(OperatorType otype_, std::unique_ptr<Operator> childP) : Operator(otype_), child(std::move(childP)){};

   [[nodiscard]] inline const std::unique_ptr<Operator>& getChild() const { return child; }
   [[nodiscard]] inline std::unique_ptr<Operator>& getChild() { return child; }

   [[nodiscard]] OrderedIUSet collectIUs() const override;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_ALGEBRA_UNARY_OPERATOR_HPP