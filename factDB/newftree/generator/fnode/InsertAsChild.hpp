#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree//generator/util/FIteratorGenerator.hpp"
#include <ranges>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct InsertAsChild : public FNodeGenerator {
   FTreeTranslator mergedFTreeTranslator;
   FTreeTranslator beforeTreeTranslator;
   DeriveBeforeMergeMap::IUMap iu2beforeNodes;
   FHandle parentHandle{"parentHandle", 0};
   IUSet currentIUsParent = {};
   const FNodeOwning* parentNode = nullptr;
   bool doCaching;

   InsertAsChild(FileWriter& o, const FTree& mergedFTree, bool caching)
      : FNodeGenerator(o), mergedFTreeTranslator(mergedFTree, "container"), beforeTreeTranslator(*mergedFTree.getLowerTree()), iu2beforeNodes(DeriveBeforeMergeMap::getChildTreesMap(mergedFTree).first), doCaching(caching) {}

   void visit(const FNodeOwning&) override;
   void visit(const FNodeReferencing&) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
// Since the handle is only owned by us and nobody can access it, it should be safe to use single-threaded containers.
inline void InsertAsChild::visitRoot(const FNodeOwning& node) {
   auto handleStorage = fw::condition(doCaching, ", [[maybe_unused]] InsertHandleStorage& hs");
   out << fw::fmt("void insertAsChild({} {}, const {}& toInsert{}) {{", FTreeUtil(*mergedFTreeTranslator.getFTree().getLastMergedNode()).iteratorType(), parentHandle.gen(), mergedFTreeTranslator.getLowerIteratorTranslator().getQualifiedIteratorName(), handleStorage) << fw::endl()
       << fw::fmt("[[maybe_unused]] bool useCached{} = true;", node.getNodeId()) << fw::endl()
       << "auto& container = *this;" << fw::endl();
   TemporarySetter tsParentNode(parentNode, &node);
   apply(*mergedFTreeTranslator.getFTree().getInsertedNode());
   out << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void InsertAsChild::visit(const FNodeOwning& node) {
   FHandle newHandle("handle");
   IUSet currentIUs = currentIUsParent;
   std::unordered_set<size_t> seenNodes;
   for (auto& iu : node.getIUs()) {
      assert(iu2beforeNodes.contains(iu));
      auto nodeId = iu2beforeNodes.find(iu)->second.node.getNodeId();
      if (seenNodes.contains(nodeId))
         continue;
      seenNodes.insert(nodeId);
      currentIUs.insert(iu);
   }

   auto createReferencedChildren = [&]() {
      for (auto& child : node.getChildren()) {
         if (child->type != FNode::Reference)
            continue;
         assert(child->type == FNode::Reference);
         apply(*child);
      }
   };

   // collect the values
   for (auto& iu : node.getIUs()) {
      out << fw::fmt("auto& {} = toInsert.get<{}>();", iu, beforeTreeTranslator.iuEnum(*iu)) << fw::endl();
   }

   FNodeOwning* mergingNode = nullptr;
   for (auto& c : node.getChildren()) {
      if (c->type != FNode::Reference) {
         mergingNode = static_cast<FNodeOwning*>(c.get());
         break;
      }
   }
   if (mergingNode != nullptr || node.getMergedNode() != nullptr) { // has a required children
      out << fw::fmt("[[maybe_unused]] auto tuple{} = std::make_tuple({});", node.getNodeId(), fw::iter(currentIUs | std::views::transform([](const IU* iu) { return fw::lc("&", *iu); }), ", ")) << fw::endl();
      if (doCaching) {
         out << fw::fmt("bool useCached{0} = useCached{1} && hs.handle{0}map.contains(tuple{0});", node.getNodeId(), parentNode->getNodeId()) << fw::endl()
             << fw::fmt("if (!useCached{}) {{", node.getNodeId()) << fw::endl()
             << mergedFTreeTranslator.push_back(parentHandle, newHandle, node.getIUs()) << fw::endl()
             << fw::fmt("hs.handle{0}map[tuple{0}] = std::move({1});", node.getNodeId(), newHandle.gen()) << fw::endl();
      } else {
         out << mergedFTreeTranslator.push_back(parentHandle, newHandle, node.getIUs()) << fw::endl();
      }
      TemporarySetter tsPH(parentHandle, newHandle);
      TemporarySetter tsCurrentIUs(currentIUsParent, currentIUs);
      TemporarySetter tsParentNode(parentNode, &node);
      createReferencedChildren();

      if (doCaching)
         out << fw::fmt("}}") << fw::endl()
             << fw::fmt("auto {0} = hs.handle{1}map[tuple{1}];", newHandle.gen(), node.getNodeId()) << fw::endl();

      for (auto& c : node.getChildren()) {
         if (c->type != FNode::Reference) {
            apply(*c);
         }
      }
   } else { // has only referenced children
      out << "[[maybe_unused]] " << mergedFTreeTranslator.push_back(parentHandle, newHandle, node.getIUs()) << fw::endl();
      TemporarySetter tsPH(parentHandle, newHandle);
      TemporarySetter tsParentNode(parentNode, &node);
      createReferencedChildren();
   }
}
// ---------------------------------------------------------------------------------------------------
inline void InsertAsChild::visit(const FNodeReferencing& node) {
   out << fw::fmt("std::get<{}>(*{}).merge(toInsert.get<{}>());", FTreeUtil(*parentNode).getChildIdx(node), parentHandle.gen(), beforeTreeTranslator.iuEnum(*node.getIUs().front())) << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
