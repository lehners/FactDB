#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct Begin : public FIteratorGenerator {
   FWContainer currentList = "t.cl";
   FWContainer factorizedTableName;

   Begin(FileWriter& o, const FIterator& iter, FWContainer factorizedTable) : FIteratorGenerator(o, iter), factorizedTableName(std::move(factorizedTable)){};

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void Begin::visitRoot(const FNodeOwning& node) {
   if (iterator.hasReferencedIterator())
      return;
   out << fw::fmt("static {} begin({}& t) {{", FTreeUtil::getIteratorName(iterator), factorizedTableName) << fw::endl()
       << fw::fmt("{} i;", FTreeUtil::getIteratorName(iterator)) << fw::endl();
   visit(node);
   out << "if (i.isAnyLast()) ++i;" << fw::endl()
       << "return i;" << fw::endl()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void Begin::visit(const FNodeOwning& node) {
   out << fw::fmt("i.{} = {}.begin();", FTreeUtil(node).iteratorVar(), currentList) << fw::endl()
       << "if (!i.isLast()) [[likely]] {" << fw::endl();

   for (auto& child : node.getChildren()) {
      if (isNodeRequired(*child)) {
         TemporarySetter ts(currentList, fw::fmt("std::get<{}>(*i.{})", FTreeUtil(node).getChildIdx(*child), FTreeUtil(node).iteratorVar()));
         apply(*child);
      }
   }
   out << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
