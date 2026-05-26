#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/fnode/PushBack.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct DereferenceAllAccessible : public FIteratorGenerator {
   bool isRoot = true;

   DereferenceAllAccessible(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter) {};

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override {
      isRoot = true;
      visit(node);
   }
};
// ---------------------------------------------------------------------------------------------------
inline void DereferenceAllAccessible::visit(const FNodeOwning& node) {
   if (!isRoot && !isNodeInIteratorContained(node))
      return;
   isRoot = false;

   out << fw::pushSeparator("else ")
       << fw::fmt("template <{} Attribute> auto& get() const {{", FTreePushBack::getPushbackEnum(node)) << fw::endl();
   for (auto& iu : node.getIUs()) {
      out << fw::separator() << fw::fmt("if constexpr (Attribute == {}) return std::get<{}>(*get{}());", FTreePushBack::getPushbackEnumValue(node, *iu), FTreeUtil(node).getIUIdx(*iu), FTreeUtil(node).iteratorVar()) << fw::endl();
   }
   out << "}" << fw::endl() << fw::popSeparator();

   for (auto& child : node.getChildren()) {
      if (isNodeRequired(*child)) {
         apply(*child);
      } else {
         out << fw::fmt("template <{} Attribute> auto& get() const {{", FTreePushBack::getPushbackEnum(*child)) << fw::endl()
             << fw::fmt("  return std::get<{}>(*{});", FTreeUtil(node).getChildIdx(*child), FTreeUtil(node).iteratorVar()) << fw::endl()
             << fw::fmt("}}") << fw::endl();
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
