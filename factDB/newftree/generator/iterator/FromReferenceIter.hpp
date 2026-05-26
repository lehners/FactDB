#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct FromReferencedIterator : public FIteratorGenerator {
   FWContainer currentList;
   bool isRoot = true;

   FromReferencedIterator(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter){};

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void FromReferencedIterator::visitRoot(const FNodeOwning& node) {
   if (!iterator.hasReferencedIterator())
      return;
   isRoot = true;
   out << fw::fmt("static {} fromReferencedIterator(const {}& r) {{", FTreeUtil::getIteratorName(iterator), FTreeUtil::getIteratorName(iterator.getReferencedIterator())) << fw::endl()
       << fw::fmt("{} i;", FTreeUtil::getIteratorName(iterator)) << fw::endl()
       << fw::fmt("i.{0} = r.get{0}();", FTreeUtil(node).iteratorVar()) << fw::endl();
   apply(node);
   out << "if (i.isAnyLast()) ++i;" << fw::endl()
       << "return i;" << fw::endl()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void FromReferencedIterator::visit(const FNodeOwning& node) {
   if (!isRoot && isNodeInIteratorContained(node)) {
      if (isNodeRequiredByReferenceIterator(node)) {
         out << fw::fmt("i.{0} = r.get{0}();", FTreeUtil(node).iteratorVar()) << fw::endl();
      } else if (!currentList.isNOP()) {
         out << fw::fmt("i.{0} = {1}.begin();", FTreeUtil(node).iteratorVar(), currentList) << fw::endl();
      }
   }

   isRoot = false;
   for (auto& child : node.getChildren()) {
      if (isNodeRequired(*child)) {
         TemporarySetter ts(currentList, fw::fmt("std::get<{}>(*i.{})", FTreeUtil(node).getChildIdx(*child), FTreeUtil(node).iteratorVar()));
         apply(*child);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
