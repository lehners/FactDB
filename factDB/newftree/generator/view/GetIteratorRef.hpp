#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct GetIteratorRef : public FIteratorGenerator {
   DeriveBeforeMergeMap::IUMap iu2beforeNodes;
   IUSet availableIUs;
   std::unordered_set<size_t> availableNodes;

   GetIteratorRef(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter) {
      auto res = DeriveBeforeMergeMap::getChildTreesMap(iter.getOwner());
      iu2beforeNodes = std::move(res.first);
      availableIUs = iterator.getOwner().collectComposedAvailableIUs();
      availableNodes = iterator.getOwner().collectComposedAvailableNodes();
   }
   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;

   void generateIterator(const FNodeOwning& node) const;
};
// ---------------------------------------------------------------------------------------------------
inline void GetIteratorRef::generateIterator(const FNodeOwning& node) const {
   if (isNodeRequired(node) && !availableNodes.contains(node.getNodeId())) { // newly created iterator, just return it as is
      out << fw::fmt("auto& get{0}Ref() const {{ return {0}; }}", FTreeUtil(node).iteratorVar()) << fw::endl();
   } else if (!availableNodes.contains(node.getNodeId())) {
      std::cout << "some IUs not available: " << node.getIUs().front() << " in node " << node.getNodeId() << std::endl;
   } else {
      std::set<const FNode*> involvedNodes;
      const FTree* tree = nullptr;
      // detect participating nodes first
      for (const auto* iu : node.getIUs()) {
         assert(iu2beforeNodes.contains(iu));
         const auto& res = iu2beforeNodes.find(iu)->second;
         involvedNodes.insert(&res.node);
         assert(tree == nullptr || tree == &res.tree);
         tree = &res.tree;
      }
      assert(tree != nullptr);
      auto treeTranslator = FTreeTranslator(*tree, tree == iterator.getOwner().getTopTree() ? "upper" : "lower");
      auto listIterator = fw::iter(involvedNodes | std::views::transform([&](const FNode* n) { return treeTranslator.getIteratorRef(n); }));
      if (involvedNodes.size() == 1) {
         out << fw::fmt("auto get{}Ref() const {{ return {}; }}", FTreeUtil(node).iteratorVar(), listIterator) << fw::endl();
      } else {
         out << fw::fmt("auto get{}Ref() const {{ return std::make_tuple({}); }}", FTreeUtil(node).iteratorVar(), listIterator) << fw::endl();
      }
   }
}
// ---------------------------------------------------------------------------------------------------
inline void GetIteratorRef::visitRoot(const FNodeOwning& node) {
   generateIterator(node);
   recurseChildren(node);
}
// ---------------------------------------------------------------------------------------------------
inline void GetIteratorRef::visit(const FNodeOwning& node) {
   generateIterator(node);
   recurseChildren(node);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
