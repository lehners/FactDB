#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/FNode.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class FNodeGenerator {
   public:
   virtual ~FNodeGenerator() = default;
   FileWriter& out;
   bool root = true;

   public:
   explicit FNodeGenerator(FileWriter& outWriter) : out(outWriter) {}

   virtual void visit(const FNodeOwning& node) = 0;
   virtual void visit(const FNodeReferencing& node) { visit(node.getOwningNode()); }

   virtual void visitRoot(const FNodeOwning& node) { visit(node); }
   virtual void visitRoot(const FNodeReferencing& node) { visitRoot(node.getOwningNode()); }

   void apply(const FNode& node);

   void recurseChildren(const FNode& node);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
