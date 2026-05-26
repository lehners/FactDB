#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/infra/alloc/AllocatorPooled.hpp"
#include "factDB/opt/Plan.hpp"
#include <cassert>
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
class OptimizeBuildSide : public algebra::visitors::AlgebraVisitor {
   private:
   pool::PooledAllocator<JoinPlan> planAllocator;

   BaseEstimator& estimator;
   QueryGraph& queryGraph;
   const OptimizerRepresentationGenerator& optimizerRep;
   std::list<BaseTablePlan> baseTables;

   Plan* curPlan = nullptr;

   OptimizeBuildSide(QueryGraph& qg, const OptimizerRepresentationGenerator& optRep, BaseEstimator& estimator);

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

   static std::unique_ptr<algebra::Operator> apply(algebra::Operator& op, Database& database);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------