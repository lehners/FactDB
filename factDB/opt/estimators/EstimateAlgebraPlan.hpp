#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/infra/alloc/AllocatorPooled.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/queryc/writer/FileWriter.hpp"
#include <list>
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
struct OptimizerRepresentationGenerator;
class BaseEstimator;
// ---------------------------------------------------------------------------------------------------
class EstimateAlgebraPlan : public algebra::visitors::AlgebraVisitor {
   private:
   pool::PooledAllocator<JoinPlan> planAllocator;

   BaseEstimator& estimator;
   QueryGraph& queryGraph;
   const OptimizerRepresentationGenerator& optimizerRep;
   std::list<BaseTablePlan> baseTables;

   Plan* curPlan = nullptr;

   EstimateAlgebraPlan(QueryGraph& qg, const OptimizerRepresentationGenerator& optRep, BaseEstimator& estimator);

   public:
   void visitProduce(const algebra::InnerJoin&) override;
   void visitProduce(const algebra::Print&) override;
   void visitProduce(const algebra::Selection&) override;
   void visitProduce(const algebra::TableScan&) override;
   void visitProduce(const algebra::Count&) override;
   void visitProduce(const algebra::CrossProduct&) override;

   static size_t apply(const algebra::Operator& op, Database& database);
};
// ---------------------------------------------------------------------------------------------------
class EstimateAlgebraPlanHelper : public algebra::visitors::DefaultVisitor {
   private:
   Database& db;
   FileWriter out;

   public:
   EstimateAlgebraPlanHelper(Database& database, std::ostream& writer) : db(database), out(writer, FileWriter::Silent) {}
   void visitProduce(const algebra::Count& op) override;
   void visitProduce(const algebra::Print& op) override;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------