// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/OptimizerRepresentationGenerator.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/CrossProduct.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/BitSet.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/RuntimeException.hpp"
#include "factDB/infra/iu.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
void OptimizerRepresentationGenerator::visitProduce(const algebra::Print& print) {
   aggregationMode = AggregationMode::Print;
   print.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void OptimizerRepresentationGenerator::visitProduce(const algebra::Count& count) {
   aggregationMode = AggregationMode::CountStar;
   count.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void OptimizerRepresentationGenerator::visitProduce(const algebra::Selection& selection) {
   selection.getChild()->produce(*this);
   if (coveredRelations.size() != 1)
      throw RuntimeException(NotImplementedYet, "Optimizer Representations for Selections only implemented on base tables yet.");
   tablePredicates.emplace_back(*coveredRelations.begin(), selection.getPredicate());
}
// ---------------------------------------------------------------------------------------------------
void OptimizerRepresentationGenerator::visitProduce(const algebra::CrossProduct& crossProduct) {
   crossProduct.getLeftChild()->produce(*this);
   auto leftCovered = std::move(coveredRelations);
   crossProduct.getRightChild()->produce(*this);
   coveredRelations += leftCovered;
}
// ---------------------------------------------------------------------------------------------------
void OptimizerRepresentationGenerator::visitProduce(const algebra::InnerJoin& join) {
   join.getLeftChild()->produce(*this);
   auto leftCovered = std::move(coveredRelations);
   join.getRightChild()->produce(*this);
   auto rightCovered = std::move(coveredRelations);

   for (const JoinCondition& predicate : join.getJoinCondition()) {
      assert(tableMap.contains(predicate.get_left().table));
      assert(tableMap.contains(predicate.get_right().table));

      auto leftTable = tableMap[predicate.get_left().table];
      auto rightTable = tableMap[predicate.get_right().table];
      infra::BitSet64 leftTableSet(leftTable), rightTableSet(rightTable);
      if (join.getJoinType() == algebra::JoinType::RightSemi) { // filter right side by left elements
         assert(leftCovered.contains(leftTable));
         leftTableSet = leftCovered;
      }

      size_t joinInfoIdx = joinInfos.size();
      joins.emplace_back(leftTableSet, rightTableSet, joinInfoIdx, join.getJoinType());
      // create the new join predicate if necessary
      std::vector<JoinCondition> predicateVec = {predicate};
      auto& newList = adaptedJoinConditions.emplace_back(std::move(predicateVec));
      joinInfos.emplace_back(&newList);
      assert(joinInfoIdx + 1 == joinInfos.size());
   }

   coveredRelations = leftCovered + rightCovered;
}
// ---------------------------------------------------------------------------------------------------
void OptimizerRepresentationGenerator::visitProduce(const algebra::TableScan& tableScan) {
   assert(!tableMap.contains(tableScan.getAlias())); // no table must occur twice
   tableMap[tableScan.getAlias()] = relations.size();
   coveredRelations = infra::BitSet64(relations.size());
   relations.emplace_back(tableScan.getTable(), tableScan.getAlias());
}
// ---------------------------------------------------------------------------------------------------
OptimizerRepresentationGenerator OptimizerRepresentationGenerator::apply(const algebra::Operator* op) {
   OptimizerRepresentationGenerator optimizerRepresentation;
   op->produce(optimizerRepresentation);
   return optimizerRepresentation;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
