#include "factDB/newftree/FNode.hpp"
#include "factDB/newftree/FIterator.hpp"
#include "factDB/newftree/generator/fnode/Graphviz.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
static size_t globalFNodeId = 0;
// ---------------------------------------------------------------------------------------------------
void FNode::resetFNodeId() {
   globalFNodeId = 0;
}
// ---------------------------------------------------------------------------------------------------
FNodeOwning::FNodeOwning() noexcept : FNode(Owning), nodeID(globalFNodeId++) {
}
// ---------------------------------------------------------------------------------------------------
FNode::~FNode() = default;
// ---------------------------------------------------------------------------------------------------
bool FNode::operator==(const factDB::FNode& other) const {
   return getOwningNode() == other.getOwningNode();
}
// ---------------------------------------------------------------------------------------------------
bool FNodeOwning::operator==(const factDB::FNodeOwning& other) const {
   if (ius != other.ius) {
      return false;
   }
   if (children.size() != other.children.size())
      return false;
   for (auto& child : children) {
      bool hasPartner = false;
      for (auto& otherChild : other.children) {
         if (*child == *otherChild) {
            hasPartner = true;
            break;
         }
      }
      if (!hasPartner)
         return false; // if we did not find a partner for the child, the nodes are not equal
   }
   return true;
}
// ---------------------------------------------------------------------------------------------------
FNode& FNodeOwning::addToMergedNode(FNode& child) {
   if (mergedChild == nullptr) { // new merged node has to be created
      mergedChild = children.emplace_back(std::make_unique<FNodeOwning>()).get();
   }
   mergedChild->addIUs(child.getIUs());
   return *mergedChild;
}
// ---------------------------------------------------------------------------------------------------
void FNodeOwning::addChild(FNode& child, bool referencing) {
   if (referencing || child.type == Reference) {
      children.emplace_back(std::make_unique<FNodeReferencing>(child));
   } else {
      auto& newChild = addChild();
      newChild.addIUs(child.getIUs());
      for (auto& c : child.getChildren()) {
         newChild.addChild(*c, false);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
FNode& FNodeOwning::addChild() {
   return *children.emplace_back(std::make_unique<FNodeOwning>()).get();
}
// ---------------------------------------------------------------------------------------------------
FNode& FNodeOwning::addIUs(const OrderedIUSet& insertedIUs) {
   ius.merge(insertedIUs);
   return *this;
}
// ---------------------------------------------------------------------------------------------------
bool FNodeOwning::isRequired(const FIterator& iterator) const {
   return iterator.getRequiredNodeIDs().contains(nodeID);
}
// ---------------------------------------------------------------------------------------------------
bool FNode::isRequiredByReferenceIterator(const FIterator& iterator) const {
   auto curIter = &iterator;
   while (curIter->getReferencedIteratorPtr() != nullptr) {
      // required to check all reference iterators recursively
      curIter = curIter->getReferencedIteratorPtr();
      if (isRequired(*curIter))
         return true;
   }
   return false;
}
// ---------------------------------------------------------------------------------------------------
void FNode::print(factDB::FileWriter& out, bool printNodeId, FWContainer subgraphName) const {
   Graphviz(out, printNodeId, std::move(subgraphName)).apply(*this);
}
// ---------------------------------------------------------------------------------------------------