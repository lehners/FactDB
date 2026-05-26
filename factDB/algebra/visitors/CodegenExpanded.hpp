#ifndef H_FACTDB_FACT_DB_ALGEBRA_VISITORS_CODEGENEXPANDED_HPP
#define H_FACTDB_FACT_DB_ALGEBRA_VISITORS_CODEGENEXPANDED_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/queryc/writer/FileWriter.hpp"
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
class CodegenExpanded : public AlgebraVisitorDB {
   protected:
   FileWriter& out;
   std::vector<uint32_t> requiredTableIds;
   std::vector<std::string> mismatchDebugOutput = {};

   virtual void addHeaders();
   virtual std::string execMode() { return "CodegenFlat"; }

   public:
   explicit CodegenExpanded(const Database& db, FileWriter& writer);
   /// Destructor
   ~CodegenExpanded() override = default;

   void visitPrepare(const Stage, const TableScan& op) override;

   void visitProduce(const InnerJoin& join) override;
   void visitProduce(const Print& print) override;
   void visitProduce(const Selection& selection) override;
   void visitProduce(const TableScan& tableScan) override;
   void visitProduce(const Count& count) override;
   void visitProduce(const CrossProduct& count) override;

   void visitConsume(const AlgebraVisitor::ConsumeStage stage, const InnerJoin& join, const Operator* caller) override;
   void visitConsume(const AlgebraVisitor::ConsumeStage stage, const Print& print, const Operator* caller) override;
   void visitConsume(const AlgebraVisitor::ConsumeStage stage, const Selection& selection, const Operator* caller) override;
   void visitConsume(const AlgebraVisitor::ConsumeStage stage, const Count&, const Operator*) override;
   void visitConsume(const AlgebraVisitor::ConsumeStage stage, const CrossProduct&, const Operator*) override;

   void open() override;
   void close() override;

   void produceGeneratorInfo(const Operator& op);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACT_DB_ALGEBRA_VISITORS_CODEGENEXPANDED_HPP
