// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/OptimizeBuildSide.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/CrossProduct.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/opt/CardinalityOracle.hpp"

#include "estimators/FactorizedEstimatorOnlyCardinality.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/opt/OptimizerRepresentationGenerator.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/estimators/FactorizedEstimator.hpp"
#include <list>
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
OptimizeBuildSide::OptimizeBuildSide(QueryGraph& qg, const OptimizerRepresentationGenerator& optRep, BaseEstimator& e) : estimator(e), queryGraph(qg), optimizerRep(optRep) {
}
// ---------------------------------------------------------------------------------------------------
void OptimizeBuildSide::visitProduce(algebra::Selection& selection) {
   setConsumer(*selection.getChild(), &selection);
   selection.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void OptimizeBuildSide::visitProduce(algebra::TableScan& tableScan) {
   auto tblIdx = optimizerRep.getTableIdx(tableScan.getAlias());
   curPlan = &baseTables.emplace_back(tblIdx, estimator);
}
// ---------------------------------------------------------------------------------------------------
void OptimizeBuildSide::visitProduce(algebra::Print& print) {
   setConsumer(*print.getChild(), &print);
   print.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void OptimizeBuildSide::visitProduce(algebra::Count& count) {
   setConsumer(*count.getChild(), &count);
   count.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void OptimizeBuildSide::visitProduce(factDB::algebra::InnerJoin& join) {
   assert(join.getJoinMode() == algebra::JoinMode::TopInsert);
   join.getLeftChild()->produce(*this);
   auto* leftPlan = curPlan;
   join.getRightChild()->produce(*this);
   auto* rightPlan = curPlan;

   auto leftEstimate = static_cast<const FlatEstimate&>(*leftPlan->getEstimate()).cardinality;
   auto rightEstimate = static_cast<const FlatEstimate&>(*rightPlan->getEstimate()).cardinality;

   JoinPlan* joinPlan = planAllocator.allocate();
   curPlan = joinPlan;
   if (leftEstimate <= 40 * rightEstimate) {
      *joinPlan = opt::Plan::makeJoin(leftPlan, rightPlan, estimator, algebra::JoinMode::TopInsert);
   } else {
      *joinPlan = opt::Plan::makeJoin(rightPlan, leftPlan, estimator, algebra::JoinMode::BottomInsert);
   }
}
// ---------------------------------------------------------------------------------------------------
void OptimizeBuildSide::visitProduce(algebra::CrossProduct& crossProduct) {
   setConsumer(*crossProduct.getLeftChild(), &crossProduct);
   crossProduct.getLeftChild()->produce(*this);

   setConsumer(*crossProduct.getRightChild(), &crossProduct);
   crossProduct.getRightChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<algebra::Operator> OptimizeBuildSide::apply(algebra::Operator& op, Database& database) {
   CardinalityOracle oracle(database);
   auto rep = OptimizerRepresentationGenerator::apply(&op);
   auto qg = QueryGraph::generateQueryGraph(database, rep.relations, rep.joins, rep.joinInfos, rep.tablePredicates);
   FactorizedEstimatorOnlyCardinality estimator(qg, oracle);
   OptimizeBuildSide obs(qg, rep, estimator);
   op.produce(obs);

   auto planTree = obs.curPlan->generateOperatorPlan(qg, estimator, false);
   return qg.generateAggregatedTree(std::move(planTree), op);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------