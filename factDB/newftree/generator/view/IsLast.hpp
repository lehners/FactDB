#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct IsLastView : public FIteratorGenerator {
   std::unordered_set<size_t> availableNodes;
   std::unordered_set<size_t> variableNodes;

   IsLastView(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter), availableNodes(iterator.getOwner().collectComposedAvailableNodes()) {}

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void IsLastView::visitRoot(const FNodeOwning& node) {
   out << "bool isLast() const { return true" << fw::pushSeparator(" && ", false, false);
   visit(node);
   out << "; }" << fw::endl() << fw::popSeparator();
}
// ---------------------------------------------------------------------------------------------------
inline void IsLastView::visit(const FNodeOwning& node) {
   if (!availableNodes.contains(node.getNodeId())) {
      assert(isNodeRequired(node));
      out << fw::separator() << FTreeUtil(node).iteratorVar() << ".isLast()";
   }
   recurseRequiredChildren(node);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
