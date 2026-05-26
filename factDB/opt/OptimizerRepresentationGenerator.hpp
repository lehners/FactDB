#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/opt/OptimizerRepresentation.hpp"
#include <list>
#include <string_view>
#include <unordered_map>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
enum class AggregationMode : uint8_t {
   NotSelected = 0,
   Print = 1,
   CountStar = 2
};
// ---------------------------------------------------------------------------------------------------
struct OptimizerRepresentationGenerator : public algebra::visitors::AlgebraVisitor {
   AggregationMode aggregationMode = AggregationMode::NotSelected;

   std::vector<OptimizerRelation> relations;
   std::vector<JoinInfos> joinInfos;
   std::vector<OptimizerJoin> joins;
   std::list<JoinConditionList> adaptedJoinConditions;
   std::vector<std::pair<unsigned, const Expression&>> tablePredicates;

   infra::BitSet64 coveredRelations;

   std::unordered_map<std::string_view, size_t> tableMap;

   void visitProduce(const algebra::InnerJoin& join) override;
   void visitProduce(const algebra::Print& print) override;
   void visitProduce(const algebra::Selection& selection) override;
   void visitProduce(const algebra::TableScan& tableScan) override;
   void visitProduce(const algebra::Count& tableScan) override;
   void visitProduce(const algebra::CrossProduct& crossProduct) override;

   size_t getTableIdx(const std::string_view& val) const {
      assert(tableMap.contains(val));
      return tableMap.find(val)->second;
   }

   static OptimizerRepresentationGenerator apply(const algebra::Operator* op);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
