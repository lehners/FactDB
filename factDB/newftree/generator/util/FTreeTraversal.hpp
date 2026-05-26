#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/util/TemporaySetter.hpp"
#include <memory>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct FTreeTraversalGenerator : public FNodeGenerator {
   using TemporarySetterVector = std::vector<std::unique_ptr<TemporarySetterBase>>;

   FWContainer currentList = fw::lc("cl");
   FWContainer loopIterator = fw::nop();
   bool doLoop = true;
   FNode* curChild = nullptr;

   FTreeTraversalGenerator(FileWriter& o) : FNodeGenerator(o) {};
   void visit(const FNodeOwning& node) final override;

   enum stage { prepare,
                prepareLoop,
                beforeApply,
                afterApply,
                finalizeLoop,
                finalize };
   virtual TemporarySetterVector loopChildren(stage curStage, const FNodeOwning& node) = 0;
};
// ---------------------------------------------------------------------------------------------------
inline void FTreeTraversalGenerator::visit(const FNodeOwning& node) {
   loopChildren(prepare, node);

   TemporarySetter tsIter(loopIterator, fw::fmt("iter{}", node.getNodeId()));
   if (doLoop) {
      out << fw::fmt("for (auto& {} : {}) {{", loopIterator, currentList) << fw::endl();
      loopChildren(prepareLoop, node);
      size_t childIdx = 0;
      for (const auto& child : node.getChildren()) {
         TemporarySetter tsCurChild(curChild, child.get());
         TemporarySetter tsCurrentList(currentList, fw::get(FTreeUtil(node).getChildIdx(childIdx), loopIterator));
         auto setters = loopChildren(beforeApply, node);
         apply(*child);
         loopChildren(afterApply, node);
         ++childIdx;
      }
      loopChildren(finalizeLoop, node);
      out << "}" << fw::endl();
   }

   loopChildren(finalize, node);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
