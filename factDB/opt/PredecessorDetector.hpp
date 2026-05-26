#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/factorized/FactorizedTreeDeriver.hpp"
#include "factDB/infra/BitSet.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/util/DoOnDestruction.hpp"
#include <sstream>
#include <unordered_map>
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class FactorizedEstimator;
// ---------------------------------------------------------------------------------------------------
struct PredecessorInformation {
   std::unordered_map<uint32_t, infra::BitSet64> predecessors;

   public:
   infra::BitSet64 getPredecessors(uint32_t relations) const;

   /// returns the list of all predecessors for a given set of relations
   infra::BitSet64 getPredecessors(infra::BitSet64 relations) const;
   uint32_t findFirstRelation(infra::BitSet64 relations) const;
   const IU* findFirstIU(const IUSet& ius, const QueryGraph& queryGraph) const;
};
// ---------------------------------------------------------------------------------------------------
class PredecessorDetector {
   std::unique_ptr<algebra::Print> algebraTree;
   factDB::algebra::visitors::FactorizedTreeDeriver factTreeDeriver;
   // const Plan& plan;
   const QueryGraph& queryGraph;
   DoOnDestruction doOnDestruction;

   public:
   PredecessorDetector(const Plan& plan_, const QueryGraph& queryGraph_, FactorizedEstimator& estimator);
   PredecessorDetector(std::unique_ptr<algebra::Operator>& op, const QueryGraph& queryGraph_);

   algebra::Operator& getRootTree() const;
   PredecessorInformation genPredecessorInformation(const algebra::Operator& op) const;
   static size_t getPDC();
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------