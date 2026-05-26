#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FGenUtil.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
#include <optional>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct IteratedTupleSize : public FIteratorGenerator {
   private:
   FWContainer previousSizeVar = "unspecified";

   protected:
   using TemporarySetterVector = std::vector<std::unique_ptr<TemporarySetterBase>>;

   FWContainer currentList = "unspecified";
   FWContainer loopIterator = "unspecified";

   public:
   IteratedTupleSize(FileWriter& o, const FIterator& iter) : FIteratorGenerator(o, iter){};

   void visitRoot(const FNodeOwning& node) override;
   void visit(const FNodeOwning& node) override;

   TemporarySetterVector start_cache_code(const FNodeOwning& node);
   void end_cache_code(const FNodeOwning& node, TemporarySetterVector& tmpSetter);
};
// ---------------------------------------------------------------------------------------------------
inline void IteratedTupleSize::visitRoot(const FNodeOwning& node) {
   FWContainer vSize = fw::fmt("size{}", node.getNodeId());
   FWContainer vSum = fw::fmt("totalSize");
   out << "size_t iteratedTupleSize([[maybe_unused]] bool useCache=true) const {" << fw::endl();

   bool iterateFirst = (iterator.hasReferencedIterator() && !isNodeRequiredByReferenceIterator(node) && !isNodeRequired(node)) || iterator.getIteratedIUs().empty();
   if (!iterateFirst) { // root is fixed = required case
      out << fw::fmt("size_t {} = 0;", vSum) << fw::endl()
          << fw::fmt("size_t {} = 1;", vSize) << fw::endl();
      TemporarySetter tsPreviousSizeVar(previousSizeVar, vSize);

      auto curLastVarTmp = start_cache_code(node);
      for (auto& child : node.getChildren()) {
         TemporarySetter tsCurrentList(currentList, fw::get(FTreeUtil(node).getChildIdx(*child), FTreeUtil(node).iteratorVar().dereference()));
         apply(*child);
      }
      end_cache_code(node, curLastVarTmp);
      out << fw::fmt("  {} += {};", vSum, vSize) << fw::endl();
   } else {
      TemporarySetter tsIter(loopIterator, fw::fmt("iterSize{}", node.getNodeId()));
      out << fw::fmt("size_t {} = 0;", vSum) << fw::endl()
          << fw::fmt("for (auto iterSize{0} = {1}; !iterSize{0}.isLast(); ++iterSize{0}) {{", node.getNodeId(), FTreeUtil(node).iteratorVar()) << fw::endl()
          << fw::fmt("  size_t {} = 1;", vSize) << fw::endl();

      auto curLastVarTmp = start_cache_code(node);
      for (auto& child : node.getChildren()) {
         // need to dereference due to other loop
         TemporarySetter tsCurrentList(currentList, fw::get(FTreeUtil(node).getChildIdx(*child), const_cast<const FWContainer&>(loopIterator).dereference()));
         TemporarySetter tsPreviousSizeVar(previousSizeVar, vSize);
         apply(*child);
      }
      end_cache_code(node, curLastVarTmp);
      out << fw::fmt("  {} += {};", vSum, vSize) << fw::endl()
          << fw::fmt("}}") << fw::endl();
   }

   out << "  return totalSize;" << fw::endl()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline IteratedTupleSize::TemporarySetterVector IteratedTupleSize::start_cache_code(const FNodeOwning& node) {
   bool doCacheSetting = SettingBase::getSetting<bool>("codegen.cacheCountStar")->get();
   bool doCache = doCacheSetting && isNodeInIteratorContained(node) && hasNotRequiredChild(node) && !hasRequiredChild(node);
   auto currentSizeVar = fw::fmt("size{}PL", node.getNodeId());
   TemporarySetterVector vec;
   if (doCache) {
      // leaf of the current iterator => cache the result for speedup in other steps
      out << fw::fmt("if (useCache && std::get<{}>(*{}) != 0) {{", FTreeUtil::getSizeIdx(), FTreeUtil(node).iteratorVar()) << fw::endl()
          << fw::fmt("  {} *= std::get<{}>(*{});", previousSizeVar, FTreeUtil::getSizeIdx(), FTreeUtil(node).iteratorVar()) << fw::endl()
          << fw::fmt("}} else {{") << fw::endl()
          << fw::fmt("  size_t {} = 1;", currentSizeVar) << fw::endl();
      vec.push_back(TemporarySetterBase::create(previousSizeVar, std::move(currentSizeVar)));
   }
   return vec;
}
// ---------------------------------------------------------------------------------------------------
inline void IteratedTupleSize::end_cache_code(const FNodeOwning& node, TemporarySetterVector& tmpSetter) {
   if (!tmpSetter.empty()) { // now cache the result, maybe we can use it later
      tmpSetter.clear(); // undo the temporary setting
      auto currentSizeVar = fw::fmt("size{}PL", node.getNodeId());
      out << fw::fmt("if (useCache) std::get<{}>(*{}) = {};", FTreeUtil::getSizeIdx(), FTreeUtil(node).iteratorVar(), currentSizeVar) << fw::endl()
          << fw::fmt("{} *= {};", previousSizeVar, currentSizeVar) << fw::endl()
          << "}" << fw::endl();
   }
}
// ---------------------------------------------------------------------------------------------------
inline void IteratedTupleSize::visit(const FNodeOwning& node) {
   FWContainer vSize = fw::fmt("size{}", node.getNodeId());
   FWContainer vSum = fw::fmt("sum{}", node.getNodeId());

   auto doCache = start_cache_code(node);
   if (isNodeRequired(node)) { // these are the fixed nodes, we do not want to iterate over these nodes.
      for (auto& child : node.getChildren()) {
         TemporarySetter tsCurrentList(currentList, fw::get(FTreeUtil(node).getChildIdx(*child), FTreeUtil(node).iteratorVar().dereference()));
         apply(*child);
      }
   } else { // iterate over these nodes
      assert(!isNodeRequired(node) || !isNodeRequiredByReferenceIterator(node));
      if (node.isLeaf()) {
         out << fw::fmt("{} *= {}.size();", previousSizeVar, currentList) << fw::endl();
      } else {
         TemporarySetter tsIter(loopIterator, fw::fmt("iterSize{}", node.getNodeId()));
         out << fw::fmt("size_t {} = 0;", vSum) << fw::endl()
             << fw::fmt("for (auto& {} : {}) {{", loopIterator, currentList) << fw::endl()
             << fw::fmt("  size_t {} = 1;", vSize) << fw::endl();
         for (auto& child : node.getChildren()) {
            TemporarySetter tsCurrentList(currentList, fw::get(FTreeUtil(node).getChildIdx(*child), loopIterator));
            TemporarySetter tsPreviousSizeVar(previousSizeVar, vSize);
            apply(*child);
         }
         out << fw::fmt("  {} += {};", vSum, vSize) << fw::endl()
             << fw::fmt("}}") << fw::endl()
             << fw::fmt("{} *= {};", previousSizeVar, vSum) << fw::endl();
      }
   }
   end_cache_code(node, doCache);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
