#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/util/ranges.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct ConstructorView : public FIteratorGenerator {
   std::unordered_set<size_t> fixedNodes;
   std::unordered_set<size_t> variableNodes;

   ConstructorView(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter) {}

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void ConstructorView::visitRoot(const FNodeOwning& node) {
   fixedNodes = iterator.getOwner().collectComposedAvailableNodes();
   for (auto& nodeId : iterator.getRequiredNodeIDs()) {
      if (!fixedNodes.contains(nodeId)) {
         variableNodes.insert(nodeId);
      }
   }

   out << fw::fmt("{}(const {}& upper_, const {}& lower_)", FTreeUtil::getIteratorName(iterator), FTreeUtil::getQualifiedIteratorName(*iterator.getOwner().getTopIterator()), FTreeUtil::getQualifiedIteratorName(*iterator.getOwner().getLowerIterator())) << fw::endl()
       << fw::fmt(" : upper(upper_), lower(lower_)");
   if (!variableNodes.empty()) {
      visit(node);
   }
   out << " {}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void ConstructorView::visit(const FNodeOwning& node) {
   // generate stack of necessary nodes for increment in inverse order
   for (auto& child : node.getChildren()) {
      if (!isNodeRequired(*child))
         continue;
      if (variableNodes.contains(child->getNodeId())) { // we have to create an entry
         FTreeUtil childUtil(*child);
         if (fixedNodes.contains(node.getNodeId())) { // first node, switch from reference iterator to this iterator
            auto isFromTop = iterator.getOwner().getTopTree()->containsIU(*child->getIUs().front());
            auto* tree = isFromTop ? iterator.getOwner().getTopTree() : iterator.getOwner().getLowerTree();
            auto treeTranslator = FTreeTranslator(*tree, isFromTop ? "upper" : "lower");

            out << fw::fmt(", {}({}.get<{}>().begin())", childUtil.iteratorVar(), treeTranslator.name(), treeTranslator.iuEnum(*child->getIUs().front()));
         } else {
            out << fw::fmt(", {}(std::get<{}>(*{}).begin())", childUtil.iteratorVar(), FTreeUtil(node).getChildIdx(*child), FTreeUtil(node).iteratorVar());
         }
      }
      apply(*child);
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
