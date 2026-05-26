#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/util/ranges.hpp"
#include "factDB/newftree/generator/util/FNodeGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct Reserve : public FNodeGenerator {
   private:
   FWContainer currentList = fw::lc("cl");
   FWContainer parentHandleType = fw::nop();
   bool genReserve = true;

   public:
   Reserve(FileWriter& o) : FNodeGenerator(o){};
   void visitRoot(const FNodeOwning& node) override;
   void visit(const FNodeReferencing& node) override;
   void visit(const FNodeOwning& node) override;

   static FWContainer getPushbackEnum(const FNode& node) { return fw::fmt("PBEnum{}", node.getNodeId()); }
};
// ---------------------------------------------------------------------------------------------------
inline void Reserve::visit(const FNodeReferencing& node) {
   out << "// reserve not generated for reference nodes." << fw::endl();

   // generate for all children
   for (const auto& child : node.getChildren()) {
      TemporarySetter tsGenPushBack(genReserve, false);
      apply(*child);
   }
}
// ---------------------------------------------------------------------------------------------------
inline void Reserve::visitRoot(const FNodeOwning& node) {
   out << fw::fmt("void reserve({}, size_t size) {{ cl.reserve(size); }}", getPushbackEnum(node)) << fw::endl();

   // generate for all children
   for (const auto& child : node.getChildren()) {
      const FTreeUtil treeUtil(node);
      TemporarySetter tsCurrentList(currentList, fw::get(treeUtil.getChildIdx(*child), "*parentIter"));
      TemporarySetter tsParentHandleType(parentHandleType, treeUtil.iteratorType());
      apply(*child);
   }
}
// ---------------------------------------------------------------------------------------------------
inline void Reserve::visit(const FNodeOwning& node) {
   FWContainer pushbackEnum = getPushbackEnum(node);

   if (genReserve) {
      out << fw::fmt("void reserve({}& parentIter, {}, size_t size) const {{", parentHandleType, pushbackEnum)
          << fw::fmt("{}.reserve(size);", currentList)
          << "}" << fw::endl();
   } else {
      out << fw::fmt("// reserve not generated for children of reference node ({})", pushbackEnum) << fw::endl();
   }

   // generate for all children
   for (const auto& child : node.getChildren()) {
      const FTreeUtil treeUtil(node);
      TemporarySetter tsCurrentList(currentList, fw::get(treeUtil.getChildIdx(*child), "*parentIter"));
      TemporarySetter tsParentHandleType(parentHandleType, treeUtil.iteratorType());
      apply(*child);
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
