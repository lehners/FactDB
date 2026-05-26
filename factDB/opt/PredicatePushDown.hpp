#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include <cassert>
#include <memory>
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class PredicatePushDown : public algebra::visitors::AlgebraVisitor {
   private:
   std::unique_ptr<algebra::Operator>* parentOperatorPtr = nullptr;

   bool pushDownBinaryOperator(algebra::Selection& selection, std::unique_ptr<algebra::Operator>& childsLeftOperator, std::unique_ptr<algebra::Operator>& childsRightOperator);

   public:
   void visitProduce(const algebra::InnerJoin&) override { assert(false); }
   void visitProduce(const algebra::Print&) override { assert(false); }
   void visitProduce(const algebra::Selection&) override { assert(false); }
   void visitProduce(const algebra::TableScan&) override { assert(false); }
   void visitProduce(const algebra::Count&) override { assert(false); }
   void visitProduce(const algebra::CrossProduct&) override { assert(false); }

   void visitProduce(algebra::InnerJoin& join) override;
   void visitProduce(algebra::Print& print) override;
   void visitProduce(algebra::Selection& selection) override;
   void visitProduce(algebra::TableScan& tableScan) override;
   void visitProduce(algebra::Count& count) override;
   void visitProduce(algebra::CrossProduct& crossProduct) override;

   static void apply(algebra::Operator& op);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------