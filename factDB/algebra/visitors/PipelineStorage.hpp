#ifndef H_FACTDB_ALGEBRA_VISITORS_PIPELINE_VISITOR
#define H_FACTDB_ALGEBRA_VISITORS_PIPELINE_VISITOR
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/Operator.hpp"
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/algebra/visitors/FindPipelineBreaker.hpp"
#include <cassert>
#include <memory>
#include <unordered_map>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
template <typename ValueT>
class PipelineStore {
   std::unordered_map<const Operator*, ValueT> map;

   public:
   const ValueT& get(const Operator& op) const {
      const Operator& pipelineBreaker = FindPipelineBreaker::getPipelineStarter(op, true);
      auto found = map.find(&pipelineBreaker);
      assert(found != map.end());
      return found->second;
   }

   const ValueT& operator[](const Operator& op) const { return get(op); }
   const ValueT& operator[](const Operator* op) const { return get(*op); }
   template <std::derived_from<Operator> OperatorType>
   const ValueT& operator[](const std::unique_ptr<OperatorType>& op) const { return get(*op); }

   ValueT& get(const Operator& op) { return map[&FindPipelineBreaker::getPipelineStarter(op, true)]; }
   ValueT& operator[](const Operator& op) { return get(op); }
   ValueT& operator[](const Operator* op) { return get(*op); }
   template <std::derived_from<Operator> OperatorType>
   ValueT& operator[](const std::unique_ptr<OperatorType>& op) { return get(*op); }

   bool containsPipeline(const Operator& op) { return map.contains(&FindPipelineBreaker::getPipelineStarter(op, true)); }

   auto begin() { return map.begin(); }
   auto end() { return map.end(); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_ALGEBRA_VISITORS_PIPELINE_VISITOR
