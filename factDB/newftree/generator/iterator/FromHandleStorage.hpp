#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct FromHandleStorage : public FIteratorGenerator {
   FWContainer currentList = fw::nop();
   std::unordered_set<size_t> nodeIdsInHandleStorage;

   FromHandleStorage(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter) {};

   void visit(const FNodeOwning&) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void FromHandleStorage::visitRoot(const FNodeOwning& node) {
   if (iterator.hasReferencedIterator())
      return;

   const FNode* curNode = &node;
   while (curNode != nullptr) {
      nodeIdsInHandleStorage.insert(curNode->getNodeId());
      curNode = curNode->getMergedNode();
   }

   if (!isNodeRequired(node) && node != iterator.getOwner().getRootNode().getOwningNode())
      return;
   out << fw::fmt("static {} fromHandleStorage(const HandleStorage& hs) {{", FTreeUtil::getIteratorName(iterator)) << fw::endl()
       << fw::fmt("{} iter;", FTreeUtil::getIteratorName(iterator)) << fw::endl();

   visit(node);

   out << fw::fmt("  return iter;") << fw::endl()
       << fw::fmt("}}") << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void FromHandleStorage::visit(const FNodeOwning& node) {
   if (nodeIdsInHandleStorage.contains(node.getNodeId())) {
      out << fw::fmt("iter.{} = hs.handle{};", FTreeUtil(node).iteratorVar(), node.getNodeId()) << fw::endl();
   } else {
      assert(!currentList.isNOP());
      out << fw::fmt("iter.{} = {}.begin();", FTreeUtil(node).iteratorVar(), currentList) << fw::endl();
   }

   for (auto& child : node.getChildren()) {
      if (isNodeRequired(*child) && node.type != FNode::Reference) {
         TemporarySetter ts(currentList, fw::fmt("std::get<{}>(*iter.{})", FTreeUtil(node).getChildIdx(*child), FTreeUtil(node).iteratorVar()));
         apply(*child);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
