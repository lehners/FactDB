#include "factDB/newftree/generator/GenerateFTree.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/newftree/generator/fnode/AppendEmptyTree.hpp"
#include "factDB/newftree/generator/fnode/Graphviz.hpp"
#include "factDB/newftree/generator/fnode/HandleStorage.hpp"
#include "factDB/newftree/generator/fnode/InsertAsChild.hpp"
#include "factDB/newftree/generator/fnode/InsertHandleStorage.hpp"
#include "factDB/newftree/generator/fnode/ListTypes.hpp"
#include "factDB/newftree/generator/fnode/Print.hpp"
#include "factDB/newftree/generator/fnode/PrintGraphviz.hpp"
#include "factDB/newftree/generator/fnode/PushBack.hpp"
#include "factDB/newftree/generator/fnode/Reserve.hpp"
#include "factDB/newftree/generator/fnode/Size.hpp"
#include "factDB/newftree/generator/fnode/Statistics.hpp"
#include "factDB/newftree/generator/iterator/Begin.hpp"
#include "factDB/newftree/generator/iterator/BeginAt.hpp"
#include "factDB/newftree/generator/iterator/ClearCachedIteratedTupleSize.hpp"
#include "factDB/newftree/generator/iterator/Constructor.hpp"
#include "factDB/newftree/generator/iterator/Dereference.hpp"
#include "factDB/newftree/generator/iterator/DereferenceAllAccessible.hpp"
#include "factDB/newftree/generator/iterator/DereferenceAllAccessibleTableScan.hpp"
#include "factDB/newftree/generator/iterator/End.hpp"
#include "factDB/newftree/generator/iterator/Equality.hpp"
#include "factDB/newftree/generator/iterator/FromHandleStorage.hpp"
#include "factDB/newftree/generator/iterator/FromReferenceIter.hpp"
#include "factDB/newftree/generator/iterator/GetIteratorVariable.hpp"
#include "factDB/newftree/generator/iterator/Increment.hpp"
#include "factDB/newftree/generator/iterator/InsertAsChildToOther.hpp"
#include "factDB/newftree/generator/iterator/IsLast.hpp"
#include "factDB/newftree/generator/iterator/IsOtherRoot.hpp"
#include "factDB/newftree/generator/iterator/IsSubtreeEmpty.hpp"
#include "factDB/newftree/generator/iterator/IterableSize.hpp"
#include "factDB/newftree/generator/iterator/IteratedTupleSize.hpp"
#include "factDB/newftree/generator/iterator/IteratorGraphviz.hpp"
#include "factDB/newftree/generator/iterator/IteratorVars.hpp"
#include "factDB/newftree/generator/util/FGenUtil.hpp"
#include "factDB/newftree/generator/view/AdditionalIterators.hpp"
#include "factDB/newftree/generator/view/Constructor.hpp"
#include "factDB/newftree/generator/view/DereferenceReferences.hpp"
#include "factDB/newftree/generator/view/DereferenceView.hpp"
#include "factDB/newftree/generator/view/GetIteratorRef.hpp"
#include "factDB/newftree/generator/view/Increment.hpp"
#include "factDB/newftree/generator/view/IsLast.hpp"
#include "factDB/newftree/generator/view/IteratedTupleSize.hpp"
#include "factDB/newftree/generator/view/ListReferences.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
std::vector<FWContainer> Generator::getRequiredHeaders() {
   std::vector<FWContainer> vec;
   vec.emplace_back("factDB/infra/list/ListBase.hpp");
   vec.emplace_back("factDB/infra/Types.hpp");
   return vec;
}
// ---------------------------------------------------------------------------------------------------
void Generator::genStruct(const FTree& tree, fw::FileWriter& out, std::unordered_set<const FTree*>& generatedTrees) {
   if (generatedTrees.contains(&tree))
      return;
   FTreeTranslator trans(tree);

   if (tree.wasMerged()) {
      genStruct(*tree.getTopTree(), out, generatedTrees);
      genStruct(*tree.getLowerTree(), out, generatedTrees);
   }
   generatedTrees.insert(&tree);
   auto& rootNode = tree.getRootNode();
   if (rootNode.getChildren().empty()) {
      auto tmp = SettingBase::getSetting<bool>("codegen.factorized.sizePerList")->setTemporary(false);
      assert(tree.getTableScan() != nullptr);
      auto ius = tree.getTableScan()->collectIUs();
      std::unordered_map<const IU*, size_t, IUPointerHash, IUPointerEqual> iu2tblIdx;
      for (size_t idx = 0; idx != ius.size(); ++idx)
         iu2tblIdx[ius[idx]] = idx;

      auto typeIter = fw::iter(ius | std::views::transform([](const IU* iu) { return iu->type; }));
      out << fw::fmt("struct {} : public SingleNodeTree<{}> {{", trans.classname(), typeIter) << fw::endl()
          << fw::fmt("  using SNT = SingleNodeTree<{}>;", typeIter) << fw::endl()
          << fw::fmt("  using {} = SNT::ListType;", FTreeUtil(rootNode).listType()) << fw::endl()
          << fw::endl()
          << fw::fmt("  enum {} {{ {} }};", FTreePushBack::getPushbackEnum(rootNode), fw::iter(ius)) << fw::endl();

      for (auto& iter : tree.getIterators()) {
         auto tblIdxIter = fw::iter(iter.getIteratedIUs() | std::views::transform([&](const IU* iu) { return iu2tblIdx[iu]; }));
         out << fw::fmt("struct {} : public Iterator<{}", FTreeUtil::getIteratorName(iter), trans.classname()) << fw::condition(!iter.getIteratedIUs().empty(), ", ") << tblIdxIter << fw::fmt("> {{") << fw::endl()
             << fw::fmt("static const auto& fromTblIterator(const SNT::DefaultIterator& iter) {{") << fw::endl()
             << fw::fmt("  return reinterpret_cast<const {}&>(iter);", FTreeUtil::getIteratorName(iter)) << fw::endl()
             << fw::fmt("}}") << fw::endl()
             << fw::fmt("  const auto* get{}() const {{ return elements; }}", FTreeUtil(rootNode).iteratorVar()) << fw::endl()
             << fw::fmt("  const auto& get{}Ref() const {{ return *elements; }}", FTreeUtil(rootNode).iteratorVar()) << fw::endl()
             << fw::fmt("  template <typename T> static {} fromReferencedIterator(const T& r) {{ return {{*r.elements}}; }}", FTreeUtil::getIteratorName(iter)) << fw::endl()
             << FTreeGenerator::applyGenerator<DereferenceAllAccessibleTableScan>(iter) << fw::endl()
             << fw::fmt("}};") << fw::endl()
             << fw::fmt("{0} begin{1}() {{ return {0}(*this); }}", FTreeUtil::getIteratorName(iter), iter.getIteratorID()) << fw::endl()
             << fw::endl();
      }
      out << "};" << fw::endl();
   } else if (tree.isComposedTree()) {
      out << fw::fmt("struct {} {{", trans.classname()) << fw::endl()
          << "/*" << fw::verbosity(false) << fw::endl() << FTreeGenerator::applyGenerator<Graphviz>(rootNode) << "*/" << fw::verbosity(true) << fw::endl()
          << FTreeGenerator::applyGenerator<FTreePushBack>(rootNode, false)
          << FTreeGenerator::applyGenerator<ListReferences>(tree);
      for (auto& iter : tree.getIterators()) {
         genView(iter, out);
      }
      out << "};" << fw::endl()
          << fw::endl();
   } else {
      out << fw::fmt("struct {} {{", trans.classname()) << fw::endl()
          << "/*" << fw::verbosity(false) << fw::endl() << FTreeGenerator::applyGenerator<Graphviz>(rootNode) << "*/" << fw::verbosity(true) << fw::endl()
          << FTreeGenerator::applyGenerator<FNodeListTypes>(rootNode) << fw::endl()
          << fw::fmt("{} cl;", FTreeUtil(rootNode).listType()) << fw::endl()
          << fw::endl()
          << fw::fmt("// static constexpr std::string classname = \"{}\";", trans.classname()) << fw::endl()
          << FTreeGenerator::applyGenerator<FNodeSize>(rootNode) << fw::endl()
          << FTreeGenerator::applyGenerator<Printer>(rootNode) << fw::endl()
          << FTreeGenerator::applyGenerator<FNodePrinterGraphviz>(rootNode) << fw::endl()
          << FTreeGenerator::applyGenerator<FTreePushBack>(rootNode)
          << FTreeGenerator::applyGenerator<FNodeStatistics>(rootNode) << fw::endl();
      if (tree.wasMerged() && (!tree.getTopIterator()->getIteratedIUs().empty() && !tree.getLowerIterator()->getIteratedIUs().empty())) {
         // if the right tree is composed, the iterators are empty and appendEmptyTree would likely generate not compiling code (in topmost pipeline)
         out << FTreeGenerator::applyGenerator<HandleStorage>(tree) << fw::endl()
             << FTreeGenerator::applyGenerator<Reserve>(tree.getRootNode()) << fw::endl()
             << FTreeGenerator::applyGenerator<AppendEmptyTree>(tree, true) << fw::endl()
             << FTreeGenerator::applyGenerator<AppendEmptyTree>(tree, false) << fw::endl()
             << FTreeGenerator::applyGenerator<InsertHandleStorage>(tree) << fw::endl()
             << FTreeGenerator::applyGenerator<InsertAsChild>(tree, true) << fw::endl()
             << FTreeGenerator::applyGenerator<InsertAsChild>(tree, false) << fw::endl();
      }

      std::unordered_set<size_t> generatedIterators;
      for (auto& iter : tree.getIterators()) {
         genIterator(iter, out, generatedIterators);
         out << fw::endl();
         if (!iter.hasReferencedIterator())
            out << fw::fmt("{0} begin{1}() {{ return {0}::begin(*this); }}", FTreeUtil::getIteratorName(iter), iter.getIteratorID()) << fw::endl()
                << fw::fmt("{0} end{1}() {{ return {0}::end(*this); }}", FTreeUtil::getIteratorName(iter), iter.getIteratorID()) << fw::endl()
                << fw::endl();
      }
      out << "};" << fw::endl()
          << fw::endl();
   }
}
// ---------------------------------------------------------------------------------------------------
void Generator::genView(const factDB::FIterator& iter, fw::FileWriter& out) {
   out << fw::fmt("struct {} {{", FTreeUtil::getIteratorName(iter)) << fw::endl()
       << "/*" << fw::verbosity(false) << fw::endl()
       << FTreeGenerator::applyGenerator<IteratorGraphviz>(iter) << "*/" << fw::verbosity(true) << fw::endl()
       << fw::fmt("const  {}& upper;", FTreeUtil::getQualifiedIteratorName(*iter.getOwner().getTopIterator())) << fw::endl()
       << fw::fmt("const  {}& lower;", FTreeUtil::getQualifiedIteratorName(*iter.getOwner().getLowerIterator())) << fw::endl()
       << FTreeGenerator::applyGenerator<AdditionalIterators>(iter) << fw::endl_non_empty()
       << FTreeGenerator::applyGenerator<ConstructorView>(iter) << fw::endl_non_empty()
       << FTreeGenerator::applyGenerator<IncrementView>(iter)
       << FTreeGenerator::applyGenerator<IsLastView>(iter)
       << fw::endl()
       << FTreeGenerator::applyGenerator<GetIteratorRef>(iter)
       << fw::endl()
       << FTreeGenerator::applyGenerator<IteratedTupleSizeView>(iter)
       << FTreeGenerator::applyGenerator<DereferenceReferences>(iter) << fw::endl()
       << FTreeGenerator::applyGenerator<DereferenceView>(iter) << fw::endl()
       << fw::fmt("}};") << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void Generator::genIterator(const factDB::FIterator& iter, fw::FileWriter& out, std::unordered_set<size_t>& generatedIterators) {
   if (iter.hasReferencedIterator())
      genIterator(iter.getReferencedIterator(), out, generatedIterators);

   if (generatedIterators.contains(iter.getIteratorID()))
      return; // already generated early
   generatedIterators.insert(iter.getIteratorID());

   out << fw::fmt("struct {} {{", FTreeUtil::getIteratorName(iter)) << fw::endl();
   if (iter.hasReferencedIterator())
      out << fw::fmt("// referenced iterator: {}", FTreeUtil::getIteratorName(iter.getReferencedIterator())) << fw::endl();

   auto genTypes = [](const factDB::OrderedIUSet& ius) {
      return fw::iter(ius | std::views::transform([](const IU* iu) {
                         return fw::lc(iu->type.toString(), "&");
                      }));
   };
   auto classname = FTreeTranslator(iter.getOwner()).classname();

   out << "// iterated IUs: " << fw::pushSeparator(", ") << fw::iter(iter.getIteratedIUs()) << fw::popSeparator() << fw::endl()
       << "/*" << fw::verbosity(false) << fw::endl()
       << FTreeGenerator::applyGenerator<IteratorGraphviz>(iter) << "*/" << fw::verbosity(true) << fw::endl()
       << "public:" << fw::endl()
       << "  // using difference_type   = std::ptrdiff_t;" << fw::endl()
       << "  // using iterator_category = std::forward_iterator_tag;" << fw::endl()
       << "  // using pointer           = void;" << fw::endl()
       << fw::fmt("using reference = std::tuple<{}>;", genTypes(iter.getIteratedIUs())) << fw::endl()
       << fw::endl()
       << FTreeGenerator::applyGenerator<IteratorVars>(iter) << fw::endl()
       << fw::endl()
       << fw::fmt("{}() = default;", FTreeUtil::getIteratorName(iter)) << fw::endl()
       << FTreeGenerator::applyGenerator<Constructor>(iter, classname) << fw::endl()
       << FTreeGenerator::applyGenerator<FromReferencedIterator>(iter) << fw::endl()
       << FTreeGenerator::applyGenerator<FromHandleStorage>(iter) << fw::endl()
       << FTreeGenerator::applyGenerator<GetIteratorVariable>(iter) << fw::endl()
       << FTreeGenerator::applyGenerator<Begin>(iter, classname) << fw::endl()
       << FTreeGenerator::applyGenerator<BeginAt>(iter, classname) << fw::endl()
       << FTreeGenerator::applyGenerator<End>(iter, classname) << fw::endl()
       << FTreeGenerator::applyGenerator<IsLast>(iter) << fw::endl()
       << FTreeGenerator::applyGenerator<IsSubtreeEmpty>(iter) << fw::endl()
       << FTreeGenerator::applyGenerator<IsOtherRoot>(iter) << fw::endl()
       << FTreeGenerator::applyGenerator<IterableSize>(iter, classname) << fw::endl()
       << FTreeGenerator::applyGenerator<ClearCachedIteratedTupleSize>(iter, classname) << fw::endl()
       << FTreeGenerator::applyGenerator<IteratedTupleSize>(iter) << fw::endl()
       << FTreeGenerator::applyGenerator<Equality>(iter) << fw::endl()
       << FTreeGenerator::applyGenerator<Increment>(iter) << fw::endl()
       << FTreeGenerator::applyGenerator<DereferenceAllAccessible>(iter) << fw::endl()
       << FTreeGenerator::applyGenerator<Dereference>(iter) << fw::endl()
       << "};" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
