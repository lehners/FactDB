// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct FIteratorGeneratorHelper : public FIteratorGenerator {
   FIteratorGeneratorHelper(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter) {};
   void visit(const FNodeOwning&) override {}
};
// ---------------------------------------------------------------------------------------------------
bool FIteratorGenerator::isNodeRequired(const FNode& node) const {
   if (node.isRequired(iterator))
      return true;
   // lookup in the reference iterator is required for correct calculation of e.g. size, where we have
   // to multiply the result with the factor of the not expanded factors in the tree.
   if (isNodeRequiredByReferenceIterator(node))
      return true;
   return false;
}
// ---------------------------------------------------------------------------------------------------
bool FIteratorGenerator::isNodeRequiredByReferenceIterator(const FNode& node) const {
   return node.isRequiredByReferenceIterator(iterator);
}
// ---------------------------------------------------------------------------------------------------
bool FIteratorGenerator::hasRequiredChild(const FNode& node) const {
   for (auto& child : node.getChildren())
      if (child->isRequired(iterator))
         return true;
   return false;
}
// ---------------------------------------------------------------------------------------------------
bool FIteratorGenerator::hasNotRequiredChild(const FNode& node) const {
   for (auto& child : node.getChildren())
      if (!child->isRequired(iterator))
         return true;
   return false;
}
// ---------------------------------------------------------------------------------------------------
bool FIteratorGenerator::hasChildNotRequiredByReferenceIterator(const FNode& node, bool checkIteratedIUs) const {
   if (!iterator.hasReferencedIterator())
      return true; // no child required
   for (auto& child : node.getChildren()) {
      if (!isNodeRequiredByReferenceIterator(*child) && (!checkIteratedIUs || child->isRequired(iterator)))
         return true;
   }
   return false;
}
// ---------------------------------------------------------------------------------------------------
bool FIteratorGenerator::isNodeInIteratorContained(const FNode& node) const {
   if (isNodeRequired(node)) {
      if (isNodeRequiredByReferenceIterator(node)) {
         // check if the node contains an IU of this operator
         for (const IU* iu : iterator.getIteratedIUs()) {
            if (node.containsIU(iu))
               return true;
         }
         // keep iterators iff at bottom of parent iterators
         // return hasChildNotRequiredByReferenceIterator(node, false); // todo?
         return true;
      } else {
         return true;
      }
   } else {
      return false;
   }
}
// ---------------------------------------------------------------------------------------------------
std::unordered_set<size_t> FIteratorGenerator::inIteratorContainedIDs() const {
   std::stack<const FNode*> todos;
   std::unordered_set<size_t> containedIDs;
   todos.emplace(&iterator.getOwner().getRootNode());
   while (!todos.empty()) {
      const auto* cur = todos.top();
      todos.pop();
      if (isNodeInIteratorContained(*cur))
         containedIDs.insert(cur->getNodeId());
      for (auto& c : cur->getChildren()) {
         todos.emplace(c.get());
      }
   }
   return containedIDs;
}
// ---------------------------------------------------------------------------------------------------
bool FIteratorGenerator::containsFreeNodeInSubtree(const FNode& node) const {
   if (isNodeRequired(node) && !isNodeRequiredByReferenceIterator(node))
      return true;
   for (auto& child : node.getChildren()) {
      if (containsFreeNodeInSubtree(*child))
         return true;
   }
   return false;
}
// ---------------------------------------------------------------------------------------------------
void FIteratorGenerator::recurseRequiredChildren(const FNode& node) {
   [[maybe_unused]] size_t idx = 0;
   return enumerateRequiredChildren(node, idx);
}
// ---------------------------------------------------------------------------------------------------
void FIteratorGenerator::recurseNotRequiredChildren(const FNode& node) {
   for (auto& child : node.getChildren()) {
      if (!isNodeRequired(*child))
         apply(*child);
   }
}
// ---------------------------------------------------------------------------------------------------
void FIteratorGenerator::enumerateRequiredChildren(const FNode& node, size_t& id) {
   for (size_t i = 0; auto& child : node.getChildren()) {
      id = i++;
      if (isNodeRequired(*child))
         apply(*child);
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
