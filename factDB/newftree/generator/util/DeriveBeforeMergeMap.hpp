#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct DeriveBeforeMergeMap : public FNodeGenerator {
   struct ParentInfo {
      const FNode* parent;
      size_t childIdx;
   };
   struct NodeInfo {
      const FTree& tree;
      const FNode& node;
   };

   using IUMap = std::unordered_map<const IU*, NodeInfo, IUPointerHash, IUPointerEqual>;
   using ParentMap = std::unordered_map<size_t, ParentInfo>; // map  a node the index of is parent and the index in this node.

   private:
   ParentMap node2parent;
   IUMap iu2node;

   const FTree& curTree;

   DeriveBeforeMergeMap(const FTree& tree) : FNodeGenerator(fw::nullStream), curTree(tree) {}

   void visitRoot(const FNodeOwning& node) override { visit(node); }
   void visit(const FNodeOwning& node) override;
   void mergeMaps(const DeriveBeforeMergeMap& other);

   public:
   static std::pair<IUMap, ParentMap> getChildTreesMap(const FTree& tree);
};
// ---------------------------------------------------------------------------------------------------
inline void DeriveBeforeMergeMap::visit(const FNodeOwning& node) {
   for (auto* iu : node.getIUs())
      iu2node.emplace(iu, NodeInfo{curTree, node});
   size_t idx = 0;
   for (const auto& child : node.getChildren()) {
      node2parent[child->getNodeId()] = {&node, idx};
      idx++;
   }
   recurseChildren(node);
}
// ---------------------------------------------------------------------------------------------------
inline void DeriveBeforeMergeMap::mergeMaps(const DeriveBeforeMergeMap& other) {
   iu2node.insert(other.iu2node.begin(), other.iu2node.end());
   node2parent.insert(other.node2parent.begin(), other.node2parent.end());
}
// ---------------------------------------------------------------------------------------------------
inline std::pair<DeriveBeforeMergeMap::IUMap, DeriveBeforeMergeMap::ParentMap> DeriveBeforeMergeMap::getChildTreesMap(const FTree& tree) {
   DeriveBeforeMergeMap d1(*tree.getTopTree());
   d1.apply(tree.getTopTree()->getRootNode());

   DeriveBeforeMergeMap d2(*tree.getLowerTree());
   d2.apply(tree.getLowerTree()->getRootNode());

   d1.mergeMaps(d2);

   return {d1.iu2node, d1.node2parent};
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB