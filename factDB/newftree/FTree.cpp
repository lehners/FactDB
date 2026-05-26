#include "factDB/newftree/FTree.hpp"
#include "factDB/infra/util/ranges.hpp"
#include "factDB/newftree/generator/util/FGenUtil.hpp"
#include <stack>
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
FIterator& FTree::createIterator(const OrderedIUSet& ius) {
   return iterators.emplace_back(iterators.size(), ius, *this);
}
// ---------------------------------------------------------------------------------------------------
FIterator& FTree::createSubiterator(const factDB::FIterator& iterator, const factDB::OrderedIUSet& iteratedIUs) {
   auto& iter = iterators.emplace_back(iterators.size(), iteratedIUs, *this);
   iter.setReferencedIterator(iterator);
   return iter;
}
// ---------------------------------------------------------------------------------------------------
// naive approach to generate root to leaf path from paper
void FTree::createRootToLeafPathNaive(const FIterator& iterator, FNode& newRoot) {
   const auto& tree = iterator.getOwner();
   newRoot.addIUs(tree.rootNode.getIUs());
   auto composedNodes = tree.collectComposedAvailableNodes();
   for (auto req : iterator.getTransitiveRequiredNodeIDs())
      composedNodes.insert(req);

   std::stack<FNode*> todos;
   for (auto& child : tree.rootNode.getChildren() | views::reverse) {
      todos.push(child.get());
   }
   while (!todos.empty()) {
      auto& cur = *todos.top();
      todos.pop();

      if (cur.isRequired(iterator) || cur.isRequiredByReferenceIterator(iterator)) {
         newRoot.addIUs(cur.getIUs());

         for (auto& child : cur.getChildren() | views::reverse)
            todos.emplace(child.get());
      } else {
         newRoot.addChild(cur, !composedNodes.contains(cur.getNodeId()));
      }
   }
}
// ---------------------------------------------------------------------------------------------------
// create root to leaf path as described in the pseudocode in the paper
void FTree::createRootToLeafPathPaper(const FIterator& iterator, FNode& newRoot) {
   const auto& tree = iterator.getOwner();
   newRoot.addIUs(tree.rootNode.getIUs());
   auto composedNodes = tree.collectComposedAvailableNodes();
   for (auto req : iterator.getTransitiveRequiredNodeIDs())
      composedNodes.insert(req);

   struct ParentInfo {
      FNode* newParent;
      FNode& child;
   };
   std::stack<ParentInfo> todos;
   for (auto& child : tree.rootNode.getChildren() | views::reverse) {
      todos.push({&newRoot, *child});
   }
   while (!todos.empty()) {
      auto [newParent, cur] = todos.top();
      todos.pop();

      if (cur.isRequired(iterator) || cur.isRequiredByReferenceIterator(iterator)) {
         assert(composedNodes.contains(cur.getNodeId()));
         auto& newNode = newParent->addToMergedNode(cur);

         for (auto& child : cur.getChildren() | views::reverse)
            todos.emplace(&newNode, *child);
      } else {
         newParent->addChild(cur, !composedNodes.contains(cur.getNodeId()));
      }
   }
}
// ---------------------------------------------------------------------------------------------------
// select method to generate root to leaf path
void FTree::createRootToLeafPath(const FIterator& iterator, FNode& newRoot) {
   if (SettingBase::getSetting<bool>("codegen.factorized.naiveMerge")->get()) {
      createRootToLeafPathNaive(iterator, newRoot);
   } else {
      createRootToLeafPathPaper(iterator, newRoot);
   }
}
// ---------------------------------------------------------------------------------------------------
// merge two f-trees: the merged tree is stored in mergedTree. The Iterators define the required nodes
void FTree::mergeTrees(FTree& mergedTree, const FIterator& iteratorLeft, const FIterator& iteratorRight, algebra::JoinMode mode) {
   assert(mergedTree.getRootNode().getIUs().empty() && mergedTree.getRootNode().getChildren().empty() && "merged tree must be empty at beginning");

   auto& upperIterator = (mode == algebra::JoinMode::BottomInsert) ? iteratorLeft : iteratorRight;
   auto& lowerIterator = (mode == algebra::JoinMode::BottomInsert) ? iteratorRight : iteratorLeft;

   createRootToLeafPath(upperIterator, mergedTree.rootNode);
   FNode* insertionPos = &mergedTree.rootNode;
   while (insertionPos->getMergedNode()) {
      insertionPos = insertionPos->getMergedNode();
   }

   auto& insertedNode = insertionPos->addChild();
   createRootToLeafPath(lowerIterator, insertedNode);
   if (mode == algebra::JoinMode::BottomInsert) // locks only needed for BI
      insertedNode.getOwningNode().setRequiresLock();
   assert(insertionPos->getMergedNode() == nullptr);

   // link the trees
   mergedTree.topIterator = &upperIterator;
   mergedTree.lowerIterator = &lowerIterator;

   upperIterator.getOwnerWriteable().insertedTree = &mergedTree;
   lowerIterator.getOwnerWriteable().insertedTree = &mergedTree;
   // mergedTree is constructed
}
// ---------------------------------------------------------------------------------------------------
FTree FTree::fromNodes(FNodeOwning nodes) {
   return FTree(std::move(nodes));
}
// ---------------------------------------------------------------------------------------------------
// utility function to print the current tree and all its predecessors in Graphviz
void FTree::printGeneration(FileWriter& out) const {
   std::stack<std::pair<const FTree*, const FTree*>> todos;
   todos.emplace(this, nullptr);

   out << "digraph Trees {" << fw::endl()
       << "node [shape=record, fontsize=10];" << fw::endl()
       << fw::endl();

   while (!todos.empty()) {
      auto [tree, subsequentTree] = todos.top();
      todos.pop();
      if (tree->wasMerged()) {
         todos.emplace(tree->getTopTree(), tree);
         todos.emplace(tree->getLowerTree(), tree);
      }

      auto cn = FTreeTranslator(*tree).classname();
      tree->getRootNode().print(out, true, cn);

      // draw the line to its predecessor (if any)
      if (subsequentTree != nullptr) {
         out << fw::fmt("anchor_{0} -> anchor_{1} [ltail=cluster_{0}, lhead=cluster_{1}];", cn, FTreeTranslator(*subsequentTree).classname()) << fw::endl();
      }
   }

   out << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
bool FTree::containsIU(const IU& iu) const {
   std::stack<const FNode*> todos;
   todos.emplace(&rootNode);
   while (!todos.empty()) {
      auto* top = todos.top();
      if (top->containsIU(iu))
         return true;
      todos.pop();
      for (auto& child : top->getChildren())
         todos.emplace(child.get());
   }
   return false;
}
// ---------------------------------------------------------------------------------------------------
// collect all IUs hold by the f-representation
IUSet FTree::collectIUs() const {
   IUSet ius;
   std::stack<const FNode*> todos;
   todos.emplace(&rootNode);
   while (!todos.empty()) {
      auto* top = todos.top();
      todos.pop();
      ius = ius.merge(top->getIUs());
      for (auto& child : top->getChildren())
         todos.emplace(child.get());
   }
   return ius;
}
// ---------------------------------------------------------------------------------------------------
const FNode* FTree::getLastMergedNode() const {
   assert(wasMerged() && "UB if nothing inserted");
   const FNode* insertNode = &getRootNode();
   while (insertNode->getMergedNode() != nullptr)
      insertNode = insertNode->getMergedNode();
   return insertNode;
}
// ---------------------------------------------------------------------------------------------------
const FIterator* FTree::getMergingIterator() const {
   if (insertedTree == nullptr)
      return nullptr;
   assert(insertedTree->topIterator != nullptr && insertedTree->lowerIterator != nullptr);
   if (&insertedTree->topIterator->getOwner() == this)
      return insertedTree->topIterator;
   else
      return insertedTree->lowerIterator;
}
// ---------------------------------------------------------------------------------------------------
void FTree::setComposedTree() {
   composedTree = true;
}
// ---------------------------------------------------------------------------------------------------
IUSet FTree::collectComposedAvailableIUs() const {
   auto collectRequiredIUs = [](const FIterator& iterator) {
      auto requiredNodes = iterator.getTransitiveRequiredNodeIDs();
      IUSet ius;
      std::stack<const FNode*> todos;
      todos.emplace(&iterator.getOwner().getRootNode());
      while (!todos.empty()) {
         const FNode* curNode = todos.top();
         todos.pop();
         if (requiredNodes.contains(curNode->getNodeId())) {
            ius += curNode->getIUs();
         }
         for (auto& c : curNode->getChildren())
            todos.emplace(c.get());
      }
      return ius;
   };

   if (isComposedTree() && wasMerged()) {
      auto result = collectRequiredIUs(*getTopIterator());
      result += collectRequiredIUs(*getLowerIterator());
      result += getTopTree()->collectComposedAvailableIUs();
      result += getLowerTree()->collectComposedAvailableIUs();
      return result;
   } else {
      return {};
   }
}
// ---------------------------------------------------------------------------------------------------
std::unordered_set<size_t> FTree::collectComposedAvailableNodes() const {
   // we have to do it like this because nodes may have been merged and we do not know which excatly
   auto availableIUs = collectComposedAvailableIUs();
   std::unordered_set<size_t> availableNodes;
   std::stack<const FNode*> todos;
   todos.emplace(&rootNode);
   while (!todos.empty()) {
      const FNode* curNode = todos.top();
      todos.pop();
      if (availableIUs.contains(curNode->getIUs().front())) {
         availableNodes.insert(curNode->getNodeId());
      }
      for (auto& c : curNode->getChildren())
         todos.emplace(c.get());
   }
   return availableNodes;
}
// ---------------------------------------------------------------------------------------------------
std::unordered_set<size_t> FTree::collectUpperTreeNodes() const {
   std::unordered_set<size_t> upperTreeNodes;
   std::stack<std::tuple<bool, const FNode*>> todos;
   todos.emplace(false, &rootNode);

   while (!todos.empty()) {
      auto [recurseAll, node] = todos.top();
      todos.pop();
      upperTreeNodes.insert(node->getNodeId());
      if (recurseAll) {
         for (auto& c : node->getChildren())
            todos.emplace(true, c.get());
      } else if (node->getMergedNode() != nullptr) {
         for (auto& c : node->getChildren())
            todos.emplace(node->getMergedNode() != c.get(), c.get());
      } else { // one child has to be from lower tree, we have to figure out which:
         const FTree& lowerTree = *getLowerTree();
         [[maybe_unused]] bool foundLowerChild = false;
         for (auto& c : node->getChildren()) {
            if (!lowerTree.getRootNode().getIUs().contains(c->getIUs().front()))
               todos.emplace(node->getMergedNode() != c.get(), c.get());
            else
               foundLowerChild = true;
         }
         assert(foundLowerChild);
      }
   }
   return upperTreeNodes;
}
// ---------------------------------------------------------------------------------------------------
std::unordered_set<size_t> FTree::collectLowerTreeNodes() const {
   std::unordered_set<size_t> lowerTreeNodes;
   std::stack<const FNode*> todos;
   todos.emplace(getInsertedNode());
   while (!todos.empty()) {
      const auto& cur = *todos.top();
      todos.pop();
      lowerTreeNodes.insert(cur.getNodeId());
      for (auto& c : cur.getChildren())
         todos.push(c.get());
   }

   return lowerTreeNodes;
}
// ---------------------------------------------------------------------------------------------------
