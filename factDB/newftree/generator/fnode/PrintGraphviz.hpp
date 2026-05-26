#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/FNode.hpp"
#include "factDB/newftree/generator/util/FTreeTraversal.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct FNodePrinterGraphviz : public FTreeTraversalGenerator {
   private:
   size_t parentFactorizedNodeId = 0;
   size_t previousParentFactorizedNodeId = 0;

   public:
   FNodePrinterGraphviz(FileWriter& o) : FTreeTraversalGenerator(o){};

   void visitRoot(const FNodeOwning& node) override;
   TemporarySetterVector loopChildren(stage curStage, const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
inline void FNodePrinterGraphviz::visitRoot(const FNodeOwning& node) {
   out << "void printGraphviz(FileWriter& out) const {" << fw::endl()
       << "out << fw::checkQuotes(false) << fw::verbosity(false);" << fw::endl()
       << "out << \"digraph structs {\" << fw::endl();" << fw::endl()
       << "out << \"  node [shape=record];\" << fw::endl();" << fw::endl()
       << "std::unordered_map<size_t, size_t> node2idx;" << fw::endl()
       << "size_t i0 = 0;" << fw::endl()
       << "{" << fw::endl();
   visit(node);
   out << "}" << fw::endl()
       << "out << \"}\" << fw::endl();" << fw::endl()
       << "out << fw::checkQuotes(true) << fw::verbosity(true);" << fw::endl()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline FNodePrinterGraphviz::TemporarySetterVector FNodePrinterGraphviz::loopChildren(stage curStage, const FNodeOwning& node) {
   bool sizePerList = factDB::SettingBase::getSetting<bool>("codegen.factorized.sizePerList")->get();

   FWContainer vFirst = fw::fmt("first{}", node.getNodeId());
   switch (curStage) {
      case prepare:
         out << fw::pushSeparator(" | ") << fw::checkQuotes(false)
             << fw::fmt(R"(out << "struct{0}_" << i{1} << " [label=\"{{ {{ Node {0} }} | {{ {{ {2}{3} |  }} | " << fw::pushSeparator("|");)", node.getNodeId(), parentFactorizedNodeId, fw::condition(sizePerList, "size | "), fw::iter(node.getIUs())) << fw::endl() << fw::popSeparator()
             << fw::fmt("if (!node2idx.contains({0})) node2idx[{0}] = 0;", node.getNodeId()) << fw::endl()
             << fw::fmt("size_t i{0} = node2idx[{0}];", node.getNodeId()) << fw::endl()
             << fw::fmt("for (auto& iter{} : {}) {{", node.getNodeId(), currentList) << fw::endl();
         for (const IU* iu : node.getIUs()) {
            out << fw::fmt("auto& {} = std::get<{}>(iter{}).value;", iu, FTreeUtil(node).getIUIdx(*iu), node.getNodeId()) << fw::endl();
         }
         out << fw::pushSeparator(" << \"|\" << ")
             << "out << fw::separator() << \"{\" << ";
         if (sizePerList)
            out << fw::fmt("std::get<{}>(iter{}) << \"|\" << ", FTreeUtil::getSizeIdx(), node.getNodeId());
         out << fw::fmt(R"({0} << "|<a{1}_" << i{1}++ << ">}}";)", fw::iter(node.getIUs()), node.getNodeId()) << fw::endl() << fw::popSeparator()
             << "}" << fw::endl()
             << R"(out << "} } \"];" << fw::endl() << fw::popSeparator();)" << fw::endl() << fw::checkQuotes(true)
             << fw::endl();
         if (node.isLeaf()) {
            doLoop = false;
         } else {
            out << fw::fmt("i{0} = node2idx[{0}];", node.getNodeId()) << fw::endl();
            doLoop = true;
         }
         break;
      case prepareLoop: break;
      case beforeApply: {
         TemporarySetterVector ret;
         auto parentFactorizedNodeIdCopy = parentFactorizedNodeId;
         ret.push_back(TemporarySetterBase::create(previousParentFactorizedNodeId, std::move(parentFactorizedNodeIdCopy)));
         ret.push_back(TemporarySetterBase::create(parentFactorizedNodeId, node.getNodeId()));
         return ret;
      }
      case afterApply:
         if (!node.isLeaf())
            out << fw::fmt(R"(out << "struct{1}_" << i{0} << ":a{1}_" << i{1} << " -> struct{2}_" << i{1} << fw::endl();)", previousParentFactorizedNodeId, node.getNodeId(), curChild->getNodeId()) << fw::endl()
                << fw::endl();
         break;
      case finalizeLoop:
         out << fw::fmt("i{}++;", node.getNodeId()) << fw::endl();
         break;
      case finalize:
         out << fw::fmt("node2idx[{0}] = i{0};", node.getNodeId()) << fw::endl();
         break;
   }
   return {};
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
