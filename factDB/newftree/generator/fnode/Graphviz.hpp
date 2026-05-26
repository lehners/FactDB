#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FNodeGenerator.hpp"
#include "factDB/queryc/NewFileWriter.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class Graphviz : public FNodeGenerator {
   const FNode* parentNode = nullptr;
   bool printNodeId = true;
   const FWContainer subgraphName = fw::nop();

   public:
   Graphviz(FileWriter& o, bool printNodeId_ = true, FWContainer subgraphName_ = fw::nop()) : FNodeGenerator(o), printNodeId(printNodeId_), subgraphName(std::move(subgraphName_)) {
      if (subgraphName.isNOP()) {
         out << "digraph structs {" << fw::endl()
             << "node [shape=record];" << fw::endl();
      } else { // print only subgraph
         out << fw::fmt("subgraph cluster_{} {{", subgraphName) << fw::endl()
             << fw::fmt("  style = rounded;") << fw::endl()
             << fw::fmt("  label = \"{}\";", subgraphName) << fw::endl()
             << fw::fmt("  anchor_{} [shape=point, width=0.01, style=invis];", subgraphName) << fw::endl();
      }
   }

   ~Graphviz() {
      out << "}" << fw::endl();
   }

   void visit(const FNodeOwning& node) override { addNode(node); }
   void visit(const FNodeReferencing& node) override { addNode(node); }

   private:
   FNode::NodeIdType nodeID(const FNode& node) { return printNodeId ? node.getNodeId() : 0; }
   void addNode(const FNode& node);
};
// ---------------------------------------------------------------------------------------------------
inline void Graphviz::addNode(const factDB::FNode& node) {
   std::string refKey = node.type == FNode::Owning ? "Node" : "Reference";
   out << fw::fmt("{3}_node{0} [label=\"{{{{{1} {0} }}|{{ {2} }}}}\"];", nodeID(node), refKey, fw::iter(node.getIUs()), subgraphName) << fw::endl();
   if (parentNode != nullptr)
      out << fw::fmt("{0}_node{1} -> {0}_node{2};", subgraphName, nodeID(*parentNode), nodeID(node)) << fw::endl();

   for (auto& child : node.getChildren()) {
      TemporarySetter ts(parentNode, &node);
      apply(*child);
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------