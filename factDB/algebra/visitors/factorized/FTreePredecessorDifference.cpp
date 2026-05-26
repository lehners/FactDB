// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/factorized/FTreePredecessorDifference.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/CrossProduct.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/opt/PredecessorDetector.hpp"
#include "factDB/opt/QueryGraph.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace factDB::algebra::visitors;
// ---------------------------------------------------------------------------------------------------
std::unordered_map<uint32_t, factDB::infra::BitSet64> FTreePredecessorDifferenceProcessor::getPredecessorInformation(const factDB::algebra::Operator& op, const factDB::opt::QueryGraph& queryGraph, bool doNaive) const {
   auto& factorizedTree = (doNaive ? naiveDeriver : improvedDeriver).getFTree(op);
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
void FTreePredecessorDifferenceProcessor::visitProduce(const factDB::algebra::Print& p) {
   p.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FTreePredecessorDifferenceProcessor::visitProduce(const factDB::algebra::Count& p) {
   p.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FTreePredecessorDifferenceProcessor::visitProduce(const factDB::algebra::Selection& p) {
   p.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FTreePredecessorDifferenceProcessor::visitProduce(const factDB::algebra::InnerJoin& join) {
   auto queryGraph = factDB::opt::QueryGraph::generateQueryGraph(db, join);

   auto naivePred = getPredecessorInformation(join, queryGraph, true);
   auto paperPred = getPredecessorInformation(join, queryGraph, false);

   for (auto& [k, vNaive] : naivePred) {
      assert(paperPred.contains(k));
      auto vPaper = paperPred[k];
      if (vNaive != vPaper) {
         out << fw::checkQuotes(false) << "Different predecessors detected for relation \"" << queryGraph.getRelation(k).alias << "\"" << fw::checkQuotes(true) << fw::endl();
         if (vPaper.isSubsetNotEqualOf(vNaive)) {
            out << "detected real subset" << fw::endl();
         }
      }
   }
   for (auto k : interesting_nodes) {
      assert(paperPred.contains(k));
      assert(naivePred.contains(k));
      auto vPaper = paperPred[k];
      auto vNaive = naivePred[k];
      if (vNaive != vPaper) {
         out << "difference for interesting relation " << queryGraph.getRelation(k).alias << fw::endl();
      }
   }
   for (auto& jc : join.getJoinCondition()) {
      interesting_nodes.insert(queryGraph.getRelation(&jc.get_left()));
      interesting_nodes.insert(queryGraph.getRelation(&jc.get_right()));
   }
   join.getLeftChild()->produce(*this);
   join.getRightChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------