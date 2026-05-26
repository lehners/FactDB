#pragma once
// ---------------------------------------------------------------------------------------------------
#include "ListTypes.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/newftree/generator/iterator/IteratorVars.hpp"
#include "factDB/newftree/generator/util/DeriveBeforeMergeMap.hpp"
#include "factDB/newftree/generator/util/FGenUtil.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
#include <unordered_map>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct AppendEmptyTree : public FIteratorGenerator {
   FTreeTranslator mergedFTreeTranslator;
   FHandle lastHandle;
   DeriveBeforeMergeMap::IUMap iu2beforeNodes;
   DeriveBeforeMergeMap::ParentMap node2parent;
   bool isMergingNode = true;
   bool doCaching = false;

   AppendEmptyTree(FileWriter& o, const FTree& mergedFTree, bool caching)
      : FIteratorGenerator(o, *mergedFTree.getTopIterator()), mergedFTreeTranslator(mergedFTree, "container"), doCaching(caching) {
      auto res = DeriveBeforeMergeMap::getChildTreesMap(mergedFTree);
      iu2beforeNodes = std::move(res.first);
      node2parent = std::move(res.second);
   }

   void visit(const FNodeOwning& mergedFTreeNode) override;
   void visitRoot(const FNodeOwning& node) override;

   void genCode(const FNodeOwning& node, bool isRoot);
   void reserveAllocationSize(const FNodeOwning& afterInsertTreeNode, bool isRoot);
};
// ---------------------------------------------------------------------------------------------------
inline void AppendEmptyTree::visitRoot(const FNodeOwning& afterInsertTreeNode) {
   auto useLocalStorage = SettingBase::getSetting<bool>("codegen.factorized.tlsBase")->get();
   auto localReference = useLocalStorage ? fw::fmt(", {}::{}::reference local", mergedFTreeTranslator.classname(), FTreeUtil(afterInsertTreeNode).listType()) : fw::nop();
   auto handleStorage = doCaching ? ", HandleStorage& hs" : fw::nop();
   out << fw::fmt("auto appendEmptyTree(const {}& iterator{}{}) {{", FTreeUtil::getQualifiedIteratorName(*mergedFTreeTranslator.getFTree().getTopIterator()), std::move(localReference), std::move(handleStorage)) << fw::endl()
       << fw::condition(doCaching, "bool useCached = true;") << fw::endl_non_empty()
       << fw::condition(!doCaching, "HandleStorage hs;") << fw::endl_non_empty()
       << "[[maybe_unused]] size_t size2reserve = 1;" << fw::endl()
       << "auto& container = *this;" << fw::endl();

   genCode(afterInsertTreeNode, true);
   out << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void AppendEmptyTree::visit(const FNodeOwning& afterInsertTreeNode) {
   genCode(afterInsertTreeNode, false);
}
// ---------------------------------------------------------------------------------------------------
inline void AppendEmptyTree::genCode(const FNodeOwning& afterInsertTreeNode, bool isRoot) {
   if (doCaching) {
      std::unordered_set<size_t> seenIds = {};
      for (const IU* iu : afterInsertTreeNode.getIUs()) { // collect all required iterators in map to eliminate iterators with multiple ius
         assert(iu2beforeNodes.contains(iu));
         seenIds.insert(iu2beforeNodes.find(iu)->second.node.getNodeId());
      }
      out << "useCached &= " << fw::pushSeparator(" && ");
      for (auto sId : seenIds)
         out << fw::separator() << fw::fmt("hs.iter{0} == iterator.getiter{0}Ref()", sId);
      out << fw::popSeparator() << fw::sendl()
          << "if (!useCached) {" << fw::endl();
      for (auto sId : seenIds)
         out << fw::fmt("hs.iter{0} = iterator.getiter{0}Ref()", sId) << fw::sendl();
   }

   FTreeTranslator beforeInsertTopTranslator(*mergedFTreeTranslator.getFTree().getTopTree());
   for (const IU* iu : afterInsertTreeNode.getIUs())
      out << fw::fmt("auto& {} = iterator.get<{}>();", iu, beforeInsertTopTranslator.iuEnum(*iu)) << fw::endl();
   FHandle afterHandle("hs.handle", afterInsertTreeNode.getNodeId());

   FWContainer localContainer = isRoot && SettingBase::getSetting<bool>("codegen.factorized.tlsBase")->get() ? "local" : fw::nop();
   FHandle childHandle;
   out << fw::fmt("{} = {}", afterHandle.gen(), mergedFTreeTranslator.push_back(lastHandle, childHandle, afterInsertTreeNode.getIUs(), localContainer)) << fw::endl();

   TemporarySetter ts(lastHandle, afterHandle);
   const auto& lowerRoot = mergedFTreeTranslator.getFTree().getLowerTree()->getRootNode();
   for (const auto& child : afterInsertTreeNode.getChildren()) {
      if (child.get() == afterInsertTreeNode.getMergedNode())
         continue;
      if (lowerRoot.containsIU(child->getIUs().front()))
         continue; // ignore nodes, which stem from the lower part of the tree.
      assert(!child->getIUs().empty());

      if (child->type == FNode::Reference) {
         out << fw::fmt("std::get<{}>(*{}).merge(iterator.get<{}>());", FTreeUtil(afterInsertTreeNode).getChildIdx(*child), afterHandle.gen(), beforeInsertTopTranslator.iuEnum(*child->getIUs().front())) << fw::endl();
      } else if (static_cast<const FNodeOwning*>(child.get()) != afterInsertTreeNode.getMergedNode()) {
         TemporarySetter tsMergingNode(isMergingNode, false);
         apply(*child);
      }
   }

   reserveAllocationSize(afterInsertTreeNode, isRoot);
   if (doCaching)
      out << "}" << fw::endl();

   if (afterInsertTreeNode.getMergedNode() != nullptr) {
      apply(*afterInsertTreeNode.getMergedNode());
   } else if (isMergingNode) {
      // the node, which does not have a merged child in the path, is the node, where the other tree is inserted
      if (doCaching) {
         out << fw::fmt("return {};", afterHandle.gen()) << fw::endl(); // return the handle
      } else {
         out << fw::fmt("return std::make_tuple({}, hs);", afterHandle.gen()) << fw::endl(); // return the handle
      }
   }
}
// ---------------------------------------------------------------------------------------------------
inline void AppendEmptyTree::reserveAllocationSize(const FNodeOwning& afterInsertTreeNode, [[maybe_unused]] bool isRoot) {
   const auto& mergedNode = afterInsertTreeNode.getMergedNode();
   if (mergedNode != nullptr && !mergedFTreeTranslator.getFTree().getTopTree()->isComposedTree()) { // reserve only if not composed nodes are used
      // if std::vector grows, it may have to relocate. Hence, the iterators are invalidated. Therefore, we want to reserve the child node.
      // but we may want to remove this for deque and other structures, where this is no issue
      out << "size2reserve = 1;" << fw::endl();
      std::unordered_set<size_t> iteratedChildNodes;
      for (const IU* iu : afterInsertTreeNode.getMergedNode()->getIUs()) { // collect the previous nodes
         assert(iu2beforeNodes.contains(iu));
         iteratedChildNodes.insert(iu2beforeNodes.find(iu)->second.node.getNodeId());
      }
      for (size_t curChild : iteratedChildNodes) { // calc the size to reserve
         assert(node2parent.contains(curChild));
         const auto& parentInfo = node2parent[curChild];
         auto childIdx = FTreeUtil(*parentInfo.parent).getChildIdx(parentInfo.childIdx);
         out << fw::fmt("size2reserve *= std::get<{}>(*iterator.iter{}).size();", childIdx, parentInfo.parent->getNodeId()) << fw::endl();
      }
      // reserve the size of the node
      out << mergedFTreeTranslator.reserve(lastHandle, *mergedNode->getIUs().front(), "size2reserve") << fw::sendl();
   } else {
      assert(FNodeListTypes::getListType(isRoot, afterInsertTreeNode.needsLock()).first != factDB::infra::list::ListTypeEnum::STLVector);
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
