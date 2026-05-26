#ifndef H_FACTDB_ALGEBRA_VISITORS_CODEGEN_FACTORIZED
#define H_FACTDB_ALGEBRA_VISITORS_CODEGEN_FACTORIZED
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/CodegenExpanded.hpp"
#include "factDB/algebra/visitors/PipelineStorage.hpp"
#include "factDB/algebra/visitors/factorized/FactorizedTreeDeriver.hpp"
#include "factDB/newftree/generator/util/FGenUtil.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
class CodegenFactorized : public CodegenExpanded {
   FactorizedTreeDeriver tree_deriver_;
   PipelineStore<FHandle> handleStorage;

   protected:
   void addHeaders() override;
   std::string execMode() override;

   public:
   explicit CodegenFactorized(const Database& db, FileWriter& writer) : CodegenExpanded(db, writer) {};

   void visitPrepare(const Stage s, const TableScan&) override;
   void visitPrepare(const Stage s, const InnerJoin&) override;
   void visitPrepare(const Stage, const Print&) override;
   void visitPrepare(const Stage s, const Count&) override;

   void visitProduce(const TableScan& join) override;
   void visitProduce(const InnerJoin& join) override;
   void visitConsume(const ConsumeStage stage, const InnerJoin& join, const Operator* caller) override;
   void visitConsumeBottomInsert(const ConsumeStage stage, const InnerJoin& join, const Operator* caller);
   void visitConsumeTopInsert(const ConsumeStage stage, const InnerJoin& join, const Operator* caller);
   void visitConsume(const ConsumeStage stage, const Count&, const Operator*) override;

   void open() override;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_ALGEBRA_VISITORS_CODEGEN_FACTORIZED
