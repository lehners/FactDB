// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/PredecessorDetector.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/estimators/FactorizedEstimator.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
PredecessorDetector::PredecessorDetector(const Plan& plan, const QueryGraph& queryGraph_, FactorizedEstimator& estimator)
   : queryGraph(queryGraph_), doOnDestruction([]() {}) {
   algebraTree = std::make_unique<algebra::Print>(plan.generateOperatorPlan(queryGraph, estimator));
   algebraTree->prepare(factTreeDeriver, algebraTree->collectIUs(), nullptr);
}
// ---------------------------------------------------------------------------------------------------
PredecessorDetector::PredecessorDetector(std::unique_ptr<algebra::Operator>& op, const factDB::opt::QueryGraph& queryGraph_)
   : queryGraph(queryGraph_), doOnDestruction([&]() { op = std::move(algebraTree->getChild()); }) {
   algebraTree = std::make_unique<algebra::Print>(std::move(op));
   algebraTree->prepare(factTreeDeriver, algebraTree->collectIUs(), nullptr);
}
// ---------------------------------------------------------------------------------------------------
algebra::Operator& PredecessorDetector::getRootTree() const {
   return *algebraTree;
}
// ---------------------------------------------------------------------------------------------------
PredecessorInformation PredecessorDetector::genPredecessorInformation(const algebra::Operator& op) const {
   auto& factorizedTree = factTreeDeriver.getFTree(op);
   std::unordered_map<uint32_t, infra::BitSet64> relations2Predecessors;
   std::list<std::tuple<infra::BitSet64, const FNode*>> todos;
   todos.emplace_back(infra::BitSet64{}, &factorizedTree.getRootNode());
   while (!todos.empty()) {
      infra::BitSet64 seenRelations;
      auto [curPredecessors, curNode] = todos.front();
      todos.pop_front();

      for (const IU* iu : curNode->getIUs()) {
         if (!queryGraph.knowsIU(iu)) continue; // iu not relevant in query
         auto rel = queryGraph.getRelation(iu);
         curPredecessors.insert(rel);
         seenRelations.insert(rel);
      }

      for (const auto& child : curNode->getChildren()) {
         // store predecessors for all child nodes, since this node is required to expand for each child node access
         todos.emplace_back(curPredecessors, child.get());
      }

      for (auto rel : seenRelations) {
         assert(!relations2Predecessors.contains(rel));
         relations2Predecessors[rel] = curPredecessors;
      }
   }
   return {relations2Predecessors};
}
// ---------------------------------------------------------------------------------------------------
infra::BitSet64 PredecessorInformation::getPredecessors(uint32_t relation) const {
   assert(predecessors.contains(relation));
   return predecessors.find(relation)->second;
}
// ---------------------------------------------------------------------------------------------------
infra::BitSet64 PredecessorInformation::getPredecessors(infra::BitSet64 relations) const {
   auto connection = relations;
   for (auto rel : relations) {
      if (!predecessors.contains(rel)) continue;
      connection += getPredecessors(rel);
   }
   return connection;
}
// ---------------------------------------------------------------------------------------------------
uint32_t PredecessorInformation::findFirstRelation(infra::BitSet64 relations) const {
   assert(!relations.empty() && "got an empty set of relations, cannot find a first relation");
   auto optRelation = relations.front();
   auto optPredecessors = getPredecessors(optRelation);

   for (auto iter = ++relations.begin(); iter != relations.end(); ++iter) {
      auto curPredecessors = getPredecessors(*iter);
      if (curPredecessors.isSubsetOf(optPredecessors)) { // found a better relation
         optRelation = *iter;
         optPredecessors = curPredecessors;
      } // else current optimal relation is better
   }
   return optRelation;
}
// ---------------------------------------------------------------------------------------------------
const IU* PredecessorInformation::findFirstIU(const IUSet& ius, const QueryGraph& queryGraph) const {
   assert(!ius.empty() && "got an empty set of IUs, cannot find a first relation");
   auto optIU = *ius.begin();
   auto optPredecessors = getPredecessors(queryGraph.getRelation(optIU));

   for (auto iter = ++ius.begin(); iter != ius.end(); ++iter) {
      auto curRelation = queryGraph.getRelation(*iter);
      auto curPredecessors = getPredecessors(curRelation);
      if (curPredecessors.isSubsetOf(optPredecessors)) { // found a better relation
         optIU = *iter;
         optPredecessors = curPredecessors;
      } // else current optimal relation is better
   }
   return optIU;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------