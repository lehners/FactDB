#include "factDB/opt/Optimizer.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/RuntimeException.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/opt/CardinalityOracle.hpp"
#include "factDB/opt/CreateJoins.hpp"
#include "factDB/opt/FlattenJoins.hpp"
#include "factDB/opt/OptimizeBuildSide.hpp"
#include "factDB/opt/OptimizerRepresentationGenerator.hpp"
#include "factDB/opt/PredicatePushDown.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/algs/OptimizerAlgorithms.hpp"
#include "factDB/opt/estimators/EstimateAlgebraPlan.hpp"
#include "factDB/opt/estimators/FactorizedEstimator.hpp"
#include "factDB/queryc/ExecutionMode.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
static Setting<bool> doOptimize("optimizer.doOptimize", true);
static Setting<bool> genFactorizedEdges("optimizer.genFactorizedEdges", false);
static Setting<bool> useOptimizerCache("optimizer.cache", true);
// ---------------------------------------------------------------------------------------------------
static constexpr auto executionModeParser = settinghelper::makeEnumParser(
   std::tuple{OptimizerAlgorithm::DPSize, "DPSize", 'S'},
   std::tuple{OptimizerAlgorithm::DPSizeLinear, "DPSizeLinear", 'L'},
   std::tuple{OptimizerAlgorithm::DPFactLeftDeep, "DPFactLeftDeep", 'F'},
   std::tuple{OptimizerAlgorithm::DPFactBushy, "DPFactLeftDeep", 'B'},
   std::tuple{OptimizerAlgorithm::AnyPlan, "AnyPlan", 'A'});
static Setting<OptimizerAlgorithm::Algorithm> optimizerAlgorithm("optimizer.algorithm", OptimizerAlgorithm::DPFactLeftDeep, &executionModeParser);
static factDB::Setting<bool> doBottomInserts("opt.doBottomInserts", true);
static factDB::Setting<bool> baseTableFromUmbra("opt.oracle.baseTableFromUmbra", true);
static factDB::Setting<bool> reorderPlans("opt.reorderPlans", false);
// ---------------------------------------------------------------------------------------------------
enum OptimizerPass {
   PredicatePushdown,
   Reordering,
   FlattenJoins,
   OptimizeBuildSides,
};
// ---------------------------------------------------------------------------------------------------
constexpr std::initializer_list<OptimizerPass> passes = {
   PredicatePushdown,
   Reordering,
   OptimizeBuildSides,
};
// ---------------------------------------------------------------------------------------------------
QueryOptimizer::QueryOptimizer(Database& db, QueryOptimizer::OperatorTree t, factDB::OrderedIUSet cols)
   : database(db), tree(std::move(t)), requiredColumns(std::move(cols)) {
}
// ---------------------------------------------------------------------------------------------------
void QueryOptimizer::pushPredicatesDown() const {
   PredicatePushDown::apply(*tree);
   CreateJoins::apply(*tree);
}
// ---------------------------------------------------------------------------------------------------
void QueryOptimizer::flattenJoins() const {
   FlattenJoins::apply(*tree);
}
// ---------------------------------------------------------------------------------------------------
void QueryOptimizer::optimizeBuildSides() {
   auto newTree = OptimizeBuildSide::apply(*tree, database);
   requiredColumns = updateRequiredIUs(requiredColumns, *newTree);
   tree = std::move(newTree); // must be kept since required columns is still based on old tree.
}
// ---------------------------------------------------------------------------------------------------
OrderedIUSet QueryOptimizer::updateRequiredIUs(const OrderedIUSet& requiredColumns, algebra::Operator& newTree) {
   IUSet newIUs = newTree.collectIUs();

   OrderedIUSet newRequiredColumns;
   newRequiredColumns.reserve(requiredColumns.size());
   for (auto& curCol : requiredColumns) {
      auto iter = newIUs.find(curCol);
      assert(iter != newIUs.end());
      newRequiredColumns.emplace_back(*iter);
   }
   return newRequiredColumns;
}
// ---------------------------------------------------------------------------------------------------
void QueryOptimizer::reorderJoins() {
   auto tmpGoodMerge = SettingBase::getSetting<bool>("codegen.factorized.naiveMerge")->setTemporary(false); // disable naive merge during join reordering to create identical results
   CardinalityOracle oracle(database);
   auto qg = QueryGraph::generateQueryGraph(database, *tree);
   std::unique_ptr<OptimizerAlgorithm> optimizer;
   auto cachedPlan = oracle.lookupCachedPlan(optimizerAlgorithm.get());
   if (!cachedPlan.empty()) {
      optimizer = OptimizerAlgorithm::executeAlgorithmCached(qg, oracle, cachedPlan);
   } else if (reorderPlans.get()) {
      optimizer = OptimizerAlgorithm::executeAlgorithm(optimizerAlgorithm.get(), qg, oracle);
   } else {
      throw RuntimeException(DoNotGeneratePlan, "Setting disabled");
   }
   std::unique_ptr<algebra::Operator> planTree = nullptr;
   if (genFactorizedEdges.get()) {
      FactorizedEstimator factorizedEstimator(qg, oracle);
      planTree = optimizer->getOptimalPlan()->generateOperatorPlan(qg, factorizedEstimator, false);
   } else {
      planTree = optimizer->getOptimalPlan()->generateOperatorPlan(qg, optimizer->getEstimator(), false);
   }

   if (cachedPlan.empty()) {
      oracle.insertCachedPlan(optimizer->getOptimalPlan(), optimizerAlgorithm.get(), qg);
   }
   auto newTree = qg.generateAggregatedTree(std::move(planTree), *tree);
   requiredColumns = updateRequiredIUs(requiredColumns, *newTree);
   tree = std::move(newTree); // must be kept since required columns is still based on old tree.
   oracle.store();
}
// ---------------------------------------------------------------------------------------------------
void QueryOptimizer::optimizeTree() {
   std::vector curPasses(passes);
   if (SettingBase::getSetting<queryc::ExecutionMode>("codegen.executionMode")->get() == (queryc::ExecutionMode::CodegenFlat) || !doBottomInserts.get()) {
      curPasses.emplace_back(OptimizerPass::FlattenJoins);
   }

   for (auto& pass : curPasses) {
      switch (pass) {
         case PredicatePushdown:
            pushPredicatesDown();
            break;
         case Reordering:
            reorderJoins();
            break;
         case FlattenJoins:
            flattenJoins();
            break;
         case OptimizeBuildSides:
            if (optimizerAlgorithm.get() != OptimizerAlgorithm::DPFactLeftDeep)
               optimizeBuildSides();
      }
   }
}
// ---------------------------------------------------------------------------------------------------
QueryOptimizer::TreeStructure QueryOptimizer::getOptimalTree() {
   return std::make_pair(std::move(tree), requiredColumns);
}
// ---------------------------------------------------------------------------------------------------
QueryOptimizer::OperatorTree QueryOptimizer::optimize(Database& db, OperatorTree tree, OrderedIUSet& requiredColumns) {
   if (!doOptimize.get())
      return tree;
   QueryOptimizer qo(db, std::move(tree), requiredColumns);
   qo.optimizeTree();

   auto optTreeStruct = qo.getOptimalTree();
   requiredColumns = optTreeStruct.second;
   return std::move(optTreeStruct.first);
}
// ---------------------------------------------------------------------------------------------------
size_t QueryOptimizer::estimateCost(algebra::Operator& tree, Database& database) {
   return EstimateAlgebraPlan::apply(tree, database);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------