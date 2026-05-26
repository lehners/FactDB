#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct IsLast : public FIteratorGenerator {
   IsLast(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter) {};

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void IsLast::visitRoot(const FNodeOwning& node) {
   out << "bool isLast() const { return true" << fw::pushSeparator(" && ", false, false);
   visit(node);
   out << "; }" << fw::endl() << fw::popSeparator();

   out << "bool isAnyLast() const { return false" << fw::pushSeparator(" || ", false, false);
   visit(node);
   out << "; }" << fw::endl() << fw::popSeparator();
}
// ---------------------------------------------------------------------------------------------------
inline void IsLast::visit(const FNodeOwning& node) {
   if (isNodeInIteratorContained(node)) {
      out << fw::separator() << FTreeUtil(node).iteratorVar() << ".isLast()";
   }
   recurseRequiredChildren(node);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
