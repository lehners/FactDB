#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/FTree.hpp"
#include "factDB/newftree/generator/util/FIteratorGenerator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::factorized_table_generator {
// ---------------------------------------------------------------------------------------------------
struct InsertAsChildIntoOther : public FIteratorGenerator {
   enum Stage {
      HandleStorage,
      InsertFunc,
   };
   FTree& thisStructure;
   FTree& otherInstance;
   FHandle parentHandle{"parentHandle", 0};
   const FNode* parentNode;
   Stage curStage = HandleStorage;
   IUSet previousIUs;
   bool isStorageEmtpy = true;

   InsertAsChildIntoOther(FileWriter& o, FIterator& iter, FTree& thisTree, FTree& otherTree) // NOLINT(bugprone-easily-swappable-parameters)
      : FIteratorGenerator(o, iter), thisStructure(thisTree), otherInstance(otherTree) {}

   void visit(const FNodeOwning&) override;
   void visit(const FNodeReferencing&) override;
   void visitRoot(const FNodeOwning& node) override;
};
// ---------------------------------------------------------------------------------------------------
// Since the handle is only owned by us and nobody can access it, it should be save to use single-threaded containers.
inline void InsertAsChildIntoOther::visitRoot(const FNodeOwning& /*beforeInsertRootNode*/) {
   auto& afterInsertStruct = otherInstance;
   // at the moment, we know that the child of the root in the new tree is the inserted node.
   assert(afterInsertStruct.getRootNode().getChildren().size() == 1);
   parentNode = &afterInsertStruct.getRootNode().getOwningNode();
   const FNode& insertedNode = *afterInsertStruct.getRootNode().getChildren().front();
   out << "struct HandleStorage {" << fw::endl();
   apply(insertedNode);
   curStage = InsertFunc;
   out << "};" << fw::endl()
       << "static HandleStorage createHandleStorage() { return {};}" << fw::endl()
       << fw::fmt("void insertAsChildIntoOther({} container, {}::{}& {}, {}HandleStorage& hs) const {{", FTreeTranslator(otherInstance).reference(), FTreeTranslator(afterInsertStruct).classname(), FTreeUtil(afterInsertStruct.getRootNode()).iteratorType(), parentHandle.gen(), isStorageEmtpy ? "[[maybe_unused]]" : "") << fw::endl()
       << "bool useCached = true;" << fw::endl();
   apply(insertedNode);
   out << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void InsertAsChildIntoOther::visit(const FNodeOwning& node) {
   IUSet currentIUs = previousIUs;
   currentIUs.insert(node.getIUs().front());
   if (curStage == HandleStorage) {
      if (hasRequiredChild(node)) {
         out << "std::unordered_map<std::tuple<" << fw::pushSeparator(", ");
         for (auto& iu : currentIUs)
            out << fw::separator() << iu->type << "*";
         out << fw::popSeparator()
             << fw::fmt(">, {}::{}::Iterator, HashTuple> handle{}map;", FTreeTranslator(otherInstance).classname(), FTreeUtil(node).listType(), node.getNodeId()) << fw::endl();
         TemporarySetter ts(previousIUs, currentIUs);
         recurseChildren(node);
         isStorageEmtpy = true;
      }
   } else {
      assert(curStage == InsertFunc);
      // collect the values
      for (auto& iu : node.getIUs()) {
         out << fw::fmt("[[maybe_unused]] auto& {0} = get<{1}::{0}>();", iu, FTreeTranslator(thisStructure).classname()) << fw::endl();
      }

      // push elements into newly created node if necessary to create a new one
      FHandle newHandle("handle");
      bool hasRequiredChildrenVal = hasRequiredChild(node);
      if (hasRequiredChildrenVal) {
         out << "auto tuple" << node.getNodeId() << " = std::make_tuple(" << fw::pushSeparator(", ");
         for (auto& iu : currentIUs)
            out << fw::separator() << "&" << iu;
         out << fw::popSeparator() << ");" << fw::endl()
             << fw::fmt("useCached &= hs.handle{}map.contains(tuple{});", node.getNodeId(), node.getNodeId()) << fw::endl()
             << "if (!useCached) {" << fw::endl()
             << FTreeTranslator(otherInstance).push_back(parentHandle, newHandle, node.getIUs()) << fw::endl()
             << fw::fmt("hs.handle{0}map[tuple{0}] = std::move({1});", node.getNodeId(), newHandle.gen()) << fw::endl();
      } else {
         out << "[[maybe_unused]] " << FTreeTranslator(otherInstance).push_back(parentHandle, newHandle, node.getIUs()) << fw::endl();
      }

      TemporarySetter tsPH(parentHandle, newHandle);
      TemporarySetter<const FNode*> tsPN(parentNode, &node);
      TemporarySetter tsPIUs(previousIUs, currentIUs);
      recurseNotRequiredChildren(node);

      if (hasRequiredChildrenVal) {
         out << fw::fmt("}}") << fw::endl()
             << fw::fmt("auto {0} = hs.handle{1}map[tuple{1}];", newHandle.gen(), node.getNodeId()) << fw::endl();
      }

      recurseRequiredChildren(node);
   }
}
// ---------------------------------------------------------------------------------------------------
inline void InsertAsChildIntoOther::visit(const FNodeReferencing& node) {
   if (curStage == InsertFunc) {
      out << fw::fmt("std::get<{}>(*{}).merge(get<{}::{}>());", FTreeUtil(*parentNode).getChildIdx(node), parentHandle.gen(), FTreeTranslator(thisStructure).classname(), node.getIUs().front()) << fw::endl();
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::factorized_table_generator
// ---------------------------------------------------------------------------------------------------
