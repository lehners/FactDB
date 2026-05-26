#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/opt/CardinalityOracle.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/estimators/FactorizedEstimatorOnlyCardinality.hpp"
#include "factDB/opt/estimators/FlatEstimator.hpp"
#include "factDB/queryc/writer/FileWriter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
class FlatEstimatorVariant : public opt::BaseEstimator {
   std::list<opt::FlatEstimate> estimates;

   public:
   explicit FlatEstimatorVariant(const opt::QueryGraph& qg, opt::BaseOracle& cardinalityOracle) : BaseEstimator(qg, cardinalityOracle) {}
   ~FlatEstimatorVariant() override = default;

   opt::Estimate* estimateBaseTablePlan(const opt::BaseTablePlan& baseTablePlan) override;
   opt::Estimate* estimateJoinPlan(const opt::JoinPlan& plan) override;
};
// ---------------------------------------------------------------------------------------------------
class FactorizationGain : public AlgebraVisitor {
   private:
   Database& database;
   FileWriter out;

   opt::QueryGraph queryGraph;
   std::list<opt::BaseTablePlan> baseTablePlans;
   std::list<opt::JoinPlan> joinPlans;

   opt::CardinalityOracle oracle;
   opt::FactorizedEstimatorOnlyCardinality factorizedEstimator;
   FlatEstimatorVariant flatEstimator{queryGraph, oracle};
   opt::Plan* generatedPlanFactorized = nullptr;
   opt::Plan* generatedPlanFlat = nullptr;
   std::unordered_map<opt::Plan*, size_t> flatCost;

   std::vector<std::tuple<const uint64_t*, const uint64_t*, JoinMode>> toPrint;

   public:
   FactorizationGain(Database& db, std::ostream& writer) : database(db), out(writer, FileWriter::Silent), queryGraph(database), oracle(database), factorizedEstimator(queryGraph, oracle) {}
   ~FactorizationGain() override = default;

   // void visitPrepare(const Stage, const InnerJoin& join) override;
   // void visitPrepare(const Stage, const Print& print) override;
   // void visitPrepare(const Stage, const Selection& selection) override;
   // void visitPrepare(const Stage, const TableScan& tableScan) override;
   // void visitPrepare(const Stage, const Count& tableScan) override;
   // void visitPrepare(const Stage, const CrossProduct& crossProduct) override;

   void visitProduce(const InnerJoin& join) override;
   void visitProduce(const Print& print) override;
   void visitProduce(const Selection& selection) override;
   void visitProduce(const TableScan& tableScan) override;
   void visitProduce(const Count& tableScan) override;
   void visitProduce(const CrossProduct& crossProduct) override;
};
// ---------------------------------------------------------------------------------------------------
class FlatSizes : public AlgebraVisitor {
   Database& database;
   FileWriter out;

   opt::QueryGraph queryGraph;
   std::list<opt::BaseTablePlan> baseTablePlans;
   std::list<opt::JoinPlan> joinPlans;

   opt::CardinalityOracle oracle;
   FlatEstimatorVariant flatEstimator = FlatEstimatorVariant(queryGraph, oracle);
   opt::Plan* generatedPlanFlat = nullptr;
   std::unordered_map<opt::Plan*, size_t> flatCost;

   std::vector<const uint64_t*> toPrint;

   public:
   FlatSizes(Database& db, std::ostream& writer) : database(db), out(writer, FileWriter::Silent), queryGraph(database), oracle(database) {}
   ~FlatSizes() override = default;

   void visitProduce(const InnerJoin& join) override;
   void visitProduce(const Print& print) override;
   void visitProduce(const Selection& selection) override;
   void visitProduce(const TableScan& tableScan) override;
   void visitProduce(const Count& tableScan) override;
   void visitProduce(const CrossProduct& crossProduct) override;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
