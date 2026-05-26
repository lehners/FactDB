#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/util/ranges.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
#include <queue>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct IncrementView : public FIteratorGenerator {
   std::unordered_set<size_t> fixedNodes;
   std::unordered_set<size_t> variableNodes;
   struct QueueHelper {
      const FNode* parent;
      const FNode* child;
   };
   std::stack<QueueHelper> todos;

   IncrementView(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter) {}

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;

   void generateCode();
};
// ---------------------------------------------------------------------------------------------------
inline void IncrementView::visitRoot(const FNodeOwning& node) {
   fixedNodes = iterator.getOwner().collectComposedAvailableNodes();
   for (auto& nodeId : iterator.getRequiredNodeIDs()) {
      if (!fixedNodes.contains(nodeId)) {
         variableNodes.insert(nodeId);
      }
   }

   if (!variableNodes.empty()) {
      out << fw::fmt("{}& operator++() {{", FTreeUtil::getIteratorName(iterator)) << fw::endl();
      if (variableNodes.contains(iterator.getOwner().getRootNode().getNodeId()))
         todos.emplace(nullptr, &iterator.getOwner().getRootNode());
      visit(node);
      generateCode();
      out << "return *this;" << fw::endl()
          << "}" << fw::endl();
   }
}
// ---------------------------------------------------------------------------------------------------
inline void IncrementView::generateCode() {
   if (todos.empty())
      return;
   auto cur = todos.top();
   todos.pop();

   FTreeUtil childUtil(*cur.child);

   out << fw::fmt("if (!{0}.isLast()) ++{0};", childUtil.iteratorVar()) << fw::endl()
       << fw::fmt("while ({}.isLast()) {{", childUtil.iteratorVar()) << fw::endl();

   auto isTodoEmpty = todos.empty();
   // do recursive call here
   generateCode();

   if (isTodoEmpty) {
      out << "assert(isLast());" << fw::endl()
          << "return *this;" << fw::endl();
      // do not increment this node since it is the root node
   } else {
      assert(cur.parent != nullptr);

      if (fixedNodes.contains(cur.parent->getNodeId())) {
         auto isFromTop = iterator.getOwner().getTopTree()->containsIU(*cur.child->getIUs().front());
         auto* tree = isFromTop ? iterator.getOwner().getTopTree() : iterator.getOwner().getLowerTree();
         auto treeTranslator = FTreeTranslator(*tree, isFromTop ? "upper" : "lower");
         out << fw::fmt("{} = {}.get<{}>().begin();", childUtil.iteratorVar(), treeTranslator.name(), treeTranslator.iuEnum(*cur.child->getIUs().front()), *cur.parent->getIUs().front()) << fw::endl();
      } else {
         out << fw::fmt("{} = std::get<{}>(*{}).begin();", childUtil.iteratorVar(), FTreeUtil(*cur.parent).getChildIdx(*cur.child), FTreeUtil(*cur.parent).iteratorVar()) << fw::endl();
      }
   }
   out << "}" << fw::endl()
       << fw::fmt("assert(!{}.isLast());", childUtil.iteratorVar()) << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void IncrementView::visit(const FNodeOwning& node) {
   // generate stack of necessary nodes for increment in inverse order
   for (auto& child : node.getChildren()) {
      if (isNodeRequired(*child)) {
         if (variableNodes.contains(child->getNodeId()))
            todos.emplace(&node, child.get());
         apply(*child);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
