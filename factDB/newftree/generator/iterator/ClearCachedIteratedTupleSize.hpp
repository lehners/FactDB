#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/fnode/Size.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct ClearCachedIteratedTupleSize : public FIteratorGenerator {
   FWContainer factorizedTable;
   FWContainer currentList = fw::lc("fti.cl");
   bool doLoop = true;

   public:
   ClearCachedIteratedTupleSize(FileWriter& o, const FIterator& iter, FWContainer factorizedTable_) : FIteratorGenerator(o, iter), factorizedTable(std::move(factorizedTable_)){};

   void visitRoot(const FNodeOwning& node) override;
   void visit(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void ClearCachedIteratedTupleSize::visitRoot(const FNodeOwning& node) {
   out << fw::fmt("static void clearCachedIteratedTupleSize([[maybe_unused]] {}& fti) {{", factorizedTable) << fw::endl();
   if (!SettingBase::getSetting<bool>("codegen.cacheCountStar")->get()) {
      out << "}" << fw::endl();
      return;
   }
   bool parallel = SettingBase::getSetting<bool>("codegen.parallelFor")->get();
   bool usePool = factDB::SettingBase::getSetting<bool>("codegen.threadPool")->get();
   if (parallel) {
      size_t chunkSize = SettingBase::getSetting<size_t>("codegen.chunkSize")->get();
      if (usePool)
         out << fw::fmt("ThreadPool::getPool().parallelFor(BlockedRange(0, {}.size(), {}), [&](BlockedRange range) {{", currentList, chunkSize) << fw::endl();
      else
         out << fw::fmt("tbb::parallel_for(tbb::blocked_range<size_t>(0, {}.size(), {}), [&](tbb::blocked_range<size_t> range) {{", currentList, chunkSize) << fw::endl();
      out << fw::fmt("  size_t idx = range.begin();") << fw::endl()
          << fw::fmt("  for (auto iterClear{0} = {1}.begin() + idx; idx != range.end(); ++iterClear{0}, ++idx) {{", node.getNodeId(), currentList) << fw::endl();
   } else {
      out << fw::fmt("for (auto iterClear{0} = {1}.begin(); !iterClear{0}.isLast(); ++iterClear{0}) {{", node.getNodeId(), currentList) << fw::endl();
   }

   out << fw::fmt("std::get<{}>(*iterClear{}) = 0;", FTreeUtil::getSizeIdx(), node.getNodeId()) << fw::endl();
   for (auto& child : node.getChildren()) {
      if (!isNodeRequired(*child))
         continue;
      currentList = fw::get(FTreeUtil(node).getChildIdx(*child), fw::fmt("*iterClear{}", node.getNodeId()));
      apply(*child);
   }

   out << "  }" << fw::endl()
       << fw::condition(parallel, "});") << fw::endl_non_empty()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void ClearCachedIteratedTupleSize::visit(const FNodeOwning& node) {
   out << fw::fmt("for (auto& iterClear{} : {}) {{", node.getNodeId(), currentList) << fw::endl()
       << fw::fmt("  std::get<{}>(iterClear{}) = 0;", FTreeUtil::getSizeIdx(), node.getNodeId()) << fw::endl();
   for (auto& child : node.getChildren()) {
      if (!isNodeRequired(*child))
         continue;
      currentList = fw::get(FTreeUtil(node).getChildIdx(*child), fw::fmt("iterClear{}", node.getNodeId()));
      apply(*child);
   }

   out << "  }" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------