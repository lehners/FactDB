#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FGenUtil.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct IsSubtreeEmpty : public FIteratorGenerator {
   FWContainer comparator;
   IsSubtreeEmpty(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter){};

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void IsSubtreeEmpty::visitRoot(const FNodeOwning& node) {
   out << "bool isSubtreeEmpty() const { return false" << fw::pushSeparator(" || ", false, false);
   visit(node);
   out << "; }" << fw::endl() << fw::popSeparator();
}
// ---------------------------------------------------------------------------------------------------
inline void IsSubtreeEmpty::visit(const FNodeOwning& node) {
   if (isNodeInIteratorContained(node) && !node.isLeaf() && hasNotRequiredChild(node)) {
      out << fw::separator() << fw::fmt("std::get<{}>(*{}) == 0", FTreeUtil::getSizeIdx(), FTreeUtil(node).iteratorVar());
   }
   recurseRequiredChildren(node);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
