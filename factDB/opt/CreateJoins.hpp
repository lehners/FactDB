#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include <cassert>
#include <memory>
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB { class JoinConditionList; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class CreateJoins : public algebra::visitors::AlgebraVisitor {
   private:
   std::unique_ptr<algebra::Operator>* parentOperatorPtr = nullptr;

   bool mergeSelectionWithBinaryOperator(algebra::Selection& selection, std::unique_ptr<algebra::Operator>& leftChild, std::unique_ptr<algebra::Operator>& rightChild, JoinConditionList* joinConditions);

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