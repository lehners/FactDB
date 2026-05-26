#ifndef H_FACTDB_FACTDB_ALGEBRA_SELECTION_HPP
#define H_FACTDB_FACTDB_ALGEBRA_SELECTION_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/UnaryOperator.hpp"
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB { class Expression; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class PredicatePushDown;
class CreateJoins;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
class Selection : public UnaryOperator {
   friend class factDB::opt::PredicatePushDown;
   friend class factDB::opt::CreateJoins;

   protected:
   // Predicates
   std::unique_ptr<Expression> predicate;

   public:
   Selection(std::unique_ptr<Operator> childP, std::unique_ptr<Expression> predicatesP);

   void prepare(visitors::AlgebraVisitor& visitor, IUSet& requiredPipeline, Operator* consumer, const IUSet& required) override;

   void produce(visitors::AlgebraVisitor& visitor) const override;
   void produce(visitors::AlgebraVisitor& visitor) override;

   void consume(const visitors::ConsumeStage stage, visitors::AlgebraVisitor& visitor, const Operator* caller) const override;

   [[nodiscard]] inline const Expression& getPredicate() const { return *predicate.get(); }

   static auto* dynCast(Operator* ptr) { return dynCastTemplate<Operator::Selection, Selection>(ptr); }
   static const auto* dynCast(const Operator* ptr) { return dynCastTemplate<Operator::Selection, const Selection>(ptr); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_ALGEBRA_SELECTION_HPP