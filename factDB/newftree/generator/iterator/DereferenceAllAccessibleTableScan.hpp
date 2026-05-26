#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/util/ranges.hpp"
#include "factDB/newftree/generator/fnode/PushBack.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/DoOnDestruction.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct DereferenceAllAccessibleTableScan : public FIteratorGenerator {
   DereferenceAllAccessibleTableScan(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter){};

   void visit(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void DereferenceAllAccessibleTableScan::visit(const FNodeOwning& node) {
   assert(node.getChildren().empty());
   assert(iterator.getOwner().getTableScan() != nullptr);

   out << fw::pushSeparator("else ")
       << fw::fmt("template <{} Attribute> auto& get() const {{", FTreePushBack::getPushbackEnum(node)) << fw::endl();
   size_t idx = 0;
   for (auto& iu : iterator.getOwner().getTableScan()->collectIUs()) {
      DoOnDestruction dod([&]() { ++idx; });
      if (!node.containsIU(*iu))
         continue;
      size_t idxS = idx;
      out << fw::separator() << fw::fmt("if constexpr (Attribute == {}) return std::get<{}>(*get{}());", FTreePushBack::getPushbackEnumValue(node, *iu), idxS, FTreeUtil(node).iteratorVar()) << fw::endl();
   }
   out << "}" << fw::endl() << fw::popSeparator();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
