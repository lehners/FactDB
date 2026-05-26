#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/algebra/visitors/PipelineStorage.hpp"
#include "factDB/newftree/FTree.hpp"
#include <list>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
class FactorizedTreeDeriver : public AlgebraVisitor {
   PipelineStore<FTree*> ftStorage;
   std::list<FTree> ownedTrees;
   const Operator* topmost_operator = nullptr;

   public:
   explicit FactorizedTreeDeriver() {};

   void visitPrepare(const Stage s, const TableScan&) override;
   void visitPrepare(const Stage s, const InnerJoin&) override;
   void visitPrepare(const Stage, const Print&) override;
   void visitPrepare(const Stage s, const Count&) override;

   void visitProduce(const InnerJoin&) override { __builtin_unreachable(); }
   void visitProduce(const Print&) override { __builtin_unreachable(); }
   void visitProduce(const Selection&) override { __builtin_unreachable(); }
   void visitProduce(const TableScan&) override { __builtin_unreachable(); }
   void visitProduce(const Count&) override { __builtin_unreachable(); }
   void visitProduce(const CrossProduct&) override { __builtin_unreachable(); }

   const FTree& getFTree(const Operator& op) const;
   void genRootTrees(fw::FileWriter& out);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
