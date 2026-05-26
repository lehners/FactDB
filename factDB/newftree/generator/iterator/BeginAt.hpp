#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
// Attention with operator++: BeginAt(5) may be different to 5 increments, since the increment does
// not check, if subtrees are empty. To be sure to increment properly, check if the subtree is empty.
// ---------------------------------------------------------------------------------------------------
struct BeginAt : public FIteratorGenerator {
   FWContainer factorizedTableName;
   FWContainer currentList = "t.cl";
   FWContainer previousOffset = 0;
   FWContainer previousTotalSize = fw::nop();
   FWContainer initialShift;
   const FNodeOwning* parentNode = nullptr;

   std::vector<std::tuple<std::reference_wrapper<FNode>, size_t, std::reference_wrapper<FNode>>> previousIterators;
   BeginAt(FileWriter& o, const FIterator& iter, FWContainer factorizedTable) : FIteratorGenerator(o, iter), factorizedTableName(std::move(factorizedTable)){};

   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void BeginAt::visitRoot(const FNodeOwning& node) {
   if (iterator.hasReferencedIterator())
      return; // currently not for reference iterators

   out << fw::fmt("static {} beginAt({}& t, size_t idx, const std::vector<size_t>& bounds) {{", FTreeUtil::getIteratorName(iterator), factorizedTableName) << fw::endl()
       << fw::fmt("  auto lowerBound = std::lower_bound(bounds.begin(), bounds.end(), idx);") << fw::endl()
       << fw::fmt("  assert(lowerBound != bounds.end());") << fw::endl()
       << fw::fmt("  auto idxOffset = (lowerBound - bounds.begin()) * 50;") << fw::endl()
       << fw::fmt("  {} i;", FTreeUtil::getIteratorName(iterator)) << fw::endl();

   previousOffset = "(idxOffset == 0 ? 0 : *(lowerBound-1))";
   initialShift = "idxOffset";

   visit(node);
   // get iterator at position i -> i.e. before is smaller and after offset is larger or equal

   out << "return i;" << fw::endl()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void BeginAt::visit(const FNodeOwning& node) {
   // ensure that ordering of the nodes considered here is equivalent to the increment.
   out << fw::fmt("size_t offset{} = {}", node.getNodeId(), previousOffset);

   out << fw::sendl()
       << "{" << fw::endl();
   if (!previousTotalSize.isNOP()) {
      assert(parentNode != nullptr);
      if (parentNode->getChildren().size() == 1)
         ; // nothing has to be done, since we handle this case already below with node.getChildren().size() == 1 (single child => parent dictates sizeScaler)
      else if (node.isLeaf())
         out << fw::fmt("{} /= {}.size();", previousTotalSize, currentList) << fw::endl();
      else
         out << fw::fmt("size_t listSize{} = 0;", node.getNodeId()) << fw::endl()
             << fw::fmt("for (auto iter = {}.begin(); !iter.isLast(); ++iter) {{", currentList) << fw::endl()
             << fw::fmt("  listSize{} += std::get<{}>(*iter);", node.getNodeId(), FTreeUtil::getSizeIdx()) << fw::endl()
             << "}" << fw::endl()
             << fw::fmt("{} /= listSize{};", previousTotalSize, node.getNodeId()) << fw::endl();

      out << fw::endl();
   }

   auto tupleSize = fw::condition(node.isLeaf(), 1, fw::fmt("std::get<{}>(*i.{})", FTreeUtil::getSizeIdx(), FTreeUtil(node).iteratorVar()));
   auto sizeScaler = fw::condition(previousTotalSize.isNOP(), 1, FWContainer(previousTotalSize));

   out << fw::fmt("i.{} = {}.begin();", FTreeUtil(node).iteratorVar(), currentList) << fw::endl()
       << fw::condition(!initialShift.isNOP(), fw::fmt("i.{} += {};", FTreeUtil(node).iteratorVar(), initialShift)) << fw::endl_non_empty()
       << fw::fmt("const size_t sizeScaler{} = {};", node.getNodeId(), sizeScaler) << fw::endl()
       << fw::fmt("while (offset{0} + sizeScaler{0} * {1} <= idx) {{", node.getNodeId(), tupleSize) << fw::endl()
       << fw::fmt("  offset{0} += sizeScaler{0} * {1};", node.getNodeId(), tupleSize) << fw::endl()
       << fw::fmt("  ++i.{};", FTreeUtil(node).iteratorVar()) << fw::endl()
       << fw::fmt("  assert(!i.isLast());") << fw::endl()
       << fw::fmt("}}") << fw::endl()
       << "}" << fw::endl();
   if (node.getChildren().size() == 1) {
      out << fw::fmt("[[maybe_unused]] size_t tupleSize{} = {};", node.getNodeId(), sizeScaler) << fw::endl();
   } else {
      out << fw::fmt("[[maybe_unused]] size_t tupleSize{} = std::get<{}>(*i.{}) * {};", node.getNodeId(), FTreeUtil::getSizeIdx(), FTreeUtil(node).iteratorVar(), sizeScaler) << fw::endl();
   }

   initialShift = fw::nop();

   previousOffset = fw::fmt("offset{}", node.getNodeId());
   for (auto& child : node.getChildren()) {
      if (isNodeRequired(*child)) {
         TemporarySetter tParentNode(parentNode, &node);
         TemporarySetter tCurrentList(currentList, fw::fmt("std::get<{}>(*i.{})", FTreeUtil(node).getChildIdx(*child), FTreeUtil(node).iteratorVar()));
         TemporarySetter tPreviousTotalSize(previousTotalSize, fw::fmt("tupleSize{}", node.getNodeId()));
         apply(*child);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
