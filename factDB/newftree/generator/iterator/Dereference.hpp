#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct Dereference : public FIteratorGenerator {
   FWContainer comparator;
   Dereference(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter){};

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void Dereference::visitRoot(const FNodeOwning& node) {
   out << "reference operator*() const {" << fw::endl()
       << "  assert(!isAnyLast());" << fw::endl();
   visit(node);
   out << fw::fmt("return std::tie({});", fw::iter(iterator.getIteratedIUs())) << fw::endl();
   out << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void Dereference::visit(const FNodeOwning& node) {
   for (auto& iu : node.getIUs()) {
      if (iterator.getIteratedIUs().contains(iu))
         out << fw::fmt("auto& {} = {};", iu, fw::get(FTreeUtil(node).getIUIdx(*iu), fw::dereference(FTreeUtil(node).iteratorVar()))) << fw::endl();
   }
   recurseRequiredChildren(node);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
