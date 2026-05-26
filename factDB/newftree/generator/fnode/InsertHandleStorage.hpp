#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct InsertHandleStorage : public FNodeGenerator {
   const FTree& currentTree;
   IUSet previousIUs;
   DeriveBeforeMergeMap::IUMap iu2beforeNodes;

   InsertHandleStorage(FileWriter& o, const FTree& mergedFTree)
      : FNodeGenerator(o), currentTree(mergedFTree), iu2beforeNodes(DeriveBeforeMergeMap::getChildTreesMap(mergedFTree).first) {}

   void visitRoot(const FNodeOwning&) override;
   void visit(const FNodeOwning&) override;
   void visit(const FNodeReferencing&) override;
};
// ---------------------------------------------------------------------------------------------------
// Since the handle is only owned by us and nobody can access it, it should be safe to use single-threaded containers.
inline void InsertHandleStorage::visitRoot(const FNodeOwning& /*outputRoot*/) {
   out << "struct InsertHandleStorage {" << fw::endl();
   apply(*currentTree.getInsertedNode());
   out << "};" << fw::endl()
       << "static InsertHandleStorage createHandleStorage() { return {};}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void InsertHandleStorage::visit(const FNodeOwning& node) {
   IUSet currentIUs = previousIUs;
   std::unordered_set<size_t> seenNodes;
   for (auto& iu : node.getIUs()) {
      assert(iu2beforeNodes.contains(iu));
      auto nodeId = iu2beforeNodes.find(iu)->second.node.getNodeId();
      if (seenNodes.contains(nodeId))
         continue;
      seenNodes.insert(nodeId);
      currentIUs.insert(iu);
   }

   auto iuIter = fw::iter(currentIUs | std::views::transform([](const IU* iu) { return fw::fmt("const {}*", iu->type); }));
   out << fw::fmt("std::unordered_map<std::tuple<{}>, {}::{}::Iterator, HashTuple> handle{}map;", iuIter, FTreeTranslator(currentTree).classname(), FTreeUtil(node).listType(), node.getNodeId()) << fw::endl();
   TemporarySetter ts(previousIUs, currentIUs);
   recurseChildren(node);
}
// ---------------------------------------------------------------------------------------------------
inline void InsertHandleStorage::visit(const FNodeReferencing& /*node*/) {
   // reference nodes are not cached.
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
