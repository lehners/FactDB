#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct GetIteratorVariable : public FIteratorGenerator {
   GetIteratorVariable(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter){};
   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void GetIteratorVariable::visitRoot(const FNodeOwning& node) {
   out << fw::fmt("auto get{0}() const {{ return {0}; }}", FTreeUtil(node).iteratorVar()) << fw::endl();
   out << fw::fmt("auto get{0}Ref() const {{ return {0}; }}", FTreeUtil(node).iteratorVar()) << fw::endl();
   // if (iterator.getIteratedIUs().empty())
   //    return;
   recurseRequiredChildren(node);
}
// ---------------------------------------------------------------------------------------------------
inline void GetIteratorVariable::visit(const FNodeOwning& node) {
   assert(isNodeRequired(node));
   if (isNodeInIteratorContained(node)) {
      out << fw::fmt("auto get{0}() const {{ return {0}; }}", FTreeUtil(node).iteratorVar()) << fw::endl();
      out << fw::fmt("auto get{0}Ref() const {{ return {0}; }}", FTreeUtil(node).iteratorVar()) << fw::endl();
   }

   recurseRequiredChildren(node);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
