#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
#include <ranges>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
constexpr bool printNodeId = true;
// ---------------------------------------------------------------------------------------------------
struct IteratorGraphviz : public FIteratorGenerator {
   FWContainer parentStruct = fw::nop();

   IteratorGraphviz(FileWriter& o, const FIterator& iteratorP) : FIteratorGenerator(o, iteratorP) {};
   void drawNode(const FNode& node, const std::string& nodePrefix);
   void drawArrowsAndChildren(const FNode& node, FWContainer curStruct);
   void visit(const FNodeOwning&) override;
   void visit(const FNodeReferencing&) override;
   void visitRoot(const FNodeOwning&) override;
};
// ---------------------------------------------------------------------------------------------------
inline void IteratorGraphviz::drawNode(const FNode& node, const std::string& nodePrefix) {
   auto curStruct = fw::fmt("struct{}", node.getNodeId());

   out << curStruct << " [label=";

   auto markIUIfRequired = [&](const IU* iu) {
      if (iterator.getIteratedIUs().contains(iu))
         return fw::fmt("*{}*", iu);
      else
         return fw::fmt("{}", iu);
   };
   auto markedIUs = fw::iter(node.getIUs() | std::views::transform(markIUIfRequired));

   if (printNodeId) {
      out << fw::fmt("\"{{ {{ {} Node {} }}|{{ {} }} }}\"", nodePrefix, node.getNodeId(), markedIUs);
   } else if (nodePrefix.empty()) {
      out << fw::fmt("\"{{ {} }}\"", markedIUs);
   } else {
      out << fw::fmt("\"{{ {{ {} }}|{{ {} }} }}\"", nodePrefix, markedIUs);
   }

   if (isNodeRequiredByReferenceIterator(node))
      out << ", style=dashed";
   if (node.isRequired(iterator))
      out << ", fontcolor=red";

   out << "];" << fw::endl();
   drawArrowsAndChildren(node, std::move(curStruct));
}
// ---------------------------------------------------------------------------------------------------
inline void IteratorGraphviz::drawArrowsAndChildren(const FNode& node, FWContainer curStruct) {
   if (!parentStruct.isNOP())
      out << fw::fmt("{} -> {};", parentStruct, curStruct) << fw::endl();
   for (auto& child : node.getChildren()) {
      TemporarySetter ts(parentStruct, curStruct);
      apply(*child);
   }
}
// ---------------------------------------------------------------------------------------------------
inline void IteratorGraphviz::visit(const FNodeOwning& node) {
   drawNode(node, "");
}
// ---------------------------------------------------------------------------------------------------
inline void IteratorGraphviz::visit(const FNodeReferencing& node) {
   drawNode(node, "Reference");
}
// ---------------------------------------------------------------------------------------------------
inline void IteratorGraphviz::visitRoot(const FNodeOwning& node) {
   out << "digraph structs {" << fw::endl()
       << "node [shape=record];" << fw::endl() << fw::pushSeparator("|");
   visit(node);
   out << "}" << fw::endl() << fw::popSeparator();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
