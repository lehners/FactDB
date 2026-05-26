#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct IteratorVars : public FIteratorGenerator {
   FWContainer prefix = fw::nop();
   const FNode* curParent = nullptr;
   size_t childIdx = 0;
   bool isRoot = true;

   IteratorVars(FileWriter& o, const FIterator& iter, FWContainer prefixP = fw::nop()) : FIteratorGenerator(o, iter), prefix(std::move(prefixP)) {};

   void visitRoot(const FNodeOwning& node) override {
      isRoot = true;
      visit(node);
   }

   void visit(const FNodeOwning& node) override {
      // always use iterator for the root node, required for the size calculation
      if (isRoot || isNodeInIteratorContained(node)) {
         if (prefix.isNOP()) {
            out << fw::fmt("{}::Iterator {} = {{}};", FTreeUtil(node).listType(), FTreeUtil(node).iteratorVar()) << fw::endl();
         } else {
            out << fw::fmt("{}::{}::Iterator {} = {{}};", prefix, FTreeUtil(node).listType(), FTreeUtil(node).iteratorVar()) << fw::endl();
         }
      }
      TemporarySetter<const FNode*> ts(curParent, &node);
      isRoot = false;
      enumerateRequiredChildren(node, childIdx);
   }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
