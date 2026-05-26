// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include "factDB/SQLExecution.hpp"
#include "factDB/config.h"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/list/ListTypeEnum.hpp"
#include "factDB/newftree/generator/fnode/Graphviz.hpp"
#include "factDB/opt/algs/OptimizerAlgorithms.hpp"
#include "factDB/queryc/ExecutionMode.hpp"
#include "factDB/queryc/queryDispatcher/compiletime/QueryImplementationBenchmark.hpp"
#include "factDB/queryc/queryDispatcher/compiletime/QueryImplementationGenerator.hpp"
#include "factDB/queryc/queryDispatcher/runtime/QueryDispatcher.hpp"
#include "factDB/util/CommandLine.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "factDB/util/Perfetto.hpp"
#include "factDB/util/SQLite3Shutdown.hpp"
#include <fstream>
#include <iostream>
#include <fmt/format.h>
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
struct Options : public commandLine::CommandLine {
   commandLine::Option<bool> silent;
   commandLine::Option<bool> help;
   commandLine::Option<bool> parallel;
   commandLine::Option<bool> inlining;
   commandLine::Option<bool> caching;
   commandLine::Option<bool> bottomInserts;
   commandLine::Option<bool> naiveMerge;
   commandLine::Option<unsigned> warmups;
   commandLine::Option<unsigned> runs;
   commandLine::Option<std::string> mode;
   commandLine::Option<std::string> filter;
   commandLine::Option<bool> validate;

   Options() : CommandLine("benchCE", "[options] [ce|ssb_sf1|ssb_sf10]") {
      add(help).longName("help").description("Print this help message.");
      add(silent).shortName('s').longName("silent").description("Supress output");
      add(parallel).shortName('p').longName("parallel").description("Parallel Execution");
      add(inlining).shortName('i').longName("inlining").description("Inline first element");
      add(caching).shortName('c').longName("caching").description("Cache the count aggregates");
      add(bottomInserts).shortName('b').longName("bottomInserts").description("Enable bottom inserts");
      add(warmups).shortName('w').longName("warmups").description("Number of warmup runs");
      add(runs).shortName('r').longName("runs").description("Number of measured runs");
      add(mode).shortName('m').longName("mode").description("Execution Mode");
      add(naiveMerge).shortName('n').longName("naiveMerge").description("Enable Naive Merge strategy.");
      add(validate).shortName('v').longName("validate").description("Validate the query result.");
      add(filter).shortName('f').longName("filter").description("Filter the query name.");
   }
   using CommandLine::add;
};
// ---------------------------------------------------------------------------------------------------
std::fstream getQueriesStream(const std::string& schema) {
   if (schema == "ce") {
      return std::fstream(CURRENT_SRC_DIR "/bench/ce/all_queries.sql", std::ios_base::in);
   } else if (schema == "ssb_sf1") {
      return std::fstream(CURRENT_SRC_DIR "/bench/ssb/sf1/all_queries.sql", std::ios_base::in);
   } else if (schema == "ssb_sf10") {
      return std::fstream(CURRENT_SRC_DIR "/bench/ssb/sf10/all_queries.sql", std::ios_base::in);
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
std::tuple<std::string, std::string> setQuerynameFilter(const std::string& schema, const std::string& filter) {
   // create filter to execute only a subset of queries (relevant if ssb and ce queries are generated)
   auto& querynameFilter = *SettingBase::getSetting<std::string>("queryname.filter");
   if (schema == "ce") {
      querynameFilter.set("^(?=(dblp|epinions|hetio|job|watdiv|yago)_)");
   } else if (schema == "ssb_sf1") {
      querynameFilter.set("^(?=(ssb_sf1_query)_)");
   } else if (schema == "ssb_sf10") {
      querynameFilter.set("^(?=(ssb_sf10_query)_)");
   }

   if (!filter.empty())
      querynameFilter.set(fmt::format("{}(?={})", querynameFilter.get(), filter));
   auto genBlacklist = [&](std::initializer_list<std::string_view> blacklist) {
      if (querynameFilter.get().empty())
         return fmt::format("^(?!({})).*", fmt::join(blacklist, "|"));
      else
         return fmt::format("{}(?!({})).*", querynameFilter.get(), fmt::join(blacklist, "|"));
   };

   // exclude some queries (out of memory)
   auto blacklistFact = genBlacklist({
      "hetio_cyclic_q12_12", // out of memory on xeon1
      "hetio_cyclic_q*",
      "hetio_cyclic_q2_13",
      "hetio_cyclic_q2_17",
      "hetio_cyclic_q3_11",
      "hetio_cyclic_q3_21",
      "hetio_cyclic_q3_24",
      "yago_cyclic_Cycle_6_04",
      "dblp_cyclic_q9_06", // umbra iu error
      "watdiv_cyclic_q9_02",
      "watdiv_cyclic_q3_10", // oom
      "watdiv_cyclic_q12_00", // oom
      "yago_cyclic_Cycle_6_09", // oom
   });

   auto blacklistFlat = genBlacklist({
      "dblp_acyclic_218_15", // out of memory on xeon0
      "hetio_cyclic_q12_12",
      "hetio_cyclic_q.*",
      "watdiv_cyclic_q3_10" // oom
   });

   return {blacklistFact, blacklistFlat};
}
// ---------------------------------------------------------------------------------------------------
size_t executeSQL(Database& db, const std::string& schema, queryc::ExecutionMode mode, bool singleRun = false, const std::string& blacklist = "", opt::OptimizerAlgorithm::Algorithm optAlg = opt::OptimizerAlgorithm::DPFactBushy) {
   [[maybe_unused]] auto tmpCodegenExecutionMode = factDB::SettingBase::getSetting("codegen.executionMode")->setTemporary(mode);
   [[maybe_unused]] auto tmpOptimizerAlg = factDB::SettingBase::getSetting("optimizer.algorithm")->setTemporary(optAlg);

   std::vector<ScopeSetter<int>> scope_setters;
   if (singleRun) {
      scope_setters.emplace_back(factDB::SettingBase::getSetting("run.warmup")->setTemporary(0));
      scope_setters.emplace_back(factDB::SettingBase::getSetting("run.repeat")->setTemporary(1));
   }

   if (!blacklist.empty())
      SettingBase::getSetting<std::string>("queryname.filter")->set(blacklist);

   SQLExecutor sqlExecutor(db);
   std::fstream queries = getQueriesStream(schema);

   if (!queries) {
      std::cerr << "Queries file was not opened succesfully" << std::endl;
      return 1;
   }
   sqlExecutor.executeSQL(queries);
   return 0;
}
// ---------------------------------------------------------------------------------------------------
void printBenchmarkHeader(const std::string& middle, int totalWidth = 80) {
   int middleLength = middle.length();
   if (middleLength >= totalWidth) {
      std::cout << middle << std::endl;
      return;
   }

   int padding = totalWidth - middleLength;
   int padLeft = padding / 2;
   int padRight = padding - padLeft;

   std::cout << std::string(padLeft - 1, '-') << " " << middle << " " << std::string(padRight - 1, '-') << std::endl;
}
// ---------------------------------------------------------------------------------------------------
void runBenchmarks(queryc::ExecutionMode execution, Database& db) {
   // factDB::generated::runBenchmarks(execution, db);
   factDB::queryc::QueryDispatcher::runBenchmarks(execution, db);
}
// ---------------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
   PerfettoTracer::registerThread("main");
   DoSQLite3Shutdown doShutdown;
   Options options;
   std::stringstream s;
   options.parse(s, argc, argv);

   if (options.help.get()) {
      options.showHelp(std::cout);
      return 0;
   } else if (options.getPositional().size() != 1) {
      options.showHelp(std::cout);
      return 1;
   } else if (auto& schema = options.getPositional()[0]; schema != "ce" && schema != "ssb_sf1" && schema != "ssb_sf10") {
      options.showHelp(std::cout);
      return 1;
   }

   auto& schema = options.getPositional()[0];
   SettingBase::getSetting<std::string>("schema.key")->set(schema);
   bool needTables = (options.mode.get() != "CodegenFactorized" && options.mode.get() != "CodegenFlat");
   auto db = factDB::DatabaseLoadUtil::genById(schema, {}, needTables);

   bool fix = (options.runs.get() + options.warmups.get()) == 0;
   auto tmpRunWarmup = factDB::SettingBase::getSetting("run.warmup")->setTemporary(options.warmups.get());
   auto tmpRunRepeat = factDB::SettingBase::getSetting("run.repeat")->setTemporary(options.runs.get() + fix);
   auto tmpValResult = factDB::SettingBase::getSetting<bool>("codegen.validateResultSize")->setTemporary(options.validate.get());
   auto tmpNaiveMerge = factDB::SettingBase::getSetting<bool>("codegen.factorized.naiveMerge")->setTemporary(options.naiveMerge.get());
   auto tmpInlining = factDB::SettingBase::getSetting("codegen.factorized.inline")->setTemporary(options.inlining.get());
   auto tmpCaching = factDB::SettingBase::getSetting("codegen.cacheCountStar")->setTemporary(options.caching.get());
   auto tmpBottomInserts = factDB::SettingBase::getSetting("opt.doBottomInserts")->setTemporary(options.bottomInserts.get());

   auto tmpParallelFor = factDB::SettingBase::getSetting("codegen.parallelFor")->setTemporary(options.parallel.get());
   auto tmpParallelForTLS = factDB::SettingBase::getSetting("codegen.parallelFor.tls")->setTemporary(options.parallel.get());
   auto tmpSilence = SettingBase::getSetting<bool>("statistics.silent")->setTemporary(options.silent.get());
   auto tmpPQueryname = SettingBase::getSetting<bool>("queryname.print")->setTemporary(!options.silent.get());
   SettingBase::getSetting<bool>("codegen.pregenerateQueries")->set(true);

   auto [blacklistFact, blacklistFlat] = setQuerynameFilter(schema, options.filter.get());

   // different execution modes. If the modes are not described by an comment, they are deprecated.
   if (options.mode.get() == "Graphviz" || options.mode.get().empty()) {
      // print queryplans in graphviz format
      printBenchmarkHeader("Graphviz");
      if (executeSQL(db, schema, queryc::ExecutionMode::Graphviz, true, blacklistFact) != 0) return 1;
   } else if (options.mode.get() == "CodegenFactorized") {
      // generate factorized query files with bushy plans
      printBenchmarkHeader("CodegenFactorized");
      auto tmpListType = factDB::SettingBase::getSetting<factDB::infra::list::ListTypeEnum>("codegen.factorized.listType")->setTemporary(infra::list::FastDequeEnum);
      if (executeSQL(db, schema, queryc::ExecutionMode::CodegenFactorized) != 0) return 1;
   } else if (options.mode.get() == "CodegenFactorizedLeftDeep") {
      // generate factorized query files with left-deep plans
      printBenchmarkHeader("CodegenFactorizedLeftDeep");
      auto tmpListType = factDB::SettingBase::getSetting<factDB::infra::list::ListTypeEnum>("codegen.factorized.listType")->setTemporary(infra::list::FastDequeEnum);
      if (executeSQL(db, schema, queryc::ExecutionMode::CodegenFactorized, true, {}, opt::OptimizerAlgorithm::DPFactLeftDeep) != 0) return 1;
   } else if (options.mode.get() == "CodegenFactorizedPregen" || options.mode.get() == "CodegenFactorizedLeftDeepPregen") {
      // run the generated factorized queries
      SettingBase::getSetting<std::string>("queryname.filter")->set(blacklistFact);
      printBenchmarkHeader("CodegenFactorizedPregen");
      runBenchmarks(queryc::ExecutionMode::CodegenFactorized, db);
   } else if (options.mode.get() == "CodegenFlat") {
      // generate flat query code (bushy plans)
      printBenchmarkHeader("CodegenFlat");
      if (executeSQL(db, schema, queryc::ExecutionMode::CodegenFlat, true, blacklistFact) != 0) return 1;
   } else if (options.mode.get() == "CodegenFlatLeftDeep") {
      // generate flat query plans (left deep plans)
      printBenchmarkHeader("CodegenFlatLeftDeep");
      if (executeSQL(db, schema, queryc::ExecutionMode::CodegenFlat, true, blacklistFact, opt::OptimizerAlgorithm::DPFactLeftDeep) != 0) return 1;
   } else if (options.mode.get() == "UmbraLocal") {
      // deprecated: run umbra
      printBenchmarkHeader("UmbraLocal");
      if (executeSQL(db, schema, queryc::ExecutionMode::UmbraLocal) != 0) return 1;
   } else if (options.mode.get() == "FactorizationGain") {
      printBenchmarkHeader("FactorizationGain");
      if (executeSQL(db, schema, queryc::ExecutionMode::FactorizationGain, true) != 0) return 1;
   } else if (options.mode.get() == "FlatSizes") {
      printBenchmarkHeader("FlatSizes");
      if (executeSQL(db, schema, queryc::ExecutionMode::FlatSizes, true) != 0) return 1;
   } else if (options.mode.get() == "CodegenFlatFlatTree") {
      printBenchmarkHeader("CodegenFlatFlatTree");
      if (executeSQL(db, schema, queryc::ExecutionMode::CodegenFlat, false, "", opt::OptimizerAlgorithm::DPSize) != 0) return 1;
   } else if (options.mode.get() == "CodegenFlatPregen" || options.mode.get() == "CodegenFlatLeftDeepPregen") {
      // run the flat, pregenerated queries.
      printBenchmarkHeader("CodegenFlatPregen");
      SettingBase::getSetting<std::string>("queryname.filter")->set(blacklistFlat);
      runBenchmarks(queryc::ExecutionMode::CodegenFlat, db);
   } else if (options.mode.get() == "FTreeComposition") {
      printBenchmarkHeader("FTreeComposition");
      factDB::SettingBase::getSetting<bool>("codegen.factorized.naiveMerge")->set(false);
      if (executeSQL(db, schema, queryc::ExecutionMode::FTreeComposition, true) != 0) return 1;
   } else if (options.mode.get() == "FTreeDiff") {
      printBenchmarkHeader("FTreeDiff");
      factDB::SettingBase::getSetting<bool>("codegen.factorized.naiveMerge")->set(false);
      if (executeSQL(db, schema, queryc::ExecutionMode::FTreeDifference, true) != 0) return 1;
   } else if (options.mode.get() == "FTreePredecessors") {
      printBenchmarkHeader("FTreePredecessors");
      factDB::SettingBase::getSetting<bool>("codegen.factorized.naiveMerge")->set(false);
      if (executeSQL(db, schema, queryc::ExecutionMode::FTreePredecessorDifference, true) != 0) return 1;
   } else if (options.mode.get() == "FactorizedCost") {
      printBenchmarkHeader("FactorizedCost");
      factDB::SettingBase::getSetting<bool>("optimizer.estimateFactorizedCosts")->set(true);
      if (executeSQL(db, schema, queryc::ExecutionMode::Costs, true) != 0) return 1;
   } else if (options.mode.get() == "FlatCost") {
      printBenchmarkHeader("FlatCost");
      factDB::SettingBase::getSetting<bool>("optimizer.estimateFactorizedCosts")->set(false);
      if (executeSQL(db, schema, queryc::ExecutionMode::Costs, true) != 0) return 1;
   } else {
      std::cout << "Unkown exeuction mode: " << options.mode.get() << std::endl;
      return 1;
   }

   try {
      queryc::QueryImplementationGenerator qg;
   } catch (std::runtime_error& e) {
      std::cerr << "Did not create updated query file, because: " << e.what() << std::endl;
   }

   return 0;
}
// ---------------------------------------------------------------------------------------------------
