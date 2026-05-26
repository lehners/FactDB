#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Setting.hpp"
#include "factDB/newftree/generator/util/FGenUtil.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct IterableSize : public FIteratorGenerator {
   private:
   FWContainer previousSizeVar = "totalSize";

   protected:
   using TemporarySetterVector = std::vector<std::unique_ptr<TemporarySetterBase>>;

   FWContainer factorizedTableName;
   FWContainer currentList = fw::lc("tbl.cl");
   FWContainer loopIterator = fw::nop();
   bool doLoop = true;
   bool isRoot = true;

   enum stage { prepare,
                prepareParallel,
                prepareLoop,
                beforeApply,
                afterApply,
                finalizeLoop,
                finalize };

   public:
   IterableSize(FileWriter& o, const FIterator& iter, FWContainer factorizedTable) : FIteratorGenerator(o, iter), factorizedTableName(std::move(factorizedTable)){};

   void visitRoot(const FNodeOwning& node) override;
   void visit(const FNodeOwning& node) override;

   virtual TemporarySetterVector loopChildren(stage curStage, const FNodeOwning& node);
};
// ---------------------------------------------------------------------------------------------------
inline void IterableSize::visitRoot(const FNodeOwning& node) {
   bool parallel = SettingBase::getSetting<bool>("codegen.parallelFor")->get();
   if (iterator.hasReferencedIterator())
      return; // currently not implemented for reference iterators
   out << fw::fmt("static std::pair<size_t, std::vector<size_t>> iterableSize({}& tbl) {{", factorizedTableName) << fw::endl()
       << "  // returns the number of different values this iterator can take." << fw::endl()
       << fw::fmt("  {} totalSize = {};", (parallel && !iterator.getIteratedIUs().empty()) ? "std::atomic<size_t>" : "size_t", parallel ? "0" : "1") << fw::endl()
       << "  std::vector<size_t> bounds;" << fw::endl()
       << "  bounds.resize(tbl.cl.size()/50 + 1);" << fw::endl()
       << fw::endl();

   visit(node);

   out << "  return {totalSize, bounds};" << fw::endl()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void IterableSize::visit(const FNodeOwning& node) {
   if (isNodeRequiredByReferenceIterator(node)) { // these are the fixed nodes, we do not want to iterate over these nodes.
      assert(false);
      for (auto& child : node.getChildren()) {
         currentList = fw::fmt("std::get<{}>(*{})", FTreeUtil(node).getChildIdx(*child), FTreeUtil(node).iteratorVar());
         apply(*child);
      }
   } else { // iterate over these nodes
      loopChildren(prepare, node);
      TemporarySetter tsIter(loopIterator, fw::fmt("iterSize{}", node.getNodeId()));

      if (doLoop) {
         bool parallel = SettingBase::getSetting<bool>("codegen.parallelFor")->get();
         bool usePool = factDB::SettingBase::getSetting<bool>("codegen.threadPool")->get();
         if (isRoot && parallel) {
            isRoot = false;
            size_t chunkSize = SettingBase::getSetting<size_t>("codegen.chunkSize")->get();
            if (usePool)
               out << fw::fmt("ThreadPool::getPool().parallelFor(factDB::BlockedRange(0, {}.size(), {}), [&](factDB::BlockedRange range) {{", currentList, chunkSize) << fw::endl();
            else
               out << fw::fmt("tbb::parallel_for(tbb::blocked_range<size_t>(0, {}.size(), {}), [&](tbb::blocked_range<size_t> range) {{", currentList, chunkSize) << fw::endl();
            loopChildren(prepareParallel, node);
            out << fw::fmt("  size_t idx{0} = range.begin();", node.getNodeId()) << fw::endl();
            if (SettingBase::getSetting<bool>("codegen.factorized.tlsBase")->get()) {
               out << fw::fmt("  auto {}Iter = {}.begin() + idx{};", loopIterator, currentList, node.getNodeId()) << fw::endl()
                   << fw::fmt("  for (; idx{0} != range.end(); ++idx{0}, ++{1}Iter) {{", node.getNodeId(), loopIterator) << fw::endl()
                   << fw::fmt("    auto& {0} = *{0}Iter;", loopIterator) << fw::endl();
            } else {
               out << fw::fmt("  for (; idx{0} != range.end(); ++idx{0}) {{", node.getNodeId()) << fw::endl()
                   << fw::fmt("    auto& {} = {}[idx{}];", loopIterator, currentList, node.getNodeId()) << fw::endl();
            }

            loopChildren(prepareLoop, node);
            for (auto& child : node.getChildren()) {
               TemporarySetter tsCurrentList(currentList, fw::get(FTreeUtil(node).getChildIdx(*child), loopIterator));
               auto setters = loopChildren(beforeApply, node);
               apply(*child);
               loopChildren(afterApply, node);
            }

            out << fw::fmt("    if (idx{0} != 0 && idx{0} % 50 == 0) {{", node.getNodeId()) << fw::endl()
                << fw::fmt("      std::atomic_ref(bounds[(idx{0}-1)/50]) += sum{0};", node.getNodeId()) << fw::endl()
                << fw::fmt("      {} += sum{};", previousSizeVar, node.getNodeId()) << fw::endl()
                << fw::fmt("      sum{} = 0;", node.getNodeId()) << fw::endl()
                << fw::fmt("    }}") << fw::endl();
            loopChildren(finalizeLoop, node);
            out << fw::fmt("  }}") << fw::endl()
                << fw::fmt("  if (idx{0} != 0) std::atomic_ref(bounds[(idx{0}-1)/50]) += sum{0};", node.getNodeId()) << fw::endl()
                << fw::fmt("  {} += sum{};", previousSizeVar, node.getNodeId()) << fw::endl()
                << fw::fmt("}});") << fw::endl()
                << "for (size_t runningSum = 0; auto& b : bounds) {" << fw::endl()
                << "  runningSum += b;" << fw::endl()
                << "  b = runningSum;" << fw::endl()
                << "}" << fw::endl();
         } else {
            bool wasRoot = isRoot;
            isRoot = false;

            loopChildren(prepareParallel, node);
            out << fw::condition(wasRoot, "size_t clIdx = 0;") << fw::endl()
                << fw::fmt("for (auto& {} : {}) {{", loopIterator, currentList) << fw::endl();
            loopChildren(prepareLoop, node);
            for (auto& child : node.getChildren()) {
               TemporarySetter tsCurrentList(currentList, fw::get(FTreeUtil(node).getChildIdx(*child), loopIterator));
               auto setters = loopChildren(beforeApply, node);
               apply(*child);
               loopChildren(afterApply, node);
            }
            loopChildren(finalizeLoop, node);

            if (wasRoot)
               out << "if (clIdx != 0 && clIdx % 50 == 0) {" << fw::endl()
                   << fw::fmt("bounds[(clIdx-1) / 50] = sum{};", node.getNodeId()) << fw::endl()
                   << "}" << fw::endl()
                   << "++clIdx;" << fw::endl();
            out << "}" << fw::endl();
            loopChildren(finalize, node);
            if (wasRoot)
               out << "assert(clIdx == 0 || (clIdx-1) / 50 == bounds.size()-1);" << fw::endl()
                   << "bounds.back() = totalSize;" << fw::endl();
         }
      } else if (isRoot) { // !doLoop
         out << "bounds.clear();" << fw::endl();
      }
   }
}
// ---------------------------------------------------------------------------------------------------
inline IterableSize::TemporarySetterVector IterableSize::loopChildren(stage curStage, const FNodeOwning& node) {
   bool sizePerList = factDB::SettingBase::getSetting<bool>("codegen.factorized.sizePerList")->get();
   TemporarySetterVector ret;
   FWContainer vSize = fw::fmt("size{}", node.getNodeId());
   FWContainer vSum = fw::fmt("sum{}", node.getNodeId());
   switch (curStage) {
      case prepare:
         if (!isNodeRequired(node)) {
            if (node.isLeaf()) {
               out << fw::fmt("{} *= !{}.empty(); // node{} not required, check if empty", previousSizeVar, currentList, node.getNodeId()) << fw::endl();
               doLoop = false;
            } else {
               doLoop = true;
            }
         } else if (node.isLeaf()) {
            out << fw::fmt("{} {}= {}.size();", previousSizeVar, isRoot ? "" : "*", currentList) << fw::endl();
            doLoop = false;
         } else {
            doLoop = true;
         }
         break;
      case prepareParallel:
         out << fw::fmt("size_t {} = 0;", vSum) << fw::endl();
         break;
      case prepareLoop:
         out << fw::fmt("  size_t {} = 1;", vSize) << fw::endl();
         break;
      case beforeApply:
         ret.push_back(TemporarySetterBase::create(previousSizeVar, std::move(vSize)));
         break;
      case afterApply: break;
      case finalizeLoop:
         if (sizePerList && isNodeRequired(node))
            out << fw::fmt("std::get<{}>({}) = {};", FTreeUtil::getSizeIdx(), loopIterator, vSize) << fw::endl();
         out << fw::fmt("{} += {};", vSum, vSize) << fw::endl();
         if (!isNodeRequired(node))
            out << fw::fmt("if ({} != 0) break;  // only check for non empty children, nodes are not required", vSum) << fw::endl();
         break;
      case finalize:
         if (!node.isLeaf() && isNodeRequired(node))
            out << fw::fmt("{} *= {};", previousSizeVar, vSum) << fw::endl();
         else if (!node.isLeaf())
            out << fw::fmt("{} *= std::min<size_t>({}, 1);", previousSizeVar, vSum) << fw::endl();
         doLoop = true;
         break;
   }
   return ret;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
