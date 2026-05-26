// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/estimators/FactorizedEstimatorOnlyCardinality.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Reference.hpp"
#include "factDB/infra/BitSet.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/opt/CardinalityOracle.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/opt/PredecessorDetector.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/algs/OptimizerAlgorithms.hpp"
#include "factDB/opt/estimators/DummyEstimator.hpp"
#include "factDB/opt/estimators/FactorizedEstimator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
opt::Estimate* FactorizedEstimatorOnlyCardinality::estimateBaseTablePlan(const opt::BaseTablePlan& plan) {
   auto& estimate = estimates.emplace_back();
   estimate.cardinality = oracle.estimate(plan, queryGraph, *this);
   estimate.cost = 0;
   return &estimate;
}
// ---------------------------------------------------------------------------------------------------
opt::Estimate* FactorizedEstimatorOnlyCardinality::estimateJoinPlan(const opt::JoinPlan& plan) {
   // only generates cardinalities of children
   assert(plan.getType() == opt::Join);

   auto& estimate = estimates.emplace_back();

   // here is the algebra tree generated. Maybe we can optimize here to generate the factorized tree only once?
   opt::PredecessorDetector predecessorDetector(plan, queryGraph, *this);
   auto* topJoin = algebra::Print::dynCast(&predecessorDetector.getRootTree())->getChild().get();
   while (topJoin->getType() == algebra::Operator::Reference) {
      topJoin = &static_cast<algebra::Reference*>(topJoin)->getChild();
   }
   assert(topJoin->getType() == algebra::Operator::InnerJoin);
   auto& topJoinTree = *algebra::InnerJoin::dynCast(topJoin);

   auto leftPredecessorInformation = predecessorDetector.genPredecessorInformation(*topJoinTree.getLeftChild());
   auto rightPredecessorInformation = predecessorDetector.genPredecessorInformation(*topJoinTree.getRightChild());

   {
      infra::BitSet64 connectingRelations;
      for (auto& joinCondition : topJoinTree.getJoinCondition()) { // reuse join condition to get the connecting joins, these used IUs should already be optimized.
         assert(queryGraph.knowsIU(&joinCondition.get_left()) && queryGraph.knowsIU(&joinCondition.get_right()));
         connectingRelations.insert(queryGraph.getRelation(&joinCondition.get_left()));
         connectingRelations.insert(queryGraph.getRelation(&joinCondition.get_right()));
      }
      auto leftConnectingRelations = leftPredecessorInformation.getPredecessors(connectingRelations & plan.getLeft()->getCoveredRelations());
      auto rightConnectingRelations = rightPredecessorInformation.getPredecessors(connectingRelations & plan.getRight()->getCoveredRelations());

      opt::FlatEstimate* leftEstimate = static_cast<opt::FlatEstimate*>(plan.getLeft()->getEstimate());
      opt::FlatEstimate* rightEstimate = static_cast<opt::FlatEstimate*>(plan.getRight()->getEstimate());

      leftEstimate->cardinality = executeSemiJoinPlan(*plan.getLeft(), leftConnectingRelations, plan.getLeft()->getCoveredRelations() - leftConnectingRelations);
      rightEstimate->cardinality = executeSemiJoinPlan(*plan.getRight(), rightConnectingRelations, plan.getRight()->getCoveredRelations() - rightConnectingRelations);

      estimate.cardinality = oracle.estimate(plan, queryGraph, *this);
      estimate.cost = leftEstimate->cost + rightEstimate->cost + leftEstimate->cardinality + rightEstimate->cardinality;
   }
   return &estimate;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------