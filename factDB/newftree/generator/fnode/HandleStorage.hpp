#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/iterator/IteratorVars.hpp"
#include "factDB/newftree/generator/util/FGenUtil.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct HandleStorage : public FIteratorGenerator {
   FTreeTranslator mergedFTreeTranslator;
   DeriveBeforeMergeMap::IUMap iu2beforeNodes;
   bool doIterators;

   HandleStorage(FileWriter& o, const FTree& mergedFTree)
      : FIteratorGenerator(o, *mergedFTree.getTopIterator()), mergedFTreeTranslator(mergedFTree, "container"), iu2beforeNodes(DeriveBeforeMergeMap::getChildTreesMap(mergedFTree).first) {};

   void visit(const FNodeOwning& mergedFTreeNode) override;
   void visit(const FNodeReferencing& node) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void HandleStorage::visitRoot(const FNodeOwning& afterInsertTreeNode) {
   out << "struct HandleStorage {" << fw::endl();
   assert(mergedFTreeTranslator.getFTree().wasMerged());
   auto& upperTree = *mergedFTreeTranslator.getFTree().getTopTree();

   doIterators = true;
   if (upperTree.isComposedTree()) {
      visit(upperTree.getRootNode().getOwningNode());
   } else {
      IteratorVars iterVars(out, iterator, FTreeTranslator(upperTree).classname());
      iterVars.visitRoot(upperTree.getRootNode().getOwningNode());
      // todo fix case where references occur => required if in generated tree required
      out << fw::endl();
   }

   doIterators = false;
   visit(afterInsertTreeNode);
   out << "};" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void HandleStorage::visit(const FNodeOwning& afterInsertFNode) {
   if (doIterators) {
      std::unordered_set<const FNode*> seenIds = {};
      for (const IU* iu : afterInsertFNode.getIUs()) { // collect all required iterators in map to eliminate iterators with multiple ius
         assert(iu2beforeNodes.contains(iu));
         seenIds.insert(&iu2beforeNodes.find(iu)->second.node);
      }

      for (const FNode* node : seenIds)
         out << fw::fmt("{}::{} {} = {{}};", FTreeTranslator(*mergedFTreeTranslator.getFTree().getTopTree()).classname(), FTreeUtil(*node).iteratorType(), FTreeUtil(*node).iteratorVar()) << fw::endl();
      for (auto& child : afterInsertFNode.getChildren()) {
         apply(*child);
      }
   } else {
      if (mergedFTreeTranslator.getFTree().getLowerTree()->containsIU(*afterInsertFNode.getIUs().front()))
         return;
      out << fw::fmt("{}::{}::Iterator handle{};", mergedFTreeTranslator.classname(), FTreeUtil(afterInsertFNode).listType(), afterInsertFNode.getNodeId()) << fw::endl();
      for (auto& child : afterInsertFNode.getChildren()) {
         apply(*child);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
inline void HandleStorage::visit(const FNodeReferencing& node) {
   if (doIterators) { // here for legacy reasons, I was to lazy to check if this is really required
      visit(node.getOwningNode());
   } // else iterator cache is not required.
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------