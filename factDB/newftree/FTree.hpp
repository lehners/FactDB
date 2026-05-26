#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/JoinMode.hpp"
#include "factDB/newftree/FIterator.hpp"
#include "factDB/newftree/FNode.hpp"
#include <list>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
namespace algebra {
class TableScan;
} // namespace algebra
// ---------------------------------------------------------------------------------------------------
class FTree {
   friend class FIterator;
   /// the root node
   FNodeOwning rootNode;
   /// the iterators that are used to access the elements of the tree
   std::list<FIterator> iterators = {};

   /// reference to the merged tree
   FTree* insertedTree = nullptr;

   const FIterator* topIterator = nullptr;
   const FIterator* lowerIterator = nullptr;

   const algebra::TableScan* tableScan = nullptr;
   bool composedTree = false;
   bool generateInsertions = true;

   private:
   explicit FTree(FNodeOwning node) : rootNode(std::move(node)) {}

   public:
   FTree() {}
   FTree(const FTree&) = delete;
   FTree(FTree&& other) = default;
   ~FTree() = default;

   const FNode& getRootNode() const { return rootNode; }
   FIterator& createIterator(const OrderedIUSet& iteratedIUs);
   FIterator& createSubiterator(const FIterator& iterator, const OrderedIUSet& iteratedIUs);

   bool wasMerged() const { return topIterator != nullptr || lowerIterator != nullptr; }
   const FTree* getInsertedTree() const { return insertedTree; }
   const FIterator* getTopIterator() const { return topIterator; }
   const FIterator* getLowerIterator() const { return lowerIterator; }
   const FTree* getTopTree() const { return topIterator == nullptr ? nullptr : &topIterator->getOwner(); }
   const FTree* getLowerTree() const { return lowerIterator == nullptr ? nullptr : &lowerIterator->getOwner(); }
   const FIterator* getMergingIterator() const;
   const algebra::TableScan* getTableScan() const { return tableScan; }
   const std::list<FIterator>& getIterators() const { return iterators; }
   std::list<FIterator>& getIteratorsNC() { return iterators; }

   const FNode* getLastMergedNode() const;
   const FNode* getInsertedNode() const { return getLastMergedNode()->getChildren().back().get(); }

   bool operator==(const FTree& other) const { return rootNode == other.rootNode; }

   void print(FileWriter& out = fw::silentCout, bool printNodeId_ = true) const { rootNode.print(out, printNodeId_); }
   void printGeneration(FileWriter& out = fw::silentCout) const;
   bool containsIU(const IU& iu) const;
   IUSet collectIUs() const;
   bool isComposedTree() const { return composedTree; }

   static void createRootToLeafPath(const FIterator& iterator, FNode& newRoot);
   static void mergeTrees(FTree& mergedTree, const FIterator& leftIterator, const FIterator& rightIterator, algebra::JoinMode mode);
   static FTree fromNodes(FNodeOwning nodes);
   void setTableScan(const algebra::TableScan& ts) { tableScan = &ts; }
   void setComposedTree();
   IUSet collectComposedAvailableIUs() const;
   std::unordered_set<size_t> collectComposedAvailableNodes() const;
   std::unordered_set<size_t> collectUpperTreeNodes() const;
   std::unordered_set<size_t> collectLowerTreeNodes() const;

   private:
   static void createRootToLeafPathNaive(const FIterator& iterator, FNode& newRoot);
   static void createRootToLeafPathPaper(const FIterator& iterator, FNode& newRoot);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------