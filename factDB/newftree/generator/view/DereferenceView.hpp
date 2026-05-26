#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct DereferenceView : public FIteratorGenerator {
   FWContainer comparator;
   DereferenceView(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter) {};

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void DereferenceView::visitRoot(const FNodeOwning& node) {
   out << "auto operator*() const {" << fw::endl();
   visit(node);
   out << fw::fmt("return std::tie({});", fw::iter(iterator.getIteratedIUs())) << fw::endl();
   out << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void DereferenceView::visit(const FNodeOwning& node) {
   auto iterTranslator = FIteratorTranslator(iterator);
   for (auto& iu : node.getIUs()) {
      if (iterator.getIteratedIUs().contains(iu))
         out << fw::fmt("auto& {} = get<{}::{}>();", iu, FTreeTranslator(iterator.getOwner()).classname(), iu) << fw::endl();
   }
   recurseRequiredChildren(node);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
