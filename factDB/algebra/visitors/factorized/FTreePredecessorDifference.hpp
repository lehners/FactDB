#pragma once
// ---------------------------------------------------------------------------------------------------
#include "FactorizedTreeDeriver.hpp"
#include "factDB/algebra/visitors/factorized/FTreeDifference.hpp"
#include "factDB/queryc/writer/FileWriter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
class QueryGraph;
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
struct FTreePredecessorDifferenceProcessor : public algebra::visitors::FTreeDifference {
   const Database& db;
   std::unordered_set<size_t> interesting_nodes;

   std::unordered_map<uint32_t, infra::BitSet64> getPredecessorInformation(const factDB::algebra::Operator& op, const factDB::opt::QueryGraph& queryGraph, bool doNaive) const;

   public:
   explicit FTreePredecessorDifferenceProcessor(const Database& db_, std::ostream& writer) : FTreeDifference(writer), db(db_){};

   void visitPrepare(const Stage, const factDB::algebra::Print&) override {}
   void visitPrepare(const Stage, const factDB::algebra::Count&) override {}

   void visitProduce(const factDB::algebra::InnerJoin&) override;
   void visitProduce(const factDB::algebra::Print&) override;
   void visitProduce(const factDB::algebra::Selection&) override;
   void visitProduce(const factDB::algebra::TableScan&) override {}
   void visitProduce(const factDB::algebra::Count&) override;
   void visitProduce(const factDB::algebra::CrossProduct&) override { __builtin_unreachable(); }

   void report(const Operator& op) const;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors