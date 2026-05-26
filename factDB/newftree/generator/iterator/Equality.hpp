#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct Equality : public FIteratorGenerator {
   FWContainer comparator;
   Equality(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter){};

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void Equality::visitRoot(const FNodeOwning& node) {
   auto genComparison = [&](FWContainer cmp, const std::string& separator, const std::string& neutral) {
      comparator = std::move(cmp);
      out << fw::fmt("bool operator{}([[maybe_unused]] const {}& other) const {{ return {}", comparator, FTreeUtil::getIteratorName(iterator), neutral)
          << fw::pushSeparator(separator, false, false);
      visit(node);
      out << "; }" << fw::endl() << fw::popSeparator();
   };

   genComparison("==", " && ", "true"); // equality
   genComparison("!=", " || ", "false"); // inequality
}
// ---------------------------------------------------------------------------------------------------
inline void Equality::visit(const FNodeOwning& node) {
   if (isNodeInIteratorContained(node))
      out << fw::separator() << fw::fmt("{0} {1} other.{0}", FTreeUtil(node).iteratorVar(), comparator);
   recurseRequiredChildren(node);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
