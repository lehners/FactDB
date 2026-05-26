#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/util/ranges.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct Increment : public FIteratorGenerator {
   std::unordered_set<size_t> fixedNodes;
   std::unordered_set<size_t> variableNodes;
   std::unordered_set<size_t> containedNodes;
   struct QueueHelper {
      const FNode* parent;
      const FNode* child;
   };
   std::stack<QueueHelper> todos;

   Increment(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter) {}

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;

   void generateCode();
};
// ---------------------------------------------------------------------------------------------------
inline void Increment::visitRoot(const FNodeOwning& node) {
   containedNodes = inIteratorContainedIDs();
   if (iterator.hasReferencedIterator()) {
      fixedNodes = iterator.getReferencedIterator().getTransitiveRequiredNodeIDs();
      if (fixedNodes.empty()) // handle case where only root node is fixed
         fixedNodes.insert(iterator.getOwner().getRootNode().getNodeId());
   }
   for (auto& nodeId : iterator.getRequiredNodeIDs()) {
      if (!fixedNodes.contains(nodeId)) {
         assert(containedNodes.contains(nodeId));
         variableNodes.insert(nodeId);
      }
   }

   out << fw::fmt("{}& operator++() {{", FTreeUtil::getIteratorName(iterator)) << fw::endl();

   if (variableNodes.empty()) {
      for (const auto& f : containedNodes) {
         out << fw::fmt("iter{}.setEnd();", f) << fw::endl();
      }
      out << "assert(isLast());" << fw::endl();
   } else {
      if (variableNodes.contains(iterator.getOwner().getRootNode().getNodeId()))
         todos.emplace(nullptr, &iterator.getOwner().getRootNode());
      visit(node);
      generateCode();
   }
   out << "return *this;" << fw::endl()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void Increment::generateCode() {
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
      for (const auto& f : containedNodes) {
         out << fw::fmt("iter{}.setEnd();", f) << fw::endl();
      }
      out << "assert(isLast());" << fw::endl()
          << "return *this;" << fw::endl();
      // do not increment this node since it is the root node
   } else {
      assert(cur.parent != nullptr);
      // todo: where do we get the value from?
      out << fw::fmt("{} = std::get<{}>(*{}).begin();", childUtil.iteratorVar(), FTreeUtil(*cur.parent).getChildIdx(*cur.child), FTreeUtil(*cur.parent).iteratorVar()) << fw::endl();
   }
   out << "}" << fw::endl()
       << fw::fmt("assert(!{}.isLast());", childUtil.iteratorVar()) << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void Increment::visit(const FNodeOwning& node) {
   // generate stack of necessary nodes for increment in inverse order
   for (auto& child : node.getChildren()) {
      if (isNodeRequired(*child)) { // for some reason we must not remove this line since the code will break otherwise for query epinions_cyclic_q9_37
         // maybe because variable nodes does not contain fixed nodes...
         if (variableNodes.contains(child->getNodeId()))
            todos.emplace(&node, child.get());
         apply(*child);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
