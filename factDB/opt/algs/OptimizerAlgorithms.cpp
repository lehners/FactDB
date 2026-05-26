// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/algs/OptimizerAlgorithms.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/opt/algs/AnyPlan.hpp"
#include "factDB/opt/algs/DPFact.hpp"
#include "factDB/opt/algs/DPSize.hpp"
#include "factDB/opt/algs/FixedPlan.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<OptimizerAlgorithm> getAlgorithmContainer(OptimizerAlgorithm::Algorithm algorithm, QueryGraph& qg, BaseOracle& oracle) {
   switch (algorithm) {
      case OptimizerAlgorithm::AnyPlan:
         return std::make_unique<algs::AnyPlan>(qg, oracle);
      case OptimizerAlgorithm::DPSizeLinear:
         return std::make_unique<algs::DPSizeLinear>(qg, oracle);
      case OptimizerAlgorithm::DPSize:
         return std::make_unique<algs::DPSize>(qg, oracle);
      case OptimizerAlgorithm::DPFactLeftDeep:
         return std::make_unique<algs::DPFactLeftDeep>(qg, oracle);
      case OptimizerAlgorithm::DPFactBushy:
         return std::make_unique<algs::DPFact>(qg, oracle);
      case OptimizerAlgorithm::FixedPlan:
         return std::make_unique<algs::FixedPlan>(qg, oracle);
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------------------------------
OptimizerAlgorithm::OptimizerAlgorithm(QueryGraph& qg) noexcept
   : queryGraph(qg) {
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<OptimizerAlgorithm> OptimizerAlgorithm::executeAlgorithm(Algorithm algorithm, QueryGraph& qg, BaseOracle& oracle) {
   assert(algorithm != OptimizerAlgorithm::FixedPlan);
   std::unique_ptr<OptimizerAlgorithm> opt = getAlgorithmContainer(algorithm, qg, oracle);

   opt->generateOptimalPlan();
   return opt;
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<OptimizerAlgorithm> OptimizerAlgorithm::executeAlgorithmCached(QueryGraph& qg, BaseOracle& oracle, const std::string& cachedPlan) {
   auto opt = std::make_unique<algs::FixedPlan>(qg, oracle);
   opt->setPlan(cachedPlan);

   opt->generateOptimalPlan();
   return opt;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------