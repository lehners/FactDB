// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/estimators/EstimateAlgebraPlan.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/CrossProduct.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/opt/CardinalityOracle.hpp"
#include "factDB/opt/OptimizerRepresentationGenerator.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/estimators/FactorizedEstimator.hpp"
#include "factDB/opt/estimators/FlatEstimator.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
#include <list>
#include <utility>
// ---------------------------------------------------------------------------------------------------
using namespace factDB::opt;
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<bool> estimateFactorizedCosts("optimizer.estimateFactorizedCosts", true);
// ---------------------------------------------------------------------------------------------------
EstimateAlgebraPlan::EstimateAlgebraPlan(QueryGraph& qg, const OptimizerRepresentationGenerator& optRep, BaseEstimator& e) : estimator(e), queryGraph(qg), optimizerRep(optRep) {
}
// ---------------------------------------------------------------------------------------------------
void EstimateAlgebraPlan::visitProduce(const algebra::Selection& selection) {
   selection.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void EstimateAlgebraPlan::visitProduce(const algebra::TableScan& tableScan) {
   auto tblIdx = optimizerRep.getTableIdx(tableScan.getAlias());
   curPlan = &baseTables.emplace_back(tblIdx, estimator);
}
// ---------------------------------------------------------------------------------------------------
void EstimateAlgebraPlan::visitProduce(const algebra::Print& print) {
   print.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void EstimateAlgebraPlan::visitProduce(const algebra::Count& count) {
   count.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void EstimateAlgebraPlan::visitProduce(const factDB::algebra::InnerJoin& join) {
   join.getLeftChild()->produce(*this);
   auto* leftPlan = curPlan;
   join.getRightChild()->produce(*this);
   auto* rightPlan = curPlan;

   JoinPlan* joinPlan = planAllocator.allocate();
   curPlan = joinPlan;
   *joinPlan = opt::Plan::makeJoin(leftPlan, rightPlan, estimator, join.getJoinMode());
}
// ---------------------------------------------------------------------------------------------------
void EstimateAlgebraPlan::visitProduce(const algebra::CrossProduct& crossProduct) {
   crossProduct.getLeftChild()->produce(*this);
   crossProduct.getRightChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<BaseEstimator> getEstimator(QueryGraph& qg, CardinalityOracle& oracle) {
   if (estimateFactorizedCosts.get()) {
      return std::make_unique<FactorizedEstimator>(qg, oracle);
   } else {
      return std::make_unique<FlatEstimator>(qg, oracle);
   }
}
// ---------------------------------------------------------------------------------------------------
size_t getCost(Estimate* estimate) {
   if (estimateFactorizedCosts.get()) {
      return FactorizedEstimate::cast(estimate)->cost;
   } else {
      return FlatEstimate::cast(estimate)->cost;
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------------------------------
size_t EstimateAlgebraPlan::apply(const algebra::Operator& op, Database& database) {
   CardinalityOracle oracle(database);
   auto rep = OptimizerRepresentationGenerator::apply(&op);
   auto qg = QueryGraph::generateQueryGraph(database, rep.relations, rep.joins, rep.joinInfos, rep.tablePredicates);

   auto estimator = getEstimator(qg, oracle);
   EstimateAlgebraPlan eap(qg, rep, *estimator);
   op.produce(eap);

   return getCost(eap.curPlan->getEstimate());
}
// ---------------------------------------------------------------------------------------------------
void EstimateAlgebraPlanHelper::visitProduce(const algebra::Count& op) {
   auto res = EstimateAlgebraPlan::apply(op, db);
   out << "Cost: " << res << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void EstimateAlgebraPlanHelper::visitProduce(const algebra::Print& op) {
   auto res = EstimateAlgebraPlan::apply(op, db);
   out << "Cost: " << res << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
