#include "factDB/algebra/visitors/CodegenExpanded.hpp"
#include "RelationLoader.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Operator.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/algebra/translator/ChainingHashtableTranslator.hpp"
#include "factDB/algebra/translator/TimingTranslator.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/queryc/ExecutionMode.hpp"
#include "factDB/queryc/queryDispatcher/runtime/KnownQueries.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
#include "fmt/format.h"
#include <factDB/infra/list/STLDeque.hpp>

// implementation of the codegen of the flat query engine.
// this follows the produce/consume pattern.
// To simplify implementation, we have added Stage and ConsumeStage, which allow for multiple calls.

namespace factDB::algebra::visitors {

static factDB::Setting<bool> parallelFor("codegen.parallelFor", true);
static factDB::Setting<bool> parallelForTLS("codegen.parallelFor.tls", true);

static factDB::Setting<size_t> chunkSize("codegen.chunkSize", 500);
static factDB::Setting<bool> hashtableStatistics("codegen.hashtable.statistics", false);

static factDB::Setting<bool> threadPool("codegen.threadPool", false);

static factDB::Setting<bool> timePipeline("time.pipeline", false);
static factDB::Setting<std::string> schemaKeySetting("schema.key", "unknown");

using HTT = HashTableTranslator;

void CodegenExpanded::addHeaders() {
   // required headers
   out.addInclude("factDB/Database.hpp");
   out.addInclude("factDB/infra/ChainingHashTable.hpp");
   out.addInclude("factDB/queryc/NewFileWriter.hpp");
   out.addInclude("factDB/infra/util/Hash.hpp");
   out.addInclude("factDB/queryc/PerformanceRecord.hpp");
   out.addInclude("factDB/infra/threading/LocalState.hpp");
   out.addInclude("factDB/queryc/ExecutionMode.hpp");
   out.addInclude("factDB/queryc/queryDispatcher/runtime/QueryDispatcher.hpp");
   out.addInclude("ostream");
   if (threadPool.get())
      out.addInclude("factDB/infra/threading/ThreadPool.hpp");
   if (parallelFor.get() || parallelForTLS.get())
      out.addInclude("tbb/tbb.h");
   if (timePipeline.get() || hashtableStatistics.get())
      out.addInclude("iostream");
   if (factDB::SettingBase::getSetting<bool>("codegen.validateResultSize")->get())
      out.addInclude("factDB/infra/RuntimeException.hpp");
   std::unordered_set<std::string> seenHeaders;
   for (auto table_id : requiredTableIds) {
      auto str = database.getHppPath(table_id);
      if (seenHeaders.contains(str)) continue;
      out.addInclude(str);
      seenHeaders.insert(str);
   }
}

CodegenExpanded::CodegenExpanded(const Database& db, FileWriter& writer)
   : AlgebraVisitorDB(db), out(writer) {
}

void CodegenExpanded::open() {
   /// write preamble
   addHeaders();
   out << fw::endl();
   if (SettingBase::getSetting<bool>("codegen.pregenerateQueries")->get()) {
      out << fw::fmt("namespace factDB::generated::{}{}::{} {{", execMode(), queryc::QueryProperties::fromCurrentSettings().genModeString(), SettingBase::getSetting<std::string>("queryname")->get()) << fw::endl();
   } else {
      out << "namespace factDB::generated {" << fw::endl();
   }
}

void CodegenExpanded::close() {
   // write end of file
   auto qp = queryc::QueryProperties::fromCurrentSettings();
   out << fw::endl()
       << fw::fmt("static bool bar_registered = []{{ ") << fw::endl()
       << fw::fmt("  factDB::queryc::QueryDispatcher::registerQuery(queryc::QueryProperties(queryc::ExecutionMode::{}, \"{}\", static_cast<queryc::QueryFlag>({})), &query, &getRequiredRelations, &getSchemaIdentifier);", queryc::executionModeParser.output(qp.mode), qp.queryname, qp.getFlagUInt()) << fw::endl()
       << fw::fmt("  return true;") << fw::endl()
       << fw::fmt("}}();") << fw::endl()
       << "} // namespace generated::factDB" << fw::endl();
}

namespace {

HashTableTranslator getHashTableTranslator(const InnerJoin& join) {
   return {fw::lc("hashtable_", join.getOperatorId()), HTT::genTypeTuple(HTT::getLeftKeys(join)), HTT::genTypeTuple(join.getLeftRequired())};
}

} // anonymous namespace

void CodegenExpanded::visitPrepare(const Stage s, const TableScan& op) {
   if (s == Stage::BeforeCall)
      requiredTableIds.emplace_back(op.getTable());
}

void CodegenExpanded::visitProduce(const CrossProduct&) {
   not_implemented();
}

void CodegenExpanded::visitConsume(const AlgebraVisitor::ConsumeStage, const CrossProduct&, const Operator*) {
   not_implemented();
}

// the produce function for the join: this generates the basic structures and makes sure, that first the left (build) and then the right (probe) child is produced.
void CodegenExpanded::visitProduce(const InnerJoin& join) {
   auto stats = fw::lc("htStats", join.getOperatorId());
   HashTableTranslator hashtable = getHashTableTranslator(join);
   out << hashtable.instance() << fw::endl();

   join.getLeftChild()->produce(*this);

   out << fw::endl()
       << "recorder.time();" << fw::endl()
       << hashtable.finalize() << fw::endl()
       << fw::endl();

   join.getRightChild()->produce(*this);
   if (hashtableStatistics.get())
      out << hashtable.printStatsBuild("std::cout") << fw::endl();
}

// consume function of the join: this is called by the children. It inserts tuples from the left side into the hashtable and probes right side tuples.
void CodegenExpanded::visitConsume(const ConsumeStage stage, const InnerJoin& join, const Operator* caller) {
   assert(join.getJoinMode() == JoinMode::TopInsert && "Only Top Insert Join is supported.");
   HashTableTranslator hashtable = getHashTableTranslator(join);
   auto useTLS = SettingBase::getSetting<bool>("codegen.hashtable.tls")->get();
   auto localEntries = useTLS ? fw::fmt("localsHT{}", join.getOperatorId()) : fw::nop();
   if (stage == ConsumeStage::TLSPrepare && caller == join.getLeftChild().get()) {
      out << hashtable.localEntries(localEntries) << fw::endl();
   } else if (stage == ConsumeStage::InLoop) {
      FWContainer hashVar = fw::lc("buildHash", join.getOperatorId());
      FWContainer stats = fw::lc("htStats", join.getOperatorId());
      FWContainer iter = fw::lc("htIter", join.getOperatorId());

      if (caller == join.getLeftChild().get()) {
         // build side
         out << hashtable.hash(hashVar, HTT::getLeftKeys(join)) << fw::endl()
             << hashtable.insert(hashVar, HTT::getLeftKeys(join), join.getLeftRequired(), localEntries) << fw::endl();
      } else {
         // probe side
         out << hashtable.hash(hashVar, HTT::getRightKeys(join)) << fw::endl()
             << hashtable.lookup(hashVar, iter) << fw::endl()
             << fw::fmt("while (!!{}) {{", iter) << fw::endl()
             << fw::fmt("auto& [{}] = {}->val;", fw::iter(OrderedIUSet(join.getLeftRequired())), iter) << fw::endl()
             << fw::endl();
         join.consumerConsume(ConsumeStage::InLoop, *this);
         switch (join.getJoinType()) {
            case JoinType::RightSemi:
               out << "break;" << fw::endl();
               break;
            case JoinType::Inner:
               out << hashtable.lookupNext(iter) << fw::sendl();
               break;
         }
         out << "}" << fw::endl();
      }
   } else {
      join.consumerConsume(stage, *this);
   }
}

void genFunctionHeader(FileWriter& out) {
   out.addFunctionHeader("void query(factDB::Database &database, std::ostream& out_stream, queryc::PerformanceRecorderExecution recorder)", !SettingBase::getSetting<bool>("codegen.pregenerateQueries")->get())
      //<< fw::condition(parallelFor.get() && threadPool.get(), "[[maybe_unused]] LocalState localState;") << fw::endl()
      << "[[maybe_unused]] size_t range_begin;" << fw::endl()
      << fw::condition(parallelFor.get() && threadPool.get(), "[[maybe_unused]] auto& pool = factDB::ThreadPool::getPool();") << fw::endl()
      << "recorder.start();" << fw::endl()
      << "FileWriter outWriter(std::cout);" << fw::endl();
   if constexpr (infra::list::doCounts)
      out << "{" << fw::endl();
#if 0
   if (!threadPool.get() && (parallelFor.get() || parallelForTLS.get())) { // required to fix segfault caused by tbb
      out << "oneapi::tbb::task_scheduler_handle handle;" << fw::endl()
          << "handle = oneapi::tbb::task_scheduler_handle{oneapi::tbb::attach{}};" << fw::endl();
   }
#endif
}

void finishFunction(FileWriter& out) {
#if 0
   if (!threadPool.get() && (parallelFor.get() || parallelForTLS.get())) { // required to fix segfault caused by tbb
      out << fw::endl()
          << "try {" << fw::endl()
          << "   oneapi::tbb::finalize(handle);" << fw::endl()
          << "   // oneTBB worker threads are terminated at this point." << fw::endl()
          << "} catch (const oneapi::tbb::unsafe_wait&) {" << fw::endl()
          << "   out_stream << \"Failed to terminate the worker threads.\" << std::endl;" << fw::endl()
          << "}" << fw::endl();
   }
#endif
   out << "  recorder.stop();" << fw::endl()
       << "}" << fw::endl();
}

// query specific information
void CodegenExpanded::produceGeneratorInfo(const Operator& op) {
   out.addFunctionHeader("std::string getSchemaIdentifier()", false)
      << fw::fmt("  return \"{}\";", schemaKeySetting.get()) << fw::endl()
      << "}" << fw::endl();
   out.addFunctionHeader("std::vector<std::string> getRequiredRelations()", false)
      << "  return std::vector<std::string>({" << fw::endl();
   algebra::visitors::RelationLoaderConst loader(database);
   op.produce(loader);
   for (uint32_t rel : loader.relations)
      out << fw::fmt("\"{}\",", database.getSchema(rel).name) << fw::endl();

   out << "  });" << fw::endl()
       << "}" << fw::endl();
}

// generates infrastructure for counting
void CodegenExpanded::visitProduce(const Count& count) {
   produceGeneratorInfo(count);
   genFunctionHeader(out);
   auto counterVar = fw::lc("counter", count.getOperatorId());

   if (parallelFor.get() && parallelForTLS.get() && threadPool.get())
      out << fw::fmt("OwningThreadLocal<size_t> tls_counter{};", count.getOperatorId()) << fw::endl();
   else if (parallelFor.get() && parallelForTLS.get())
      out << fw::fmt("tbb::enumerable_thread_specific<size_t> tls_counter{};", count.getOperatorId()) << fw::endl();
   out << fw::fmt("size_t counter{} = 0;", count.getOperatorId()) << fw::endl();

   count.getChild()->produce(*this);

   if (parallelFor.get() && parallelForTLS.get())
      out << "recorder.time();" << fw::endl()
          << fw::fmt("assert(counter{} == 0);", count.getOperatorId()) << fw::endl()
          << fw::fmt("for (auto a : tls_counter{})", count.getOperatorId()) << fw::endl()
          << fw::fmt("  counter{} += a;", count.getOperatorId()) << fw::endl();

   for (auto& s : mismatchDebugOutput)
      std::erase(s, '\n');

   out << fw::fmt("out_stream << counter{} << std::endl;", count.getOperatorId()) << fw::endl();
   // validate result
   if (factDB::SettingBase::getSetting<bool>("codegen.validateResultSize")->get()) {
      size_t expectedResultSize = SettingBase::getSetting<size_t>("resultsize")->get();
      out << fw::fmt("if (counter{} != {}) {{", count.getOperatorId(), expectedResultSize) << fw::endl()
          << fw::iter(mismatchDebugOutput, " ", true) << fw::endl()
          << fw::fmt("out_stream << counter{} << \" vs. \" << {} <<  std::endl;", count.getOperatorId(), expectedResultSize) << fw::endl()
          << "  recorder.inv();" << fw::endl()
          << "  return;" << fw::endl()
          // << "throw RuntimeException(ErrorCode::RuntimeError, \"Result size mismatches expected result size.\");" << fw::endl()
          << "}" << fw::endl();
   }

   if constexpr (infra::list::doCounts)
      out << "}" << fw::endl()
          << "infra::list::STLDequeStats::printStats();" << fw::endl();

   finishFunction(out);
}

// counts the incoming elements
void CodegenExpanded::visitConsume(const ConsumeStage stage, const Count& count, [[maybe_unused]] const Operator* caller) {
   if (stage == ConsumeStage::TLSPrepare) {
      if (parallelFor.get() && parallelForTLS.get()) {
         out << fw::fmt("auto& counter{0}_ref = tls_counter{0}.local();", count.getOperatorId()) << fw::endl();
      } else if (parallelFor.get()) {
         out << fw::fmt("std::atomic_ref counter{0}_ref(counter{0});", count.getOperatorId()) << fw::endl();
      } else {
         out << fw::fmt("auto& counter{0}_ref = counter{0};", count.getOperatorId()) << fw::endl();
      }
   } else if (stage == ConsumeStage::InLoop) {
      assert(caller == count.getChild().get());
      out << fw::fmt("counter{}_ref++;", count.getOperatorId()) << fw::endl();
   }
}

void CodegenExpanded::visitProduce(const Print& print) {
   produceGeneratorInfo(print);
   genFunctionHeader(out);

   if (parallelFor.get())
      out << "std::mutex output_mutex;" << fw::endl();
   print.getChild()->produce(*this);

   finishFunction(out);
}

// print all tuples to output (use lock for synchronization)
void CodegenExpanded::visitConsume(const ConsumeStage stage, const Print& print, [[maybe_unused]] const Operator* caller) {
   if (stage == ConsumeStage::InLoop) {
      assert(caller == print.getChild().get());

      out << "{" << fw::endl() << fw::pushSeparator(fmt::format(" << \"{}\" << ", print.getSeperator()))
          << fw::condition(parallelFor.get(), "std::lock_guard lock(output_mutex);") << fw::endl_non_empty()
          << fw::fmt("out_stream << {} << std::endl;", fw::iter(print.getRequiredIus())) << fw::endl()
          << "}" << fw::popSeparator() << fw::endl();
   }
}

void CodegenExpanded::visitProduce(const Selection& selection) {
   selection.getChild()->produce(*this);
}

// filter tuples and forward them to next operator
void CodegenExpanded::visitConsume(const ConsumeStage stage, const Selection& selection, [[maybe_unused]] const Operator* caller) {
   if (stage == ConsumeStage::InLoop) {
      out << "if (!(" << selection.getPredicate() << ")) {" << fw::endl()
          << "continue;" << fw::endl()
          << "}" << fw::endl();
   }
   selection.consumerConsume(stage, *this);
}

// implementation of table scans: start new pipeline to itearte over the tuples of each thread
void CodegenExpanded::visitProduce(const TableScan& tableScan) {
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
   if (parallelFor.get()) {
      if (threadPool.get())
         out << fw::fmt("pool.parallelFor(factDB::BlockedRange(0, {}.size(), {}), [&](factDB::BlockedRange range) {{", tableRef, chunkSize.get()) << fw::endl();
      else
         out << fw::fmt("tbb::parallel_for(tbb::blocked_range<size_t>(0, {}.size(), {}), [&](tbb::blocked_range<size_t> range) {{", tableRef, chunkSize.get()) << fw::endl();
      tableScan.consumerConsume(ConsumeStage::TLSPrepare, *this);
      out << fw::fmt("  for (auto idx = range.begin(); idx < range.end(); idx++) {{") << fw::endl()
          << fw::fmt("    auto& iterator = {}[idx];", tableRef) << fw::endl();
   } else {
      tableScan.consumerConsume(ConsumeStage::TLSPrepare, *this);
      out << fw::fmt("for (size_t idxHelper = 0; const auto &iterator : {}) {{", tableRef) << fw::endl()
          << fw::fmt("  size_t idx = idxHelper++;") << fw::endl();
   }
   OrderedIUSet allRequiredIUs = tableScan.getRequiredIus();
   allRequiredIUs.merge(tableScan.getRequiredIusPipeline());
   IUSet allRequiredIUsNoDuplicates(allRequiredIUs.begin(), allRequiredIUs.end());

   for (const auto* iu : allRequiredIUsNoDuplicates)
      out << fw::fmt("auto& {} = iterator.{}();", iu, iu->column) << fw::endl();
   out << fw::endl();

   out << fw::context_switch() << fw::endl();
   tableScan.consumerConsume(ConsumeStage::BeforeLoop, *this);
   out << fw::context_switch() << fw::endl();
   tableScan.consumerConsume(ConsumeStage::InLoop, *this);
   out << fw::context_switch() << fw::endl();
   tableScan.consumerConsume(ConsumeStage::AfterLoop, *this);
   out << fw::context_switch() << fw::endl();

   out << "}" << fw::endl()
       << (parallelFor.get() ? "});" : "") << fw::endl_non_empty()
       << timer.end() << fw::endl_non_empty()
       << timer.print(fw::lc("std::cout"));
}

} // namespace factDB::algebra::visitors
