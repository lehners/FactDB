#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/Operator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
class Reference : public Operator {
   protected:
   /// Child operator
   Operator& child;

   public:
   Reference(Operator& childP) : Operator(algebra::Operator::Reference), child(childP){};

   [[nodiscard]] inline const Operator& getChild() const { return child; }
   [[nodiscard]] inline Operator& getChild() { return child; }

   [[nodiscard]] OrderedIUSet collectIUs() const override;

   void prepare(visitors::AlgebraVisitor& visitor, IUSet& requiredPipeline, Operator* consumer, const IUSet& required) override;

   void produce(visitors::AlgebraVisitor& visitor) const override;
   void produce(visitors::AlgebraVisitor& visitor) override;

   void consume(const visitors::ConsumeStage stage, visitors::AlgebraVisitor& visitor, const Operator* caller) const override;

   static auto* dynCast(Operator* ptr) { return dynCastTemplate<Operator::Reference, Reference>(ptr); }
   static const auto* dynCast(const Operator* ptr) { return dynCastTemplate<Operator::Reference, const Reference>(ptr); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
