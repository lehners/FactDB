#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/FTree.hpp"
#include "factDB/newftree/generator/util/FNodeGenerator.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct Generator {
   static std::vector<FWContainer> getRequiredHeaders();
   static void genStruct(const FTree& tree, fw::FileWriter& out, std::unordered_set<const FTree*>& generatedTrees);
   static void genView(const FIterator& iterator, fw::FileWriter& out);
   static void genIterator(const FIterator& iterator, fw::FileWriter& out, std::unordered_set<size_t>& generatedIterators);
};
// ---------------------------------------------------------------------------------------------------
namespace FTreeGenerator {
// ---------------------------------------------------------------------------------------------------
template <std::derived_from<FNodeGenerator> T>
FWContainer applyGenerator(const FTree& tree) {
   return fw::func([&tree](FileWriter& out) {
      T generator(out, tree);
      generator.apply(tree.getRootNode());
   });
}
// ---------------------------------------------------------------------------------------------------
template <std::derived_from<FNodeGenerator> T>
FWContainer applyGenerator(const FTree& tree, bool flag) {
   return fw::func([&tree, flag](FileWriter& out) {
      T generator(out, tree, flag);
      generator.apply(tree.getRootNode());
   });
}
// ---------------------------------------------------------------------------------------------------
template <std::derived_from<FNodeGenerator> T>
FWContainer applyGenerator(const FNode& node) {
   return fw::func([&node](FileWriter& out) {
      T generator(out);
      generator.apply(node);
   });
}
// ---------------------------------------------------------------------------------------------------
template <std::derived_from<FNodeGenerator> T>
FWContainer applyGenerator(const FNode& node, bool val) {
   return fw::func([&node, val](FileWriter& out) {
      T generator(out, val);
      generator.apply(node);
   });
}
// ---------------------------------------------------------------------------------------------------
template <std::derived_from<FNodeGenerator> T>
FWContainer applyGenerator(const FNode& node, std::unique_ptr<FTree>& otherTree) {
   return fw::func([&node, &otherTree](FileWriter& out) {
      T generator(out, otherTree);
      generator.apply(node);
   });
}
// ---------------------------------------------------------------------------------------------------
template <std::derived_from<FNodeGenerator> T>
FWContainer applyGenerator(const FIterator& iter) {
   return fw::func([&iter](FileWriter& out) {
      T generator(out, iter);
      generator.apply(iter.getOwner().getRootNode());
   });
}
// ---------------------------------------------------------------------------------------------------
template <std::derived_from<FNodeGenerator> T>
FWContainer applyGenerator(const FNode& node, const FIterator& iter) {
   return fw::func([&node, &iter](FileWriter& out) {
      T generator(out, iter);
      generator.apply(node);
   });
}
// ---------------------------------------------------------------------------------------------------
template <std::derived_from<FNodeGenerator> T>
FWContainer applyGenerator(const FIterator& iter, FWContainer container) {
   return fw::func([&iter, &container](FileWriter& out) {
      T generator(out, iter, container);
      generator.apply(iter.getOwner().getRootNode());
   });
}
// ---------------------------------------------------------------------------------------------------
template <std::derived_from<FNodeGenerator> T>
FWContainer applyGenerator(const FIterator& iter, FTree& thisTree, FTree& otherTree) {
   return fw::func([&iter, &thisTree, &otherTree](FileWriter& out) {
      T generator(out, iter, thisTree, otherTree);
      generator.apply(otherTree.getRootNode());
   });
}
// ---------------------------------------------------------------------------------------------------
template <std::derived_from<FNodeGenerator> T>
FWContainer applyGenerator(const FIterator& iter, FTree& otherTree) {
   return fw::func([&iter, &otherTree](FileWriter& out) {
      T generator(out, iter, otherTree);
      generator.apply(otherTree.getRootNode());
   });
}
// ---------------------------------------------------------------------------------------------------
} // namespace FTreeGenerator
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------