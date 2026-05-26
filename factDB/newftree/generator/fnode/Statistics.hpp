#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/generator/util/FTreeTraversal.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct FNodeStatistics : public FTreeTraversalGenerator {
   FNodeStatistics(FileWriter& o) : FTreeTraversalGenerator(o){};
   void visitRoot(const FNodeOwning& node) override;
   TemporarySetterVector loopChildren(stage curStage, const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void FNodeStatistics::visitRoot(const FNodeOwning& node) {
   out << "struct tupleStats { size_t min = std::numeric_limits<size_t>::max(), max = 0, sum = 0, ctr = 0; };" << fw::endl()
       << "void genStatistics() const {" << fw::endl()
       << "  std::unordered_map<size_t, tupleStats> stats;" << fw::endl();
   visit(node);

   out << "  for (auto& [key, tStat]: stats) {" << fw::endl()
       << R"(  std::cout << key << ": min: " << tStat.min << ", max: " << tStat.max << ", sum: " << tStat.sum  << ", ctr: " <<  tStat.ctr << ", avg: " << tStat.sum * 1.0 / tStat.ctr << std::endl;)" << fw::endl()
       << "  }" << fw::endl()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline FNodeStatistics::TemporarySetterVector FNodeStatistics::loopChildren(stage curStage, const FNodeOwning& node) {
   FWContainer vIter = fw::fmt("iter{}", node.getNodeId());
   FWContainer vStats = fw::fmt("statsVar{}", node.getNodeId());

   switch (curStage) {
      case prepare:
         out << fw::fmt("auto& {} = stats[{}];", vStats, node.getNodeId()) << fw::endl()
             << fw::fmt("{0}.min = std::min({0}.min, {1}.size());", vStats, currentList) << fw::endl()
             << fw::fmt("{0}.max = std::max({0}.max, {1}.size());", vStats, currentList) << fw::endl()
             << fw::fmt("{0}.sum += {1}.size();", vStats, currentList) << fw::endl()
             << fw::fmt("{}.ctr++;", vStats) << fw::endl();
         doLoop = !node.getChildren().empty();
         break;
      case prepareLoop:
      case beforeApply:
      case afterApply:
      case finalizeLoop:
      case finalize:
         break;
   }
   return {};
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
