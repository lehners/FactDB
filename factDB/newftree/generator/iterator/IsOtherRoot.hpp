#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FGenUtil.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct IsOtherRoot : public FIteratorGenerator {
   FWContainer comparator;
   IsOtherRoot(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter){};

   void visit(const FNodeOwning&) override {};
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void IsOtherRoot::visitRoot(const FNodeOwning& node) {
   if (!isNodeRequiredByReferenceIterator(node))
      return;
   out << fw::fmt("bool isOtherRoot(const {0}& other) {{ return {1} != other.{1}; }}", FTreeUtil::getIteratorName(iterator), FTreeUtil(node).iteratorVar()) << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
