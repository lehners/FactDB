#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FGenUtil.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
#include <optional>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct IteratedTupleSizeView : public FIteratorGenerator {
   private:
   std::unordered_set<size_t> consideredNodes;
   FWContainer previousSizeVar = "unspecified";
   std::unordered_set<size_t> generatedIterators;

   protected:
   using TemporarySetterVector = std::vector<std::unique_ptr<TemporarySetterBase>>;

   FWContainer currentList = "unspecified";
   FWContainer loopIterator = "unspecified";
   const FNodeOwning* parent = nullptr;

   public:
   IteratedTupleSizeView(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter) {};

   void visitRoot(const FNodeOwning& node) override;
   void visit(const FNodeOwning& node) override;

   TemporarySetterVector start_cache_code(const FNodeOwning& child);
   void end_cache_code(const FNodeOwning& node, TemporarySetterVector& tmpSetter);
};
// ---------------------------------------------------------------------------------------------------
inline void IteratedTupleSizeView::visitRoot(const FNodeOwning& rootNode) {
   auto allRequiredIUs = iterator.getIteratedIUs();
   auto availableIUs = iterator.getOwner().collectComposedAvailableIUs();
   auto upperAllIUs = iterator.getOwner().getTopTree()->collectIUs();
   auto differenceUpper = allRequiredIUs.intersect(upperAllIUs).difference(availableIUs);

   auto lowerAllIUs = iterator.getOwner().getLowerTree()->collectIUs();
   auto differenceLower = allRequiredIUs.intersect(lowerAllIUs).difference(availableIUs);
   auto flatNodes = iterator.getOwner().collectComposedAvailableNodes();
   auto requiredNodes = iterator.getRequiredNodeIDs();
   for (auto& r : requiredNodes)
      flatNodes.insert(r);

   auto generateFunction = [&](const IUSet& difference, const std::string& tree, const std::unordered_set<size_t>& allNodes, const FNodeOwning& node) {
      if (difference.empty()) {
         out << fw::fmt("size_t iteratedTupleSize_{0}(bool useCache) const {{ return {0}.iteratedTupleSize(useCache); }}", tree) << fw::endl();
         return;
      }
      // first get all nodes which the counter actually considers
      consideredNodes.clear();
      for (auto& n : allNodes) {
         if (!flatNodes.contains(n))
            consideredNodes.insert(n);
      }

      assert(!consideredNodes.contains(iterator.getOwner().getRootNode().getNodeId()));

      FWContainer vSize = fw::fmt("size{}", node.getNodeId());
      FWContainer vSum = fw::fmt("totalSize");

      out << fw::fmt("size_t iteratedTupleSize_{}([[maybe_unused]] bool useCache=true) const {{", tree) << fw::endl()
          << fw::fmt("size_t {} = 0;", vSum) << fw::endl()
          << fw::fmt("size_t {} = 1;", vSize) << fw::endl();
      TemporarySetter tsPreviousSizeVar(previousSizeVar, vSize);

      TemporarySetter tsParent(parent, &node); // cheat to have a parent here
      auto curLastVarTmp = start_cache_code(node);
      for (auto& child : node.getChildren()) {
         TemporarySetter tsCurrentList(currentList, fw::get(FTreeUtil(node).getChildIdx(*child), FTreeUtil(node).iteratorVar().dereference()));
         apply(*child);
      }
      end_cache_code(node, curLastVarTmp);
      out << fw::fmt("{} += {};", vSum, vSize) << fw::endl()
          << fw::fmt("return {};", vSum) << fw::endl()
          << "}" << fw::endl();
   };

   // we can assume that not iterated parts stay the same since they are not touched by the previous operators and hence the structure is not changed.
   // however, in the subsequent nodes they may change. This simplifies the logic at some points because the before node IDs are equal to the current ones.
   generateFunction(differenceUpper, "upper", iterator.getOwner().collectUpperTreeNodes(), rootNode);
   generateFunction(differenceLower, "lower", iterator.getOwner().collectLowerTreeNodes(), iterator.getOwner().getLastMergedNode()->getOwningNode());

   out << fw::fmt("size_t iteratedTupleSize(bool cache) const {{ return iteratedTupleSize_upper(cache) * iteratedTupleSize_lower(cache); }}") << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline IteratedTupleSizeView::TemporarySetterVector IteratedTupleSizeView::start_cache_code(const FNodeOwning& child) {
   assert(parent != nullptr);
   bool doCacheSetting = SettingBase::getSetting<bool>("codegen.cacheCountStar")->get();
   bool doCache = doCacheSetting && consideredNodes.contains(child.getNodeId());
   auto currentSizeVar = fw::fmt("size{}PL", parent->getNodeId());
   TemporarySetterVector vec;
   if (doCache) {
      // leaf of the current iterator => cache the result for speedup in other steps
      out << fw::fmt("if (useCache && std::get<{}>(*{}) != 0) {{", FTreeUtil::getSizeIdx(), FTreeUtil(*parent).iteratorVar()) << fw::endl()
          << fw::fmt("  {} *= std::get<{}>(*{});", previousSizeVar, FTreeUtil::getSizeIdx(), FTreeUtil(*parent).iteratorVar()) << fw::endl()
          << fw::fmt("}} else {{") << fw::endl()
          << fw::fmt("  size_t {} = 1;", currentSizeVar) << fw::endl();
      vec.push_back(TemporarySetterBase::create(previousSizeVar, std::move(currentSizeVar)));
   }
   return vec;
}
// ---------------------------------------------------------------------------------------------------
inline void IteratedTupleSizeView::end_cache_code(const FNodeOwning& node, TemporarySetterVector& tmpSetter) {
   if (!tmpSetter.empty()) { // now cache the result, maybe we can use it later
      tmpSetter.clear(); // undo the temporary setting
      auto currentSizeVar = fw::fmt("size{}PL", node.getNodeId());
      out << fw::fmt("if (useCache) std::get<{}>(*{}) = {};", FTreeUtil::getSizeIdx(), FTreeUtil(*parent).iteratorVar(), currentSizeVar) << fw::endl()
          << fw::fmt("{} *= {};", previousSizeVar, currentSizeVar) << fw::endl()
          << "}" << fw::endl();
   }
}
// ---------------------------------------------------------------------------------------------------
inline void IteratedTupleSizeView::visit(const FNodeOwning& node) {
   FWContainer vSize = fw::fmt("size{}", node.getNodeId());
   FWContainer vSum = fw::fmt("sum{}", node.getNodeId());

   if (!consideredNodes.contains(node.getNodeId())) { // these are the fixed nodes, we do not want to iterate over these nodes.
      for (auto& child : node.getChildren()) {
         TemporarySetter tsCurrentList(currentList, fw::get(FTreeUtil(node).getChildIdx(*child), FTreeUtil(node).iteratorVar().dereference()));
         TemporarySetter tsParent(parent, &node);
         apply(*child);
      }
   } else { // iterate over these nodes
      // dear future developer, you might hate me, but there was a deadline...
      // this should be fixed for the case, that the parent node is also a composed node and we get a tuple or something like this
      // but this is probably ugly since we have to detect which iterators come from where and how we can fix them.
      assert(!isNodeRequired(node) || !isNodeRequiredByReferenceIterator(node));
      if (!consideredNodes.contains(parent->getNodeId()) && !generatedIterators.contains(parent->getNodeId())) {
         out << fw::fmt("auto {0} = get{0}Ref();", FTreeUtil(*parent).iteratorVar()) << fw::endl();
         generatedIterators.insert(parent->getNodeId());
      }
      if (node.isLeaf()) {
         out << fw::fmt("{} *= {}.size();", previousSizeVar, currentList) << fw::endl();
      } else {
         auto doCache = start_cache_code(node);
         TemporarySetter tsIter(loopIterator, fw::fmt("iterSize{}", node.getNodeId()));
         out << fw::fmt("size_t {} = 0;", vSum) << fw::endl()
             << fw::fmt("for (auto& {} : {}) {{", loopIterator, currentList) << fw::endl()
             << fw::fmt("  auto* {} = &{};", FTreeUtil(node).iteratorVar(), loopIterator)
             << fw::fmt("  size_t {} = 1;", vSize) << fw::endl();
         for (auto& child : node.getChildren()) {
            TemporarySetter tsCurrentList(currentList, fw::get(FTreeUtil(node).getChildIdx(*child), loopIterator));
            TemporarySetter tsPreviousSizeVar(previousSizeVar, vSize);
            TemporarySetter tsParent(parent, &node);
            apply(*child);
         }
         out << fw::fmt("  {} += {};", vSum, vSize) << fw::endl()
             << fw::fmt("}}") << fw::endl()
             << fw::fmt("{} *= {};", previousSizeVar, vSum) << fw::endl();
         end_cache_code(*parent, doCache);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
