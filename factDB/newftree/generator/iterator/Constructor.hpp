#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct Constructor : public FIteratorGenerator {
   FWContainer currentList = fw::nop();
   FWContainer factorizedTableName;

   Constructor(FileWriter& o, const FIterator& iter, FWContainer factorizedTable) : FIteratorGenerator(o, iter), factorizedTableName(std::move(factorizedTable)){};

   void visit(const FNodeOwning&) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void Constructor::visitRoot(const FNodeOwning& node) {
   if (iterator.hasReferencedIterator())
      return;
   out << fw::fmt("{}({}::Iterator&& handle, [[maybe_unused]] {}& fti)", FTreeUtil::getIteratorName(iterator), FTreeUtil(node).listType(), factorizedTableName) << fw::endl()
       << fw::fmt(" : {}(std::move(handle))  {{", FTreeUtil(node).iteratorVar()) << fw::endl();

   visit(node);

   out << "assert(!isAnyLast());" << fw::endl()
       << fw::fmt("}}") << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void Constructor::visit(const FNodeOwning& node) {
   if (!currentList.isNOP())
      out << fw::fmt("{} = {}.begin();", FTreeUtil(node).iteratorVar(), currentList) << fw::endl();

   for (auto& child : node.getChildren()) {
      if (isNodeRequired(*child)) {
         TemporarySetter ts(currentList, fw::fmt("std::get<{}>(*{})", FTreeUtil(node).getChildIdx(*child), FTreeUtil(node).iteratorVar()));
         apply(*child);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
