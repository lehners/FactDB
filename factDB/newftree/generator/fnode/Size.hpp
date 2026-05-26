#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FTreeTraversal.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct FNodeSize : public FTreeTraversalGenerator {
   friend struct IteratedTupleSize;

   protected:
   FWContainer previousSizeVar = "totalSize";

   public:
   FNodeSize(FileWriter& o) : FTreeTraversalGenerator(o){};

   void visitRoot(const FNodeOwning& node) override;
   TemporarySetterVector loopChildren(stage curStage, const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void FNodeSize::visitRoot(const FNodeOwning& node) {
   out << "size_t size() const {" << fw::endl()
       << fw::fmt("size_t {} = 1;", previousSizeVar) << fw::endl();
   visit(node);
   out << fw::fmt("return {};", previousSizeVar) << fw::endl()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline FNodeSize::TemporarySetterVector FNodeSize::loopChildren(stage curStage, const FNodeOwning& node) {
   FWContainer vSize = fw::fmt("size{}", node.getNodeId());
   FWContainer vSum = fw::fmt("sum{}", node.getNodeId());
   switch (curStage) {
      case prepare:
         if (node.isLeaf()) {
            out << fw::fmt("{} *= {}.size();", previousSizeVar, currentList) << fw::endl();
            doLoop = false;
         } else {
            out << fw::fmt("size_t {} = 0;", vSum) << fw::endl();
         }
         break;
      case prepareLoop:
         out << fw::fmt("  size_t {} = 1;", vSize) << fw::endl();
         break;
      case beforeApply: {
         TemporarySetterVector ret;
         ret.push_back(TemporarySetterBase::create(previousSizeVar, std::move(vSize)));
         return ret;
      }
      case afterApply: break;
      case finalizeLoop:
         out << fw::fmt("  {} += {};", vSum, vSize) << fw::endl();
         break;
      case finalize:
         if (!node.isLeaf())
            out << fw::fmt("{} *= {};", previousSizeVar, vSum) << fw::endl();
         doLoop = true;
         break;
   }
   return {};
}
// ---------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
