#include "factDB/newftree/generator/util/FNodeGenerator.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
void FNodeGenerator::apply(const factDB::FNode& node) {
   if (root) {
      root = false;
      switch (node.type) {
         case FNode::Reference: visitRoot(static_cast<const FNodeReferencing&>(node)); break;
         case FNode::Owning: visitRoot(static_cast<const FNodeOwning&>(node)); break;
      }
   } else {
      switch (node.type) {
         case FNode::Reference: visit(static_cast<const FNodeReferencing&>(node)); break;
         case FNode::Owning: visit(static_cast<const FNodeOwning&>(node)); break;
      }
   }
}
// ---------------------------------------------------------------------------------------------------
void FNodeGenerator::recurseChildren(const factDB::FNode& node) {
   for (const std::unique_ptr<FNode>& child : node.getChildren()) {
      apply(*child);
   }
}
// ---------------------------------------------------------------------------------------------------
