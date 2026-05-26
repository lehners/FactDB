#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/fnode/PushBack.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include <cassert>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct DereferenceReferences : public FIteratorGenerator {
   FTreeTranslator upper;
   FTreeTranslator lower;

   IUSet upperIUs;
   std::unordered_set<size_t> availableNodes;

   DereferenceReferences(FileWriter& o, const FIterator& iter)
      : FIteratorGenerator(o, iter), upper(*iter.getOwner().getTopTree(), "upper"), lower(*iter.getOwner().getLowerTree(), "lower"), upperIUs(upper.getFTree().collectIUs()), availableNodes(iterator.getOwner().collectComposedAvailableNodes()) {
   }

   void visit(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void DereferenceReferences::visit(const FNodeOwning& node) {
   out << fw::pushSeparator("else ")
       << fw::fmt("template <{} Attribute> auto& get() const {{", FTreePushBack::getPushbackEnum(node)) << fw::endl();
   bool isUpperTree = upperIUs.contains(node.getIUs().front());
   if (availableNodes.contains(node.getNodeId())) {
      for (const auto* iu : node.getIUs()) {
         assert(isUpperTree == upperIUs.contains(iu));
         auto& curFTree = isUpperTree ? upper : lower;
         out << fw::separator() << fw::fmt("if constexpr (Attribute == {}) return {}.get<{}::{}>();", FTreePushBack::getPushbackEnumValue(node, *iu), curFTree.name(), curFTree.classname(), iu) << fw::endl();
      }
   } else if (isNodeRequired(node)) {
      // a new getter has to be generated since we are the first to iterate over this node
      for (const IU* iu : node.getIUs()) {
         out << fw::separator() << fw::fmt("if constexpr (Attribute == {}) return std::get<{}>(*{});", FTreePushBack::getPushbackEnumValue(node, *iu), FTreeUtil(node).getIUIdx(*iu), FTreeUtil(node).iteratorVar()) << fw::endl();
      }
   }
   out << "}" << fw::endl() << fw::popSeparator();

   for (auto& child : node.getChildren()) {
      if (availableNodes.contains(child->getNodeId()) || isNodeRequired(*child)) {
         apply(*child);
      } else if (availableNodes.contains(node.getNodeId())) {
         const IU* iu = child->getIUs().front();
         auto& curFTree = upperIUs.contains(iu) ? upper : lower;
         out << fw::fmt("template <{} Attribute> auto& get() const {{", FTreePushBack::getPushbackEnum(*child)) << fw::endl()
             << fw::fmt("  return {}.get<{}::{}>();", curFTree.name(), curFTree.classname(), iu) << fw::endl()
             << fw::fmt("}}") << fw::endl();
      } else {
         out << fw::fmt("template <{} Attribute> auto& get() const {{", FTreePushBack::getPushbackEnum(*child)) << fw::endl()
             << fw::fmt("  return std::get<{}>(*{});", FTreeUtil(node).getChildIdx(*child), FTreeUtil(node).iteratorVar()) << fw::endl()
             << fw::fmt("}}") << fw::endl();
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
