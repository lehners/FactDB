#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/list/ListTypeEnum.hpp"
#include "factDB/newftree/generator/util/DeriveBeforeMergeMap.hpp"
#include "factDB/newftree/generator/util/FGenUtil.hpp"
#include "factDB/newftree/generator/util/FNodeGenerator.hpp"
#include <ranges>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct ListReferences : public FNodeGenerator {
   DeriveBeforeMergeMap::IUMap iu2beforeNodes;
   std::unordered_set<size_t> availableNodes;

   explicit ListReferences(FileWriter& o, const FTree& mergedFTree) : FNodeGenerator(o), availableNodes(mergedFTree.collectComposedAvailableNodes()) {
      auto res = DeriveBeforeMergeMap::getChildTreesMap(mergedFTree);
      iu2beforeNodes = std::move(res.first);
   }

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;

   private:
   void genTypeString(const FNode& node);
};
// ---------------------------------------------------------------------------------------------------
inline void ListReferences::genTypeString(const FNode& node) {
   assert(!node.getIUs().empty());

   std::set<const FNode*> involvedNodes;
   const FTree* tree = nullptr;
   for (const auto* iu : node.getIUs()) {
      assert(iu2beforeNodes.contains(iu));
      const auto& res = iu2beforeNodes.find(iu)->second;
      involvedNodes.insert(&res.node);
      assert(tree == nullptr || tree == &res.tree);
      tree = &res.tree;
   }
   assert(tree != nullptr);

   auto treeTranslator = FTreeTranslator(*tree);
   if (availableNodes.contains(node.getNodeId())) {
      auto listIterator = fw::iter(involvedNodes | std::views::transform([&](const FNode* cur) { return fw::fmt("{}::{}::Iterator", treeTranslator.classname(), FTreeUtil(*cur).listType()); }));
      if (involvedNodes.size() == 1)
         out << fw::fmt("using {} = IteratorWrapper<{}>;", FTreeUtil(node).listType(), listIterator) << fw::endl();
      else
         out << fw::fmt("using {} = IteratorWrapper<std::tuple<{}>>;", FTreeUtil(node).listType(), listIterator) << fw::endl();
   } else {
      out << fw::fmt("using {0} = {1}::{0};", FTreeUtil(node).listType(), treeTranslator.classname()) << fw::endl();
   }
}
// ---------------------------------------------------------------------------------------------------
inline void ListReferences::visitRoot(const FNodeOwning& node) {
   recurseChildren(node);
   genTypeString(node);
}
// ---------------------------------------------------------------------------------------------------
inline void ListReferences::visit(const FNodeOwning& node) {
   recurseChildren(node);
   genTypeString(node);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
