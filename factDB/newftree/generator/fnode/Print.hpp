#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FTreeTraversal.hpp"
#include <ranges>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct Printer : public FTreeTraversalGenerator {
   private:
   bool onlyLeafsSeen = true;

   public:
   Printer(FileWriter& o) : FTreeTraversalGenerator(o){};

   void visitRoot(const FNodeOwning& node) override;
   TemporarySetterVector loopChildren(stage curStage, const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void Printer::visitRoot(const FNodeOwning& node) {
   out << "void print(FileWriter& out) const {" << fw::endl();
   visit(node);
   out << "out << fw::endl();" << fw::endl()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline Printer::TemporarySetterVector Printer::loopChildren(stage curStage, const FNodeOwning& node) {
   FWContainer vFirst = fw::fmt("first{}", node.getNodeId());
   switch (curStage) {
      case prepare:
         out << fw::fmt(R"(out << "{{ " << fw::pushSeparator(", ");)") << fw::endl();
         break;
      case prepareLoop:
         out << fw::fmt("out << fw::separator() {}<< ", fw::condition(node.getIUs().size() != 1, "<< \"(\" "))
             << fw::pushSeparator(" << \", \" << ")
             << (node.getIUs() | std::views::transform([&](const IU* iu) { return fw::fmt("std::get<{}>({}).value", FTreeUtil(node).getIUIdx(*iu), loopIterator); }))
             << fw::popSeparator()
             << fw::condition(node.getIUs().size() != 1, " << \")\"") << fw::sendl();
         break;
      case beforeApply: {
         out << fw::fmt("out << \" × \"{};", fw::condition(!curChild->isLeaf(), " << fw::endl()")) << fw::endl();
         onlyLeafsSeen &= curChild->isLeaf();
         TemporarySetterVector ret;
         ret.push_back(TemporarySetterBase::create(onlyLeafsSeen, true));
         return ret;
      }
      case afterApply: break;
      case finalizeLoop:
         out << fw::condition(!onlyLeafsSeen, "out << fw::endl();") << fw::endl_non_empty();
         break;
      case finalize:
         out << "out << \"}\" << fw::popSeparator();" << fw::endl();
         break;
   }
   return {};
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
