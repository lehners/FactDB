#include "factDB/algebra/visitors/CodegenFactorized.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/algebra/translator/ChainingHashtableTranslator.hpp"
#include "factDB/algebra/translator/TimingTranslator.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/list/ListTypeEnum.hpp"
#include "factDB/newftree/generator/GenerateFTree.hpp"
#include "factDB/newftree/generator/util/FGenUtil.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
#include <sstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
using HTT = factDB::algebra::HashTableTranslator;
// ---------------------------------------------------------------------------------------------------
static constexpr auto listTypeParser = settinghelper::makeEnumParser(
   std::tuple{infra::list::ListTypeEnum::ChunkedList, "ChunkedList", 'C'},
   std::tuple{infra::list::ListTypeEnum::LinkedList, "LinkedList", 'l'},
   std::tuple{infra::list::ListTypeEnum::TbbConcurrentVector, "TbbConcurrentVector", 'c'},
   std::tuple{infra::list::ListTypeEnum::InlineConcurrentVector, "InlineConcurrentVector", 'i'},
   std::tuple{infra::list::ListTypeEnum::TLSDequeList, "TLSDeque", 't'},
   std::tuple{infra::list::ListTypeEnum::TLFastDequeList, "TLSDeque", 'f'},
   std::tuple{infra::list::ListTypeEnum::TbbTLFastDequeList, "TLSDeque", 'F'},
   std::tuple{infra::list::ListTypeEnum::STLVector, "StlVector", 'd'},
   std::tuple{infra::list::ListTypeEnum::Auto, "Auto", 'a'});
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<factDB::infra::list::ListTypeEnum> factorizedList("codegen.factorized.listType", infra::list::Auto, &listTypeParser);
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<bool> factorizedTLSList("codegen.factorized.tlsBase", true);
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<bool> factorizedIUsFirst("codegen.factorized.iusFirst", true);
static factDB::Setting<bool> factorizedSizePerList("codegen.factorized.sizePerList", true);
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<bool> tlsHashtable("codegen.hashtable.tls", true);
static factDB::Setting<bool> cacheCountStar("codegen.cacheCountStar", true);
static factDB::Setting<bool> naiveMerge("codegen.factorized.naiveMerge", false);
static factDB::Setting<bool> inlinedList("codegen.factorized.inline", true);
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
// utility function to get the hashtable type
// ---------------------------------------------------------------------------------------------------
HashTableTranslator getHashTableTranslator(const InnerJoin& join, FactorizedTreeDeriver& ftStorage) {
   FWContainer htValueType;
   auto genTypeTuple = HTT::genTypeTuple(HTT::getLeftKeys(join));
   auto htName = fw::fmt("hashtable{}", join.getOperatorId());

   const FTree& ftn = ftStorage.getFTree(join);
   const FTree& leftFtn = ftStorage.getFTree(*join.getLeftChild());

   switch (join.getJoinMode()) {
      case JoinMode::TopInsert: {
         // store new tree in HT
         auto& iterToUse = *leftFtn.getMergingIterator();
         return {std::move(htName), std::move(genTypeTuple),
                 fw::fmt("{}::{}", FTreeTranslator(leftFtn).classname(), FTreeUtil::getIteratorName(iterToUse))};
      }
      case JoinMode::BottomInsert:
         // store insert handle in HT.
         // todo distinguish which tree to use
         return {std::move(htName), std::move(genTypeTuple), FTreeTranslator(ftn).getInsertHandleType()};
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
enum FTreeStage { Before,
                  After };
// ---------------------------------------------------------------------------------------------------
// utility function to get the correct hahtable type (and corresponding translator)
// ---------------------------------------------------------------------------------------------------
FTreeTranslator getFTreeTranslator(const InnerJoin& join, FactorizedTreeDeriver& ftStorage, FTreeStage stage) {
   if (stage == Before) {
      auto& leftFTN = ftStorage.getFTree(*join.getLeftChild());
      return FTreeTranslator{leftFTN, fw::lc("fti", join.getOperatorId(), "before")};
   } else {
      auto& ftn = ftStorage.getFTree(join);
      return FTreeTranslator(ftn, fw::lc("fti", join.getOperatorId(), "after"));
   }
}
// ---------------------------------------------------------------------------------------------------
// utility function to generate parallel for
// ---------------------------------------------------------------------------------------------------
void genFor(FileWriter& out, FWContainer size, FWContainer increments, auto consume, auto body) {
   auto& parallelFor = *SettingBase::getSetting<bool>("codegen.parallelFor");
   bool usePool = factDB::SettingBase::getSetting<bool>("codegen.threadPool")->get();
   size_t chunkSize = SettingBase::getSetting<size_t>("codegen.chunkSize")->get();
   FWContainer incrementsList = increments.isNOP() ? fw::nop() : fw::lc(", ", increments);

   if (parallelFor.get()) {
      if (usePool) {
         out << fw::fmt("pool.parallelFor(factDB::BlockedRange(0, {}, {}), [&](factDB::BlockedRange range) {{", size, chunkSize) << fw::endl();
      } else {
         out << fw::fmt("tbb::parallel_for(tbb::blocked_range<size_t>(0, {}, {}), [&](tbb::blocked_range<size_t> range) {{", size, chunkSize) << fw::endl();
      }
      out << "[[maybe_unused]] size_t range_begin = range.begin();" << fw::endl();
      consume();
      out << fw::fmt("  for (auto idx = range.begin(); idx < range.end(); idx++{}) {{", incrementsList) << fw::endl();
   } else {
      out << "range_begin = 0;" << fw::endl();
      consume();
      out << fw::fmt("for (size_t idx = 0; idx != {}; ++idx{}) {{", size, incrementsList) << fw::endl();
   }
   body();

   out << "  }" << fw::endl();
   if (parallelFor.get())
      out << "});" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
} // anonymous namespace
// ---------------------------------------------------------------------------------------------------
// prepare phase: generate the required f-trees with the tree-deriver.
// ---------------------------------------------------------------------------------------------------
void CodegenFactorized::visitPrepare(const Stage s, const Count& op) {
   tree_deriver_.visitPrepare(s, op);
}
// ---------------------------------------------------------------------------------------------------
void CodegenFactorized::visitPrepare(const Stage s, const Print& op) {
   tree_deriver_.visitPrepare(s, op);
}
// ---------------------------------------------------------------------------------------------------
void CodegenFactorized::visitPrepare(const Stage s, const InnerJoin& op) {
   tree_deriver_.visitPrepare(s, op);
}
// ---------------------------------------------------------------------------------------------------
void CodegenFactorized::visitPrepare(const Stage s, const TableScan& op) {
   CodegenExpanded::visitPrepare(s, op);
   tree_deriver_.visitPrepare(s, op);
}
// ---------------------------------------------------------------------------------------------------
void CodegenFactorized::addHeaders() {
   for (const auto& header : Generator::getRequiredHeaders())
      out.addInclude(header);
   out.addInclude("iostream"); // required for printing statistics
   CodegenExpanded::addHeaders();
}
// ---------------------------------------------------------------------------------------------------
std::string CodegenFactorized::execMode() {
   bool naive = factDB::SettingBase::getSetting<bool>("codegen.factorized.naiveMerge")->get();
   return naive ? "CodegenFactorizedNaive" : "CodegenFactorized";
}
// ---------------------------------------------------------------------------------------------------
void CodegenFactorized::open() {
   CodegenExpanded::open();
   tree_deriver_.genRootTrees(out);
}
// ---------------------------------------------------------------------------------------------------
// Table scan: iterate and reinterpret the flat tuples from the base tables
// ---------------------------------------------------------------------------------------------------
void CodegenFactorized::visitProduce(const TableScan& tableScan) {
   auto& ftree = tree_deriver_.getFTree(tableScan);
   auto* iteratorType = ftree.getMergingIterator();
   if (iteratorType == nullptr)
      iteratorType = &ftree.getIterators().front();

   auto& timePipeline = *SettingBase::getSetting<bool>("time.pipeline");

   auto tableRef = fw::lc("table_", tableScan.getAlias());
   TimingTranslator timer(fw::lc("table_", tableScan.getAlias()));
   out << fw::fmt("auto& {} = static_cast<const tables::{}&>(database.getTable({})).data;", tableRef, database.getSchema(tableScan.getTable()).name, tableScan.getTable()) << fw::endl()
       << fw::fmt("[[maybe_unused]] auto elements{} = {}.size();", tableScan.getOperatorId(), tableRef) << fw::endl()
       << timer.start() << fw::endl_non_empty()
       << fw::condition(timePipeline.get(), fw::fmt("auto start_execution_{} = std::chrono::steady_clock::now();", tableRef)) << fw::endl_non_empty();

   out << fw::context_switch() << fw::endl();
   tableScan.consumerConsume(ConsumeStage::Prepare, *this);
   out << fw::context_switch() << fw::endl()
       << "recorder.time();" << fw::endl();

   genFor(
      out, fw::lc(tableRef, ".size()"), fw::nop(), [&]() { tableScan.consumerConsume(ConsumeStage::TLSPrepare, *this); }, [&]() {
      FIteratorTranslator iteratorTranslator(*iteratorType);
      out << fw::fmt("  [[maybe_unused]] auto {} = {}::fromTblIterator({}[idx].toFIterator());", iteratorTranslator.name(), iteratorTranslator.getQualifiedIteratorName(), tableRef) << fw::endl()
          << fw::fmt("auto& [{}] = {}[idx].tuple;", fw::iter(tableScan.collectIUs()), tableRef) << fw::endl()
          << fw::endl();

      out << fw::context_switch() << fw::endl();
      tableScan.consumerConsume(ConsumeStage::BeforeLoop, *this);
      out << fw::context_switch() << fw::endl();
      tableScan.consumerConsume(ConsumeStage::InLoop, *this);
      out << fw::context_switch() << fw::endl();
      tableScan.consumerConsume(ConsumeStage::AfterLoop, *this);
      out << fw::context_switch() << fw::endl(); });
   out << timer.end() << fw::endl_non_empty()
       << timer.print(fw::lc("std::cout"));
}
// ---------------------------------------------------------------------------------------------------
// generate infra structure for the joins and produce input pipelines.
// ---------------------------------------------------------------------------------------------------
void CodegenFactorized::visitProduce(const InnerJoin& join) {
   FTreeTranslator ftiBefore = getFTreeTranslator(join, tree_deriver_, Before);
   FTreeTranslator ftiAfter = getFTreeTranslator(join, tree_deriver_, After);
   HashTableTranslator hashtable = getHashTableTranslator(join, tree_deriver_);

   assert(!handleStorage.containsPipeline(join));
   handleStorage[join] = FHandle("handle", join.getOperatorId());

   out << "// gen new join :begin:" << fw::endl();

   if (join.getJoinMode() == JoinMode::BottomInsert) {
      out << ftiAfter.instance() << fw::sendl()
          << "[[maybe_unused]]" << ftiBefore.instance() << fw::sendl();
   } else {
      out << "[[maybe_unused]]" << ftiBefore.instance() << fw::sendl();
   }

   out << hashtable.instance() << fw::endl()
       << fw::context_switch() << fw::endl();

   join.getLeftChild()->produce(*this);

   out << fw::context_switch() << fw::endl()
       << "recorder.time();" << fw::endl()
       << hashtable.finalize() << fw::endl()
       << fw::context_switch() << fw::endl();

   join.getRightChild()->produce(*this);

   if (join.getJoinMode() == JoinMode::BottomInsert) {
      auto iterator = ftiAfter.getFrontIteratorTranslator();

      out << fw::fmt("auto [elements{0}, bounds{0}] = {1};", join.getOperatorId(), iterator.iterableSize()) << fw::endl()
          << fw::context_switch() << fw::endl();
      join.consumerConsume(ConsumeStage::Prepare, *this);
      out << fw::context_switch() << fw::endl()
          << "recorder.time();" << fw::endl();
      auto body = [&]() {
         out << fw::popSeparator() << fw::context_switch() << fw::endl();
         join.consumerConsume(ConsumeStage::BeforeLoop, *this);
         out << fw::context_switch() << fw::endl();
         join.consumerConsume(ConsumeStage::InLoop, *this);
         out << fw::context_switch() << fw::endl();
         join.consumerConsume(ConsumeStage::AfterLoop, *this);
         out << fw::context_switch() << fw::endl();
      };

      out << fw::pushSeparator(",");
      if (!iterator.hasIteratedIUs()) {
         out << "{" << fw::endl()
             << fw::fmt("auto {0} = {1};", iterator.name(), iterator.begin()) << fw::endl();
         join.consumerConsume(ConsumeStage::TLSPrepare, *this);
         body();
         out << "}" << fw::endl();
      } else {
         genFor(
            out, fw::lc("elements", join.getOperatorId()), fw::fmt("++{}", iterator.name()), [&]() {
            join.consumerConsume(ConsumeStage::TLSPrepare, *this);
            out << fw::fmt("auto {} = {};", iterator.name(), iterator.beginAt("range_begin", fw::fmt("bounds{}", join.getOperatorId()))) << fw::endl(); }, [&]() {
            out << fw::fmt("while ({0}.isSubtreeEmpty()) ++{0};", iterator.name()) << fw::endl()
                             << iterator.dereferenceIUs() << fw::endl();
            body(); });
      }
   }

   if (factDB::SettingBase::getSetting<bool>("codegen.validateResultSize")->get()) {
      if (join.getJoinMode() == JoinMode::BottomInsert) {
         std::stringstream ss;
         FileWriter(ss) << fw::fmt("out_stream << \"{} (oID: {}):\t\" << {} << std::endl;", ftiAfter.name(), ftiAfter.classname(), ftiAfter.size());
         mismatchDebugOutput.emplace_back(ss.str());
      }
      std::stringstream ss;
      FileWriter(ss) << fw::fmt("out_stream << \"{} (oID: {}):\t\" << {} << std::endl;", ftiBefore.name(), ftiBefore.classname(), ftiBefore.size());
      mismatchDebugOutput.emplace_back(ss.str());
   }

   out << "// gen new join :end:" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
// consume the input pipeline in a bottom-insert
// ---------------------------------------------------------------------------------------------------
void CodegenFactorized::visitConsumeBottomInsert(const ConsumeStage stage, const InnerJoin& join, const Operator* caller) {
   auto ftiBefore = getFTreeTranslator(join, tree_deriver_, Before);
   auto ftiAfter = getFTreeTranslator(join, tree_deriver_, After);

   auto childIterator = ftiBefore.getMergingIteratorTranslator();
   auto hash = fw::fmt("hash{}", join.getOperatorId());

   // get initial operator of pipeline
   const Operator& leftPipelineBreakerOperator = FindPipelineBreaker::getPipelineStarter(*join.getLeftChild());
   const InnerJoin* leftJoin = InnerJoin::dynCast(&leftPipelineBreakerOperator);

   auto tlsSettingSet = SettingBase::getSetting<bool>("codegen.factorized.tlsBase")->get();
   auto tlsLocalBefore = tlsSettingSet ? fw::fmt("{}local", ftiBefore.name()) : fw::nop();
   auto tlsLocalAfter = tlsSettingSet ? fw::fmt("{}local", ftiAfter.name()) : fw::nop();

   auto hashtable = getHashTableTranslator(join, tree_deriver_);
   if (caller == join.getLeftChild().get()) { // build phase, build the HT
      auto tlsLocalHT = tlsHashtable.get() ? fw::fmt("localHT{}", join.getOperatorId()) : fw::nop();
      // here we have to materialize the tree
      // i.e. we are in the current probe phase of the previous join and have to materialize the tuples correctly
      // i.e. we either have to create the new tree and push all children, or we have to create the new tree list and get all handles.
      if (leftJoin != nullptr && leftJoin->getJoinMode() == JoinMode::TopInsert) {
         assert(childIterator.hasReferencedIterator());
         auto parentIterator = childIterator.getReferencedIteratorTranslator();

         assert(ftiBefore.getIterators().size() == 2);
         switch (stage) {
            case ConsumeStage::Prepare:
               break;
            case ConsumeStage::TLSPrepare:
               if (tlsSettingSet) {
                  out << fw::fmt("auto& {} = {}.local();", tlsLocalAfter, ftiAfter.name()) << fw::endl()
                      << fw::fmt("auto& {} = {}.local();", tlsLocalBefore, ftiBefore.name()) << fw::endl();
               }
               if (tlsHashtable.get())
                  out << hashtable.localEntries(tlsLocalHT) << fw::endl();
               break;
            case ConsumeStage::BeforeLoop:
               out << ftiBefore.appendEmptyTree(handleStorage[join], tlsLocalBefore, false) << fw::endl()
                   << "bool inserted_something = false;" << fw::endl()
                   << ftiBefore.genInsertHandleStorage() << fw::endl();
               break;
            case ConsumeStage::InLoop:
               out << ftiBefore.insertAsChild(handleStorage[join]) << fw::endl()
                   << "inserted_something = true;" << fw::endl();
               break;
            case ConsumeStage::AfterLoop:
               out << "if (inserted_something) {" << fw::endl()
                   << fw::fmt("auto {} = {}::fromHandleStorage({});", parentIterator.name(), parentIterator.getClassname(), ftiBefore.handleStorage()) << fw::endl()
                   << ftiAfter.genHandleStorage() << fw::endl()
                   << fw::fmt("for (auto {0} = {1}; !{2}; ++{0}) {{", childIterator.name(), childIterator.fromReferencedIterator(parentIterator), childIterator.isLast()) << fw::endl()
                   << childIterator.dereferenceIUs() << fw::endl()
                   << ftiAfter.appendEmptyTree(handleStorage[join], tlsLocalAfter) << fw::endl()
                   << hashtable.hash(hash, HTT::getLeftKeys(join)) << fw::endl()
                   << hashtable.insert(hash, HTT::genTuple(HTT::getLeftKeys(join)), fw::fmt("std::move({})", handleStorage[join].gen()), tlsLocalHT) << fw::endl()
                   << "  }" << fw::endl() // for loop
                   << "}" << fw::endl(); // if
               break;
         }
      } else { // BI or other sources
         assert(ftiBefore.getIterators().size() == 1);

         switch (stage) {
            case ConsumeStage::Prepare:
               break;
            case ConsumeStage::TLSPrepare:
               if (tlsSettingSet)
                  out << fw::fmt("auto& {} = {}.local();", tlsLocalAfter, ftiAfter.name()) << fw::endl();
               if (tlsHashtable.get())
                  out << hashtable.localEntries(tlsLocalHT) << fw::endl();
               out << ftiAfter.genHandleStorage() << fw::sendl();
               break;
            case ConsumeStage::BeforeLoop:
               out << "// nothing to do before loop" << fw::endl();
               break;
            case ConsumeStage::InLoop:
               out << ftiAfter.appendEmptyTree(handleStorage[join], tlsLocalAfter) << fw::endl()
                   << fw::fmt("auto tuple{} = {};", join.getOperatorId(), HTT::genTuple(HTT::getLeftKeys(join))) << fw::endl()
                   << hashtable.hash(hash, fw::lc("tuple", join.getOperatorId())) << fw::endl()
                   << hashtable.insert(hash, fw::lc("tuple", join.getOperatorId()), fw::fmt("std::move({})", handleStorage[join].gen()), tlsLocalHT) << fw::endl();
               break;
            case ConsumeStage::AfterLoop:
               out << "// nothing todo after loop" << fw::endl();
               break;
         }
      }
   } else { // probe phase, lookups in the HT.
      FWContainer treeIterator = fw::lc("treeIter", join.getOperatorId());
      FWContainer htIter = fw::lc("htIter", join.getOperatorId());
      switch (stage) {
         case ConsumeStage::Prepare:
            out << "// nothing todo in prepare phase: AlgebraVisitor::Prepare" << fw::endl();
            break;
         case ConsumeStage::TLSPrepare:
            out << "// nothing to TLS prepare" << fw::endl();
            break;
         case ConsumeStage::BeforeLoop:
            out << "// nothing todo in probe phase: AlgebraVisitor::BeforeLoop" << fw::endl();
            break;
         case ConsumeStage::InLoop: {
            out << hashtable.hash(hash, HTT::getRightKeys(join)) << fw::endl()
                << hashtable.lookup(hash, htIter) << fw::endl()
                << fw::fmt("while (!!{}) {{ // chain traversal start", htIter) << fw::endl()
                << fw::fmt("  {{") << fw::endl();
            if (!childIterator.hasIteratedIUs()) {
               auto hashtableIterator = childIterator.getReferencedIteratorTranslator();
               out << fw::fmt("auto {} = {};", treeIterator, childIterator.fromReferencedIterator(hashtableIterator)) << fw::endl();
            } else {
               out << fw::fmt("auto& {} = {}->val;", treeIterator, htIter) << fw::endl();
            }
            FHandle h("treeIter", join.getOperatorId());
            out << ftiAfter.insertAsChild(h, false) << fw::endl()
                << "  }" << fw::endl();
            switch (join.getJoinType()) {
               case JoinType::RightSemi:
                  out << "break;" << fw::endl();
                  break;
               case JoinType::Inner:
                  out << hashtable.lookupNext(htIter) << fw::sendl();
                  break;
            }

            out << "} // chain traversal stop" << fw::endl();
         } break;
         case ConsumeStage::AfterLoop:
            out << "// nothing todo in probe phase: AlgebraVisitor::AfterLoop" << fw::endl();
            break;
      }
   }
}
// ---------------------------------------------------------------------------------------------------
// ... and consume for top-inserts
// ---------------------------------------------------------------------------------------------------
void CodegenFactorized::visitConsumeTopInsert(const ConsumeStage stage, const InnerJoin& join, const Operator* caller) {
   // get initial operator of pipeline
   const Operator& leftPipelineBreakerOperator = FindPipelineBreaker::getPipelineStarter(*join.getLeftChild());
   auto ftiBefore = getFTreeTranslator(join, tree_deriver_, Before);
   auto hashtable = getHashTableTranslator(join, tree_deriver_);
   auto hash = fw::fmt("hash{}", join.getOperatorId());
   auto tlsSettingFactorizedSet = SettingBase::getSetting<bool>("codegen.factorized.tlsBase")->get();

   const InnerJoin* leftJoin = InnerJoin::dynCast(&leftPipelineBreakerOperator);
   if (caller == join.getLeftChild().get()) { // build phase, build the HT
      auto htIterator = ftiBefore.getFrontIteratorTranslator();
      auto tlsLocalHT = tlsHashtable.get() ? fw::fmt("localHT{}", join.getOperatorId()) : fw::nop();
      if (leftJoin != nullptr && leftJoin->getJoinMode() == JoinMode::BottomInsert) {
         // values are already inserted into tree & materialized
         switch (stage) {
            case ConsumeStage::Prepare:
               out << "// nothing to do prepare loop" << fw::endl();
               break;
            case ConsumeStage::TLSPrepare:
               if (tlsHashtable.get())
                  out << hashtable.localEntries(tlsLocalHT) << fw::endl();
               else
                  out << "// nothing to TLS prepare" << fw::endl();
               break;
            case ConsumeStage::BeforeLoop:
               out << "// nothing to do before loop" << fw::endl();
               break;
            case ConsumeStage::InLoop:
               out << "// nothing to do in loop" << fw::endl();
               break;
            case ConsumeStage::AfterLoop: {
               out << hashtable.hash(hash, HTT::getLeftKeys(join)) << fw::endl()
                   << fw::fmt("auto iterInst = {};", htIterator.name()) << fw::endl()
                   << hashtable.insert(hash, HTT::genTuple(HTT::getLeftKeys(join)), "std::move(iterInst)", tlsLocalHT) << fw::endl();
            } break;
         }
      } else if (leftPipelineBreakerOperator.getType() == Operator::TableScan) { // same as in case of normal join
         // table scan on the left side
         if (stage == ConsumeStage::TLSPrepare && tlsHashtable.get()) {
            out << hashtable.localEntries(tlsLocalHT) << fw::endl();
         } else if (stage == ConsumeStage::AfterLoop) {
            out << hashtable.hash(hash, HTT::getLeftKeys(join)) << fw::endl()
                << hashtable.insert(hash, HTT::genTuple(HTT::getLeftKeys(join)), fw::fmt("std::move({})", htIterator.name()), tlsLocalHT) << fw::endl();
         }
      } else {
         auto tlsLocal = tlsSettingFactorizedSet ? fw::fmt("localFTI{}before", join.getOperatorId()) : fw::nop();
         // we have to take care that the values from the left are correctly materialized since they are currently not materialized.
         switch (stage) {
            case ConsumeStage::Prepare: break;
            case ConsumeStage::TLSPrepare:
               if (tlsSettingFactorizedSet)
                  out << fw::fmt("auto& {} = {}.local();", tlsLocal, ftiBefore.name()) << fw::endl();
               if (tlsHashtable.get())
                  out << hashtable.localEntries(tlsLocalHT) << fw::endl();
               break;
            case ConsumeStage::BeforeLoop:
               out << ftiBefore.appendEmptyTree(handleStorage[join], tlsLocal, false) << fw::endl()
                   << "bool inserted_something = false;" << fw::endl()
                   << ftiBefore.genInsertHandleStorage() << fw::endl();
               break;
            case ConsumeStage::InLoop:
               out << ftiBefore.insertAsChild(handleStorage[join]) << fw::endl()
                   << "inserted_something = true;" << fw::endl();
               break;
            case ConsumeStage::AfterLoop: {
               auto childIterator = ftiBefore.getMergingIteratorTranslator();
               auto parentIterator = childIterator.getReferencedIteratorTranslator();

               if (!childIterator.hasIteratedIUs()) {
                  out << "if (inserted_something) {" << fw::endl()
                      << hashtable.hash(hash, HTT::getLeftKeys(join)) << fw::endl()
                      << fw::fmt("{} iterInst(std::move({}), {});", parentIterator.getQualifiedIteratorName(), handleStorage[join].gen(), ftiBefore.name()) << fw::endl()
                      << hashtable.insert(hash, HTT::genTuple(HTT::getLeftKeys(join)), "std::move(iterInst)", tlsLocalHT) << fw::endl()
                      << "}" << fw::endl();
               } else if (childIterator.getIteratedIUs() == parentIterator.getIteratedIUs()) {
                  out << "if (inserted_something) {" << fw::endl()
                      << hashtable.hash(hash, HTT::getLeftKeys(join)) << fw::endl()
                      << fw::fmt("auto {} = {}::fromHandleStorage({});", parentIterator.name(), parentIterator.getClassname(), ftiBefore.handleStorage()) << fw::endl()
                      << hashtable.insert(hash, HTT::genTuple(HTT::getLeftKeys(join)), childIterator.fromReferencedIterator(parentIterator), tlsLocalHT) << fw::endl()
                      << "}" << fw::endl();
               } else {
                  out << "if (inserted_something) {" << fw::endl()
                      << fw::fmt("auto {} = {}::fromHandleStorage({});", parentIterator.name(), parentIterator.getClassname(), ftiBefore.handleStorage()) << fw::endl()
                      << fw::fmt("for (auto childIterator = {}; !childIterator.isLast(); ++childIterator) {{", childIterator.fromReferencedIterator(parentIterator)) << fw::endl()
                      << fw::fmt("  auto [{}] = *childIterator;", fw::iter(childIterator.getIteratedIUs())) << fw::endl()
                      << hashtable.hash(hash, HTT::getLeftKeys(join)) << fw::endl()
                      << fw::fmt("  auto childIteratorCopy = childIterator;") << fw::endl()
                      << hashtable.insert(hash, HTT::genTuple(HTT::getLeftKeys(join)), "std::move(childIteratorCopy)", tlsLocalHT) << fw::endl()
                      << fw::fmt("  }}") << fw::endl() // for loop
                      << fw::fmt("}}") << fw::endl(); // if
               }
            } break;
         }
      }
   } else { // probe phase
      FWContainer htIter = fw::lc("htIter", join.getOperatorId());
      switch (stage) {
         case ConsumeStage::Prepare:
            out << "// forward prepare" << fw::endl();
            join.consumerConsume(ConsumeStage::Prepare, *this);
            break;
         case ConsumeStage::TLSPrepare:
            join.consumerConsume(ConsumeStage::TLSPrepare, *this);
            break;
         case ConsumeStage::BeforeLoop:
            out << "// nothing todo in probe phase: AlgebraVisitor::BeforeLoop" << fw::endl();
            break;
         case ConsumeStage::InLoop: {
            out << fw::context_switch() << fw::endl()
                << hashtable.hash(hash, HTT::getRightKeys(join)) << fw::endl()
                << hashtable.lookup(hash, htIter) << fw::endl()
                << fw::fmt("if (!{}) continue;", htIter) << fw::endl();
            join.consumerConsume(ConsumeStage::BeforeLoop, *this);
            out << fw::context_switch() << fw::endl();

            switch (join.getJoinType()) {
               case JoinType::RightSemi:
                  out << fw::fmt("for (;!!{0};{0}=nullptr) {{ // chain traversal start", htIter) << fw::endl();
                  break;
               case JoinType::Inner:
                  out << fw::fmt("for (;!!{};{}) {{ // chain traversal start", htIter, hashtable.lookupNext(htIter)) << fw::endl();
                  break;
            }

            // in the topmost pipe, an additional iterator is added for the iteration through all elemments.
            // therefore, we have to check here, if this iterator exists. This is always the last iterator and this must have a reference iterator (to the merging iterator).
            auto childIterator = ftiBefore.getIterators().back().hasReferencedIterator() ? ftiBefore.getIteratorTranslator(ftiBefore.getIterators().back()) : ftiBefore.getMergingIteratorTranslator();

            if (!childIterator.hasIteratedIUs()) {
               assert(childIterator.hasReferencedIterator());
               auto htIterator = childIterator.getReferencedIteratorTranslator(); // htIterator

               out << "{" << fw::endl()
                   << fw::fmt("[[maybe_unused]] auto& {} = {}->val;", childIterator.name(), htIter) << fw::endl()
                   << fw::fmt("// auto {} = {};", childIterator.name(), childIterator.fromReferencedIterator(htIterator)) << fw::endl();
            } else if (ftiBefore.tree_.getTableScan() != nullptr) {
               auto htIterator = ftiBefore.getMergingIteratorTranslator(); // htIterator
               out << "{" << fw::endl()
                   // << fw::fmt("auto& {} = {}->val;", htIterator.name(), htIter) << fw::endl()
                   << fw::fmt("[[maybe_unused]] auto& {} = {}->val;", htIterator.name(), htIter) << fw::endl();
               if (htIterator.iterator.getIteratorID() != childIterator.iterator.getIteratorID()) {
                  out << fw::fmt("[[maybe_unused]] auto {} = {};", childIterator.name(), childIterator.fromReferencedIterator(htIterator.name())) << fw::endl()
                      << childIterator.dereferenceIUs() << fw::endl();
               }
            } else if (&childIterator.iterator == ftiBefore.tree_.getMergingIterator()) {
               out << "{" << fw::endl()
                   << fw::fmt("[[maybe_unused]] auto {} = {}->val;", childIterator.name(), htIter) << fw::endl()
                   << childIterator.dereferenceIUs() << fw::endl();
            } else {
               assert(childIterator.hasReferencedIterator());
               auto htIterator = childIterator.getReferencedIteratorTranslator(); // htIterator

               out << fw::fmt("auto& {} = {}->val;", htIterator.name(), htIter) << fw::endl()
                   << fw::fmt("for (auto {0} = {1}; !{0}.isLast(); ++{0}) {{", childIterator.name(), childIterator.fromReferencedIterator(htIterator)) << fw::endl()
                   << fw::pushSeparator(", ")
                   << childIterator.dereferenceIUs() << fw::endl()
                   << fw::endl();
            }
            assert(ftiBefore.getFTree().getInsertedTree() != nullptr);
            auto afterTree = getFTreeTranslator(join, tree_deriver_, After);
            if (afterTree.isComposedTree()) {
               auto mergingIterator = afterTree.getMergingIteratorTranslator();
               if (afterTree.hasComposedSubiterator()) {
                  out << afterTree.constructMergingIterator() << fw::endl()
                      << fw::fmt("for (; !{}; ++{}) {{", mergingIterator.isLast(), mergingIterator.name()) << fw::endl()
                      << mergingIterator.dereferenceIUs() << fw::endl();
               } else {
                  out << "{" << fw::endl()
                      << afterTree.constructMergingIterator() << fw::endl()
                      << mergingIterator.dereferenceIUs() << fw::endl();
               }
            }

            out << fw::context_switch() << fw::endl();
            join.consumerConsume(ConsumeStage::InLoop, *this);
            if (afterTree.isComposedTree())
               out << "}" << fw::endl();
            out << fw::context_switch() << fw::endl()
                << "  }" << fw::endl()
                << "} // chain traversal stop" << fw::endl()
                << fw::context_switch() << fw::endl();
            join.consumerConsume(ConsumeStage::AfterLoop, *this);
            out << fw::context_switch() << fw::endl();
         } break;
         case ConsumeStage::AfterLoop:
            out << "// nothing todo in probe phase: AlgebraVisitor::AfterLoop" << fw::endl();
            break;
      }
   }
}
// ---------------------------------------------------------------------------------------------------
// dispatch consume in joins: call the respective function for top- or botom-inserts
// ---------------------------------------------------------------------------------------------------
void CodegenFactorized::visitConsume(const ConsumeStage stage, const InnerJoin& join, const Operator* caller) {
   switch (join.getJoinMode()) {
      case JoinMode::BottomInsert:
         return visitConsumeBottomInsert(stage, join, caller);
      case JoinMode::TopInsert:
         return visitConsumeTopInsert(stage, join, caller);
   }
}
// ---------------------------------------------------------------------------------------------------
// generate factorized count.
// ---------------------------------------------------------------------------------------------------
void CodegenFactorized::visitConsume(const ConsumeStage stage, const Count& count, const Operator* caller) {
   assert(caller == count.getChild().get());
   switch (stage) {
      case ConsumeStage::Prepare: {
         const auto& pipelineBreaker = FindPipelineBreaker::getPipelineStarter(*caller);
         if (pipelineBreaker.getType() == Operator::InnerJoin) {
            auto& childJoin = *InnerJoin::dynCast(&pipelineBreaker);
            const auto& leftInputOfJoin = FindPipelineBreaker::getPipelineStarter(*childJoin.getLeftChild());
            if (leftInputOfJoin.getType() == Operator::InnerJoin && InnerJoin::dynCast(&leftInputOfJoin)->getJoinMode() == JoinMode::BottomInsert) {
               auto leftIterator = getFTreeTranslator(childJoin, tree_deriver_, Before).getMergingIteratorTranslator();
               out << fw::fmt("{}::clearCachedIteratedTupleSize(fti{}after);", leftIterator.getQualifiedIteratorName(), leftInputOfJoin.getOperatorId()) << fw::endl();
            }
         } else {
            out << "// nothing to prepare in consume of count" << fw::endl();
         }
      } break;
      case ConsumeStage::TLSPrepare: {
         bool parallel = SettingBase::getSetting<bool>("codegen.parallelFor")->get();
         bool parallelForTLS = parallel && SettingBase::getSetting<bool>("codegen.parallelFor.tls")->get();

         if (parallelForTLS)
            out << fw::fmt("auto& counter{0}_ref = tls_counter{0}.local();", count.getOperatorId()) << fw::endl();
         else if (parallel)
            out << fw::fmt("std::atomic_ref counter{0}_ref(counter{0});", count.getOperatorId()) << fw::endl();
         else
            out << fw::fmt("auto& counter{0}_ref = counter{0};", count.getOperatorId()) << fw::endl();
      } break;
      case ConsumeStage::BeforeLoop:
         break;
      case ConsumeStage::InLoop: {
         auto counterVar = fw::fmt("counter{0}_ref", count.getOperatorId());
         const auto& pipelineBreaker = FindPipelineBreaker::getPipelineStarter(*caller);
         switch (pipelineBreaker.getType()) {
            case Operator::TableScan:
               out << counterVar << "++" << fw::sendl();
               break;
            case Operator::InnerJoin: {
               const InnerJoin& leftJoin = static_cast<const InnerJoin&>(pipelineBreaker);
               const auto& afterTree = getFTreeTranslator(leftJoin, tree_deriver_, After);
               if (leftJoin.getJoinMode() == algebra::JoinMode::TopInsert) {
                  FIteratorTranslator upperIterator(afterTree.getTopIteratorTranslator());
                  FIteratorTranslator lowerIterator(afterTree.getLowerIteratorTranslator());
                  out << fw::fmt("{} += {} * {};", counterVar, upperIterator.iteratedTupleSize(false), lowerIterator.iteratedTupleSize()) << fw::endl();
               } else {
                  FIteratorTranslator iterator(afterTree.getIterators().back());
                  out << fw::fmt("{} += {};", counterVar, iterator.iteratedTupleSize()) << fw::endl();
               }
               break;
            }
            case Operator::Count:
            case Operator::Print:
            case Operator::Selection:
            case Operator::Reference:
               unreachable();
            case Operator::CrossProduct:
               not_implemented();
         }
      } break;
      case ConsumeStage::AfterLoop:
         break;
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
