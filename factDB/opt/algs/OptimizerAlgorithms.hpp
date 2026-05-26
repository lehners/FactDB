#ifndef H_FACTDB_FACTDB_OPT_ALGS_OPTIMIZER_ALGORIHTMS_HPP
#define H_FACTDB_FACTDB_OPT_ALGS_OPTIMIZER_ALGORIHTMS_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include "factDB/infra/alloc/AllocatorPooled.hpp"
#include "factDB/opt/Plan.hpp"

#include <list>
#include <memory>
#include <unordered_map>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class BaseOracle;
// ---------------------------------------------------------------------------------------------------
class OptimizerAlgorithm {
   public:
   enum Algorithm {
      DPSize,
      DPSizeLinear,
      DPFactLeftDeep,
      DPFactBushy,
      AnyPlan,
      FixedPlan,
   };

   static size_t toInt(Algorithm alg) {
      switch (alg) {
         case DPSize: return 0;
         case DPSizeLinear: return 1;
         case DPFactLeftDeep: return 2;
         case DPFactBushy: return 3;
         case AnyPlan: return 4;
         case FixedPlan: return 5;
         default: unreachable();
      }
   }

   protected:
   pool::PooledAllocator<JoinPlan> planAllocator;

   QueryGraph& queryGraph;
   std::list<BaseTablePlan> baseTables;

   protected:
   OptimizerAlgorithm(QueryGraph& qg) noexcept;

   public:
   virtual ~OptimizerAlgorithm() = default;

   protected:
   JoinPlan* allocatePlan() { return planAllocator.allocate(); };

   virtual void generateOptimalPlan() = 0;

   public:
   [[nodiscard]] virtual Plan* getOptimalPlan() const = 0;
   virtual BaseEstimator& getEstimator() = 0;
   static std::unique_ptr<OptimizerAlgorithm> executeAlgorithm(Algorithm alg, QueryGraph& qg, BaseOracle& oracle);
   static std::unique_ptr<OptimizerAlgorithm> executeAlgorithmCached(QueryGraph& qg, BaseOracle& oracle, const std::string& cachedPlan);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_OPT_ALGS_OPTIMIZER_ALGORIHTMS_HPP
