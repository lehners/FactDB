#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct End : public FIteratorGenerator {
   FWContainer currentList = "t.cl";
   FWContainer factorizedTableName;

   End(FileWriter& o, const FIterator& iter, FWContainer factorizedTable) : FIteratorGenerator(o, iter), factorizedTableName(std::move(factorizedTable)){};

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void End::visitRoot(const FNodeOwning& node) {
   if (iterator.hasReferencedIterator())
      return;
   out << fw::fmt("static {} end({}& t) {{", FTreeUtil::getIteratorName(iterator), factorizedTableName) << fw::endl()
       << fw::fmt("{} i;", FTreeUtil::getIteratorName(iterator)) << fw::endl();
   visit(node);
   out << "return i;" << fw::endl()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void End::visit(const FNodeOwning& node) {
   out << fw::fmt("i.{} = {}{}.end();", FTreeUtil(node).iteratorVar(), fw::condition(!node.isLeaf(), "--"), currentList) << fw::endl();

   for (auto& child : node.getChildren()) {
      if (isNodeRequired(*child)) {
         TemporarySetter ts(currentList, fw::fmt("std::get<{}>(*i.{})", FTreeUtil(node).getChildIdx(*child), FTreeUtil(node).iteratorVar()));
         apply(*child);
      }
   }
   if (!node.isLeaf())
      out << fw::fmt("++i.{};", FTreeUtil(node).iteratorVar()) << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
