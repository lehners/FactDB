#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct AdditionalIterators : public FIteratorGenerator {
   std::unordered_set<size_t> availableNodes;

   AdditionalIterators(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter), availableNodes(iterator.getOwner().collectComposedAvailableNodes()) {}
   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;

   void generateIterator(const FNodeOwning& node) const;
};
// ---------------------------------------------------------------------------------------------------
inline void AdditionalIterators::generateIterator(const FNodeOwning& node) const {
   if (!availableNodes.contains(node.getNodeId())) { // create newly created iterator if needed
      out << fw::fmt("{} {};", FTreeUtil(node).iteratorType(), FTreeUtil(node).iteratorVar()) << fw::endl();
   }
}
// ---------------------------------------------------------------------------------------------------
inline void AdditionalIterators::visitRoot(const FNodeOwning& node) {
   generateIterator(node);
   recurseRequiredChildren(node);
}
// ---------------------------------------------------------------------------------------------------
inline void AdditionalIterators::visit(const FNodeOwning& node) {
   generateIterator(node);
   recurseRequiredChildren(node);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
