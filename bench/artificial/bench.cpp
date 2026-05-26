// ---------------------------------------------------------------------------------------------------
#include "bench/artificial/DataGen.hpp"
#include "bench/artificial/QueryTrees.hpp"
#include "factDB/Database.hpp"
#include "factDB/SQLExecution.hpp"
#include "factDB/config.h"
#include "factDB/infra/IUSet.hpp"
#include "factDB/queryc/QueryParseContext.hpp"
#include "factDB/queryc/queryDispatcher/compiletime/QueryImplementationBenchmark.hpp"
#include "factDB/queryc/queryDispatcher/compiletime/QueryImplementationGenerator.hpp"
#include "factDB/queryc/queryDispatcher/runtime/QueryDispatcher.hpp"
#include "factDB/util/CommandLine.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include <factDB/infra/Setting.hpp>
#include <factDB/queryc/ExecutionMode.hpp>
#include <filesystem>
#include <iostream>
#include <fmt/format.h>
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
struct Options : public commandLine::CommandLine {
   commandLine::Option<bool> parallel;
   commandLine::Option<bool> help;
   commandLine::Option<unsigned> warmups;
   commandLine::Option<unsigned> runs;
   commandLine::Option<std::string> mode;
   commandLine::Option<std::string> type;
   commandLine::Option<bool> kuzu;
   commandLine::Option<bool> generateTable;
   commandLine::Option<bool> runAll;

   commandLine::Option<bool> bottomInsert;

   Options() : CommandLine("benchCE", "[options] [args]") {
      add(help).longName("help").description("Print this help message.");
      add(parallel).shortName('p').longName("parallel").description("Parallel Execution");
      add(warmups).shortName('w').longName("warmups").description("Number of warmup runs");
      add(runs).shortName('r').longName("runs").description("Number of measured runs");
      add(mode).shortName('m').longName("mode").description("Execution Mode");
      add(bottomInsert).shortName('b').longName("bottomInsert").description("Do bottom Insert.");
      add(generateTable).shortName('g').longName("generateTable").description("Generates the input tables.");
      add(runAll).shortName('a').longName("runAll").description("Runs all benchmarks.");

      add(type).shortName('t').longName("type").description("Benchmark Type");
      add(kuzu).shortName('k').longName("kuzu").description("Generate Bash Skript for queries");

      // add(validate).shortName('v').longName("validate").description("Validate the query result.");
   }
};
// ---------------------------------------------------------------------------------------------------
void setMode(const Options& options) {
   SettingBase::getSetting<bool>("codegen.pregenerateQueries")->set(true);
   auto executionModeSetting = SettingBase::getSetting<queryc::ExecutionMode>("codegen.executionMode");
   auto cacheCountStar = SettingBase::getSetting<bool>("codegen.cacheCountStar");
   cacheCountStar->set(true);
   constexpr size_t mode = 0;
   if (mode == 1 || (mode == 0 && (options.mode.get() == "Graphviz" || options.mode.get().empty()))) {
      std::cout << "------------------------------------              Plan               ---------------------------------------" << std::endl;
      executionModeSetting->set(queryc::ExecutionMode::Graphviz);
   } else if (mode == 2 || (mode == 0 && (options.mode.get() == "CodegenFactorized" || options.mode.get() == "CodegenFactorizedPregen"))) {
      std::cout << "------------------------------------Factorized + Tbb Parallel Vector ---------------------------------------" << std::endl;
      executionModeSetting->set(queryc::ExecutionMode::CodegenFactorized);
   } else if (mode == 2 || (mode == 0 && options.mode.get() == "CodegenFactorizedNoCache")) {
      std::cout << "------------------------------------Factorized + Tbb Parallel Vector ---------------------------------------" << std::endl;
      executionModeSetting->set(queryc::ExecutionMode::CodegenFactorized);
      cacheCountStar->set(false);
   } else if (mode == 4 || (mode == 0 && (options.mode.get() == "CodegenFlat" || options.mode.get() == "CodegenFlatPregen"))) {
      std::cout << "------------------------------------           CodegenFlat           ---------------------------------------" << std::endl;
      executionModeSetting->set(queryc::ExecutionMode::CodegenFlat);
   } else if (mode == 5 || (mode == 0 && options.mode.get() == "SQL")) {
      std::cout << "------------------------------------               SQL               ---------------------------------------" << std::endl;
      executionModeSetting->set(queryc::ExecutionMode::SQL);
   } else {
      throw std::runtime_error("Unkown exeuction mode: " + options.mode.get());
   }
}
// ---------------------------------------------------------------------------------------------------
void runBenchmarks(queryc::ExecutionMode execution, Database& db) {
   // factDB::generated::runBenchmarks(execution, db);
   factDB::queryc::QueryDispatcher::runBenchmarks(execution, db);
}
// ---------------------------------------------------------------------------------------------------
void executeQuery(Database& db, DataGen& data, QueryTrees::ReturnType& query, bool pregen, std::ostream* queriesSkript, algebra::JoinMode joinMode) {
   data.write();
   data.shuffle();
   DatabaseLoadUtil::loadTablePaths(db, data.getLoadPath());
   auto& [tree, queryname] = query;

   std::string fullQueryName = queryname + (joinMode == algebra::JoinMode::TopInsert ? "ti" : "bi");
   auto ts = SettingBase::getSetting<std::string>("queryname")->setTemporary(fullQueryName);
   std::cerr << "queryname: " << queryname << std::endl;
   if (pregen) {
      auto& querynameFilter = *SettingBase::getSetting<std::string>("queryname.filter");
      querynameFilter.set(fullQueryName);
      auto executionModeSetting = SettingBase::getSetting<queryc::ExecutionMode>("codegen.executionMode");
      auto printQueryname = factDB::SettingBase::getSetting<bool>("queryname.print")->setTemporary(false);
      runBenchmarks(executionModeSetting->get(), db);
   } else {
      queryc::QueryParseContext context(db);
      context.compileAndExecute(std::move(tree), {}, db, std::cout, false, false);
   }

   if (queriesSkript != nullptr)
      *queriesSkript << fmt::format("execute_query {} .{}", queryname, data.getOutPath()) << std::endl;
}
// ---------------------------------------------------------------------------------------------------
int main(int argc, char** argv) {
   SettingBase::getSetting<std::string>("schema.key")->set("artificial");
   Options options;
   options.parse(std::cout, argc, argv);

   if (options.help.get()) {
      options.showHelp(std::cout);
      return 0;
   }

   Database db = DatabaseCreationHelper::loadDatabase();

   bool fix = (options.runs.get() + options.warmups.get()) == 0;
   auto tmpRunWarmup = factDB::SettingBase::getSetting("run.warmup")->setTemporary(options.warmups.get());
   auto tmpRunRepeat = factDB::SettingBase::getSetting("run.repeat")->setTemporary(options.runs.get() + fix);
   auto tmpParallelFor = factDB::SettingBase::getSetting("codegen.parallelFor")->setTemporary(options.parallel.get());
   auto tmpParallelForTLS = factDB::SettingBase::getSetting("codegen.parallelFor.tls")->setTemporary(options.parallel.get());
   auto tmpUseTLS = SettingBase::getSetting<bool>("codegen.factorized.tlsBase")->setTemporary(options.parallel.get());
   auto tmpGenTables = factDB::SettingBase::getSetting("bench.artificial.generateData")->setTemporary(options.generateTable.get());
   SettingBase::getSetting<std::string>("opt.oracle.path.db")->set(CURRENT_SRC_DIR "/factDB/gen/estimates_artificial.db");
   setMode(options);
   bool pregen = (options.mode.get() == "CodegenFactorizedPregen") || (options.mode.get() == "CodegenFlatPregen");
   auto joinMode = options.bottomInsert.get() ? algebra::JoinMode::BottomInsert : algebra::JoinMode::TopInsert;

   std::vector<std::string> modes = {
      "growing1NoDups",
      "diagonalUniform",
      "diagonalZipf",
      "scaleUniform",
      "scaleZipf",
      "alphaZipf",
      "tradeoff"};
   std::unordered_map<std::string, size_t> modesMap;
   for (size_t i = 0; i != modes.size(); ++i) {
      modesMap[modes[i]] = i;
   }
   if (!modesMap.contains(options.type.get())) {
      std::cout << "NO MODE SPECIFIED, DO NOTHING" << std::endl;
      return 1;
   }

   size_t mode = modesMap[options.type.get()];
   // size_t mode = 4;
   std::unique_ptr<std::ofstream> queriesSkript;
   if (options.kuzu.get())
      queriesSkript = std::make_unique<std::ofstream>(fmt::format(CURRENT_SRC_DIR "/bench/artificial/dbms/{}_queries.sh", modes[mode]));

   if (mode == 0) {
      for (size_t i = 10'000; i <= 200'000; i += 20'000) {
         auto data = DataGen::genDataNoDuplicates(i, 10'000, 20'000);
         auto query = QueryTrees::genJoinsLeftDeep(db, data, joinMode);
         executeQuery(db, data, query, pregen, queriesSkript.get(), joinMode);
      }
   }
   if (options.runAll.get() || mode == 1) {
      for (size_t i = 10'000; i <= 200'000; i += 20'000) {
         auto data = DataGen::genUniformDistribution(i, i, i, 1000, 1000, 1000);
         auto query = QueryTrees::genJoinsLeftDeep(db, data, joinMode);
         executeQuery(db, data, query, pregen, queriesSkript.get(), joinMode);
      }
   }
   if (options.runAll.get() || mode == 2) {
      for (size_t i : {100, 1000, 2000, 3000, 5000, 10'000, 15'000, 20'000}) {
         // for (size_t i = 10'000; i <= 200'000; i += 20'000) {
         auto data = DataGen::genZipfDistribution(i, i, i, 100);
         auto query = QueryTrees::genJoinsLeftDeep(db, data, joinMode);
         executeQuery(db, data, query, pregen, queriesSkript.get(), joinMode);
      }
   }
   if (options.runAll.get() || mode == 3) { // benchmark for uniform plot which increases duplication
      for (size_t i = 2; i <= 50; i += 2) {
         size_t size = 100'000;
         size_t upper_bound = size / i;
         auto data = DataGen::genUniformDistribution(size, size, size, upper_bound, upper_bound, upper_bound);
         auto query = QueryTrees::genJoinsLeftDeep(db, data, joinMode);
         std::get<1>(query) = fmt::format("{}_{}", std::get<1>(query), i);
         executeQuery(db, data, query, pregen, queriesSkript.get(), joinMode);
      }
   }
   if (options.runAll.get() || mode == 4) {
      for (double i : {.1, .2, .3, .4, .5, .6, .7, .8, .9, 1., 2., 3., 4., 5., 6., 7., 8., 9., 10.}) {
         size_t size = 10'000;
         i /= 1000;
         size_t upper_bound = size / i;
         auto data = DataGen::genZipfDistribution(size, size, size, upper_bound);
         auto query = QueryTrees::genJoinsLeftDeep(db, data, joinMode);
         auto full_name = fmt::format("{}_{}", std::get<1>(query), i);
         std::replace(full_name.begin(), full_name.end(), '.', 'o');
         std::get<1>(query) = full_name;
         executeQuery(db, data, query, pregen, queriesSkript.get(), joinMode);
      }
   }
   if (options.runAll.get() || mode == 5) { // benchmark for Zipfian plot which scales alpha
      for (double i = 0.5; i <= 2; i += .1) {
         size_t size = 10'000;
         size_t upper_bound = 1'000'000;
         auto data = DataGen::genZipfDistribution(size, size, size, upper_bound, i);
         auto query = QueryTrees::genJoinsLeftDeep(db, data, joinMode);
         auto full_name = fmt::format("{}_{}", std::get<1>(query), i);
         std::replace(full_name.begin(), full_name.end(), '.', 'o');
         std::get<1>(query) = full_name;
         executeQuery(db, data, query, pregen, queriesSkript.get(), joinMode);
      }
   }
   if (options.runAll.get() || mode == 6) {
      for (size_t i = 2; i <= 50; i += 2) {
         size_t size = 100'000;
         size_t upper_bound = size;
         auto data = DataGen::genUniformDistribution(size * i, size, size, upper_bound, upper_bound, upper_bound);
         auto query = QueryTrees::genJoinsLeftDeep(db, data, joinMode);
         std::get<1>(query) = fmt::format("{}_{}", std::get<1>(query), i);
         executeQuery(db, data, query, pregen, queriesSkript.get(), joinMode);
      }
   }

   if (options.kuzu.get())
      queriesSkript->close();
   try {
      queryc::QueryImplementationGenerator qg;
   } catch (std::runtime_error& e) {
      std::cerr << "Did not create updated query file, because: " << e.what() << std::endl;
   }
}
// ---------------------------------------------------------------------------------------------------
