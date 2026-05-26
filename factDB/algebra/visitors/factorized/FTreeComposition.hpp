#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/algebra/visitors/factorized/FactorizedTreeDeriver.hpp"
#include "factDB/queryc/writer/FileWriter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
class FTreeCompositionVisualizer : public AlgebraVisitor {
   protected:
   FileWriter out;
   FactorizedTreeDeriver deriver;

   public:
   explicit FTreeCompositionVisualizer(std::ostream& writer) : out(writer, FileWriter::Silent) {};

   void visitPrepare(const Stage s, const TableScan& op) override;
   void visitPrepare(const Stage s, const InnerJoin& op) override;
   void visitPrepare(const Stage s, const Print& op) override;
   void visitPrepare(const Stage s, const Count& op) override;

   void visitProduce(const InnerJoin& op) override;
   void visitProduce(const TableScan& op) override;
   void visitProduce(const Print& op) override;
   void visitProduce(const Selection& op) override;
   void visitProduce(const Count& op) override;
   void visitProduce(const CrossProduct&) override { __builtin_unreachable(); }

   void report(const Operator& op);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
