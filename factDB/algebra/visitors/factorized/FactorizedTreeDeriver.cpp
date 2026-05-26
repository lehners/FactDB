#include "factDB/algebra/visitors/factorized/FactorizedTreeDeriver.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/newftree/generator/GenerateFTree.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace factDB::algebra::visitors;
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<bool> factorizedPrint("codegen.factorized.print", false);
static factDB::Setting<bool> factorizedSize("codegen.factorized.size", false);
// ---------------------------------------------------------------------------------------------------
#ifdef NDEBUG
static factDB::Setting<bool> factorizedStatistics("codegen.factorized.statistics", false);
#else
static factDB::Setting<bool> factorizedStatistics("codegen.factorized.statistics", false);
#endif
// ---------------------------------------------------------------------------------------------------
void FactorizedTreeDeriver::visitPrepare(const Stage stage, const TableScan& tableScan) {
   if (stage == PrepareStage::BeforeCall) {
      assert(ftStorage.get(tableScan) == nullptr);

      FNodeOwning node;
      node.addIUs(tableScan.getRequiredIus());
      FTree treeRaw = FTree::fromNodes(std::move(node));
      auto& tree = ownedTrees.emplace_back(std::move(treeRaw));
      tree.createIterator(tableScan.getRequiredIusPipeline());
      tree.setTableScan(tableScan);

      ftStorage[tableScan] = &tree;
   } else {
      unreachable(); // no call in tablescan
   }
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTreeDeriver::visitPrepare(const AlgebraVisitor::Stage stage, const InnerJoin& join) {
   if (stage == PrepareStage::AfterCall) {
      // set the factorized tree from the child tree
      auto& leftInputFT = ftStorage[join.getLeftChild()];
      assert(leftInputFT != nullptr);
      assert(leftInputFT->getIterators().size() == 1);

      OrderedIUSet iusToInsert = join.getRightRequired().intersect(join.getParentRequired());

      auto& rightTree = *ftStorage[join.getRightChild()];
      auto& afterTree = ownedTrees.emplace_back();

      FIterator& curIter = leftInputFT->getIteratorsNC().front();
      const Operator& leftOperator = FindPipelineBreaker::getPipelineStarter(*join.getLeftChild());
      const InnerJoin* leftJoin = InnerJoin::dynCast(&leftOperator);
      const FIterator* insertIterator = nullptr;

      if (join.getJoinMode() == JoinMode::TopInsert) {
         // iterator layout:
         // 1: iterator for parent pipeline
         // 2: iterator for merging (if different from front, else only one iterator)
         // 3: potential iterator for printing

         if (leftJoin != nullptr && leftJoin->getJoinMode() == JoinMode::TopInsert) {
            auto& parentIter = leftInputFT->createIterator(leftJoin->getRightRequired().intersect(leftJoin->getParentRequiredPipeline()));
            curIter.setReferencedIterator(parentIter);
         }
         insertIterator = &curIter;
      } else {
         assert(join.getJoinMode() == JoinMode::BottomInsert);
         if (iusToInsert.empty())
            iusToInsert.merge(join.getRightRequiredPipeline());
         if (leftJoin != nullptr && leftJoin->getJoinMode() == JoinMode::TopInsert) {
            // join type switch, require additional iterator
            // this case is difficult because:
            // when first a TI is executed on the left side and then the next join is a bottom insert,
            // the tree is generated and then the handle storage of the upper part is used to iterate
            // directly through the tree. I.e., it should not iterate over the iterated nodes of the
            // newly generated upper tree. I.e., ideally the merged nodes of the upper tree are fixed
            // in the iterator.
            // But at this point we do not know which are the upper nodes, which are fixed. Hence, we
            // have to generate a reference iterator after the merge. First, therefore, the list of iterators
            // must be stable after the mere, since some references are set.
         } else {
            assert(leftInputFT->getIterators().size() == 1);
         }
         insertIterator = &curIter;
      }

      if (auto* rightChild = InnerJoin::dynCast(join.getRightChild().get()); rightChild != nullptr && rightChild->getJoinMode() == algebra::JoinMode::TopInsert) {
         rightTree.setComposedTree();
      }

      auto& rightIter = rightTree.getIterators().front();
      {
         FTree::mergeTrees(afterTree, *insertIterator, rightIter, join.getJoinMode());

         // add iterator, see above, only parentRequired intersect left Pipeline is required here.
         afterTree.createIterator(join.getParentRequiredPipeline());
         ftStorage[join] = &afterTree;
      }

      if (join.getJoinMode() == JoinMode::BottomInsert && (leftJoin != nullptr && leftJoin->getJoinMode() == JoinMode::TopInsert)) {
         // holds the iterator such that all merged nodes from the probe pipeline are fixed.
         // but for simplicity we can just fix the root node. This works, since only the root node is shared amongst different threads
         // all other nodes are not shared, hence, they stay the final nodes until the pipeline is iterated

         auto& parentIter = leftInputFT->createIterator(leftInputFT->getRootNode().getIUs());
         // now iterate through the necessary childs, this should be equivalent to iterating through all elements from the left subtree => but for convenience we use the iterator for all iterated ius
         curIter.setReferencedIterator(parentIter);
         //insertIterator = &leftInputFT->createSubiterator(parentIter, curIter.getIteratedIUs());
         assert(leftInputFT->getIterators().size() == 2);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTreeDeriver::visitPrepare(const AlgebraVisitor::Stage /*stage*/, const Count& count) {
   // ignored for now since we always generate counts
   topmost_operator = &count;
}
// ---------------------------------------------------------------------------------------------------
void FactorizedTreeDeriver::visitPrepare(const AlgebraVisitor::Stage stage, const Print& print) {
   if (stage != PrepareStage::AfterCall)
      return;
   topmost_operator = &print;
   const Operator& leftOperator = FindPipelineBreaker::getPipelineStarter(*print.getChild());
   const InnerJoin* childJoin = InnerJoin::dynCast(&leftOperator);

   if (childJoin != nullptr && childJoin->getJoinMode() == JoinMode::TopInsert) {
      auto& ftree = *ftStorage[childJoin->getLeftChild()];
      auto& curIter = ftree.getIterators().front();
      assert(!curIter.getIteratedIUs().empty());
      ftree.createSubiterator(curIter, childJoin->getLeftRequired()); // do not intesect with parentRequiredPipeline since this set is empty
   }
}
// ---------------------------------------------------------------------------------------------------
const factDB::FTree& FactorizedTreeDeriver::getFTree(const Operator& op) const {
   return *ftStorage.get(op);
}
// ---------------------------------------------------------------------------------------------------
struct GenerationHelper : public AlgebraVisitor {
   PipelineStore<factDB::FTree*>& ftStorage;
   factDB::FileWriter& out;
   std::unordered_set<const factDB::FTree*> generatedTrees;

   GenerationHelper(PipelineStore<factDB::FTree*>& ftStorage_, factDB::FileWriter& out_) : ftStorage(ftStorage_), out(out_) {}

   void visitProduce(const factDB::algebra::InnerJoin& join) override {
      const factDB::FTree* fTree = ftStorage[join];
      factDB::Generator::genStruct(*fTree, out, generatedTrees);
      join.getRightChild()->produce(*this);
   }
   void visitProduce(const factDB::algebra::CrossProduct&) override { __builtin_unreachable(); }
   void visitProduce(const factDB::algebra::Print& op) override { op.getChild()->produce(*this); }
   void visitProduce(const factDB::algebra::Selection& op) override { op.getChild()->produce(*this); }
   void visitProduce(const factDB::algebra::TableScan& op) override { factDB::Generator::genStruct(*ftStorage[op], out, generatedTrees); }
   void visitProduce(const factDB::algebra::Count& op) override { op.getChild()->produce(*this); }
};
// ---------------------------------------------------------------------------------------------------
void FactorizedTreeDeriver::genRootTrees(fw::FileWriter& out) {
   // we have to derive the rightmost tree since we may lose some trees there
   assert(topmost_operator != nullptr);

   GenerationHelper gh{ftStorage, out};
   topmost_operator->produce(gh);
}
// ---------------------------------------------------------------------------------------------------
