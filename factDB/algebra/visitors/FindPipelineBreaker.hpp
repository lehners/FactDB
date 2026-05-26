#ifndef H_FACTDB_ALGEBRA_VISITORS_FIND_PIPELINE_BREAKER
#define H_FACTDB_ALGEBRA_VISITORS_FIND_PIPELINE_BREAKER
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra { class Operator; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
struct FindPipelineBreaker : public AlgebraVisitor {
   private:
   const Operator* pipelineBreaker = nullptr;

   public:
   void visitProduce(const Print& print) override;
   void visitProduce(const Selection& selection) override;
   void visitProduce(const Count& count) override;
   void visitProduce(const InnerJoin& join) override;
   void visitProduce(const TableScan& tableScan) override;
   void visitProduce(const CrossProduct& tableScan) override;

   void visitConsume(const ConsumeStage, const InnerJoin& join, const Operator*) override;
   void visitConsume(const ConsumeStage, const Print& print, const Operator*) override;
   void visitConsume(const ConsumeStage, const Count& count, const Operator*) override;
   void visitConsume(const ConsumeStage, const Selection& selection, const Operator*) override;
   void visitConsume(const ConsumeStage, const CrossProduct& selection, const Operator*) override;

   static const Operator& getPipelineStarter(const Operator& op, bool checkNullptr = true);
   static const Operator& getUpperPipelineBreaker(const Operator& op);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_ALGEBRA_VISITORS_FIND_PIPELINE_BREAKER
