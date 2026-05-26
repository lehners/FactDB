#ifndef H_FACTDB_FACT_DB_ALGEBRA_VISITORS_VECTORIZEDEXECUTION_HPP
#define H_FACTDB_FACT_DB_ALGEBRA_VISITORS_VECTORIZEDEXECUTION_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/infra/IUSet.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <list>
#include <unordered_map>
#include <vector>
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB { class Expression; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
class MaterializedExecution : public AlgebraVisitorDB {
   private:
   using TupleList = std::vector<std::vector<RuntimeValue>>;
   std::unordered_map<const IU*, size_t, IUPointerHash, IUPointerEqual> ius;
   TupleList result;
   std::ostream& outStream;

   public:
   explicit MaterializedExecution(const Database& db, std::ostream& out);
   /// Destructor
   ~MaterializedExecution() override = default;

   void visitProduce(const InnerJoin& join) override;
   void visitProduce(const Print& print) override;
   void visitProduce(const Selection& selection) override;
   void visitProduce(const TableScan& tableScan) override;
   void visitProduce(const Count& count) override;
   void visitProduce(const CrossProduct& count) override;

   // void visitConsume(const AlgebraVisitor::ConsumeStage stage, const InnerJoin& join, const Operator* caller) override;
   // void visitConsume(const AlgebraVisitor::ConsumeStage stage, const Print& print, const Operator* caller) override;
   // void visitConsume(const AlgebraVisitor::ConsumeStage stage, const Selection& selection, const Operator* caller) override;
   // void visitConsume(const AlgebraVisitor::ConsumeStage stage, const Count&, const Operator*) override;

   void open() override{};
   void close() override{};
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACT_DB_ALGEBRA_VISITORS_VECTORIZEDEXECUTION_HPP
