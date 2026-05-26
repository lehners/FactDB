#include "factDB/newftree/FIterator.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/UnionFind.hpp"
#include "factDB/newftree/FTree.hpp"
#include <stack>
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
FIterator::FIterator(size_t iteratorIdP, const OrderedIUSet& iteratedIUs_, FTree& owner_) : iteratorID(iteratorIdP), iteratedIUs(iteratedIUs_), owner(owner_) {
   // find set of required nodes:
   // 1. all nodes, which contain an iterated IU
   // 2. all ancestors of such nodes

   for (auto& iu : iteratedIUs_) {
      assert(owner_.containsIU(*iu));
   }

   // traverse ftree to collect the required node IDs
   std::stack<std::pair<FNode&, bool>> todos;
   todos.emplace(owner.rootNode, false);

   auto checkIfRequired = [&](auto& curNode) {
      // check if any child is already required, i.e. union find knows it
      for (auto& child : curNode.getChildren()) {
         if (requiredNodeIDs.contains(child->getNodeId())) {
            // if so, the parent is also required
            requiredNodeIDs.insert(curNode.getNodeId());
            return true; // skip this node, as it is already required
         }
      }

      // check if the current node contains an iterated IU
      for (const auto& iu : iteratedIUs) {
         if (curNode.containsIU(iu)) {
            requiredNodeIDs.insert(curNode.getNodeId());
            return true;
         }
      }
      return false;
   };

   // do DFS postorder traversal through the tree
   while (!todos.empty()) {
      auto& [curNode, visited] = todos.top();
      if (!visited) {
         for (auto& child : curNode.getChildren()) {
            todos.emplace(*child, false);
         }
         visited = true;
      } else {
         checkIfRequired(curNode);
         todos.pop();
      }
   }
}
// ---------------------------------------------------------------------------------------------------
bool FIterator::hasReferencedIterator() const {
   return referencedIterator != nullptr || owner.getTableScan() != nullptr;
}
// ---------------------------------------------------------------------------------------------------
std::unordered_set<std::size_t> FIterator::getTransitiveRequiredNodeIDs() const {
   if (referencedIterator == nullptr) {
      return requiredNodeIDs;
   } else {
      auto result = referencedIterator->getTransitiveRequiredNodeIDs();
      result.insert(requiredNodeIDs.begin(), requiredNodeIDs.end());
      return result;
   }
}
// ---------------------------------------------------------------------------------------------------
// int main(int argc, char* argv[]) {
// }
// ---------------------------------------------------------------------------------------------------