#include "factDB/queryc/QueryParseContext.hpp"
#include "factDB/algebra/visitors/CodegenExpanded.hpp"
#include "factDB/algebra/visitors/CodegenFactorized.hpp"
#include "factDB/algebra/visitors/FactorizationGain.hpp"
#include "factDB/algebra/visitors/GraphvizQueryPlan.hpp"
#include "factDB/algebra/visitors/MaterializedExecution.hpp"
#include "factDB/algebra/visitors/PushModelExecution.hpp"
#include "factDB/algebra/visitors/RelationLoader.hpp"
#include "factDB/algebra/visitors/SQLGenerator.hpp"
#include "factDB/algebra/visitors/factorized/FTreeComposition.hpp"
#include "factDB/algebra/visitors/factorized/FTreeDifference.hpp"
#include "factDB/algebra/visitors/factorized/FTreePredecessorDifference.hpp"
#include "factDB/algebra/visitors/umbra/UmbraVisitor.hpp"
#include "factDB/config.h"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/opt/Optimizer.hpp"
#include "factDB/opt/estimators/EstimateAlgebraPlan.hpp"
#include "factDB/queryc/QueryHandle.hpp"
#include "factDB/queryc/queryDispatcher/compiletime/QueryImplementationGenerator.hpp"
#include "factDB/queryc/queryDispatcher/runtime/KnownQueries.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "tbb/tbb.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <unordered_set>
// ---------------------------------------------------------------------------------------------------
namespace factDB::queryc {
// ---------------------------------------------------------------------------------------------------
using namespace std;
// ---------------------------------------------------------------------------------------------------
static Setting<ExecutionMode> executionMode("codegen.executionMode"sv, Materialized, &executionModeParser);
static Setting<size_t> runRepeats("run.repeat"sv, 1);
static Setting<size_t> runWarmupRuns("run.warmup"sv, 0);
static Setting<bool> generateCodeSetting("codegen.generate", true);
static Setting<bool> pregenerateSetting("codegen.pregenerateQueries"sv, false);
// ---------------------------------------------------------------------------------------------------
std::string QueryParseContext::getPathPregenerated(const QueryProperties& qp, const std::string& suffix) {
   auto suffixStr = (suffix[0] == '.' ? suffix.substr(1) : suffix);
   return "factDB/gen/query/query_" + executionModeParser.output(qp.mode) + "_" + qp.genModeString() + "_" + qp.queryname + "." + suffixStr;
}
// ---------------------------------------------------------------------------------------------------
std::string getPath(size_t queryIdx, const std::string& suffix) {
   if (pregenerateSetting.get()) {
      auto qp = QueryProperties::fromCurrentSettings();
      return CURRENT_SRC_DIR "/" + QueryParseContext::getPathPregenerated(qp, suffix);
   } else {
      auto suffixStr = (suffix[0] == '.' ? suffix.substr(1) : suffix);
      return CURRENT_BINARY_DIR "/gen/queries/query" + std::to_string(queryIdx) + "." + suffixStr;
   }
}
// ---------------------------------------------------------------------------------------------------
std::string QueryParseContext::getOutputFolder() {
   if (pregenerateSetting.get()) {
      return std::filesystem::path(CURRENT_SRC_DIR "/factDB/gen/query/query").parent_path().string();
   } else {
      return std::filesystem::path(CURRENT_BINARY_DIR "/gen/queries/query").parent_path().string();
   }
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<factDB::algebra::visitors::CodegenExpanded> QueryParseContext::getCodegenVisitor(const Database& db, FileWriter& stream) {
   switch (executionMode.get()) {
      case CodegenFactorized:
      case CodegenFactorizedNaive:
         return std::make_unique<algebra::visitors::CodegenFactorized>(db, stream);
      case CodegenFlat: return std::make_unique<algebra::visitors::CodegenExpanded>(db, stream);
      case Materialized:
      case PushModel:
      case Graphviz:
      case FactorizationGain:
      case FTreeComposition:
      case FTreeDifference:
      case FTreePredecessorDifference:
      case FlatSizes:
      case Costs:
      case SQL:
      case UmbraLocal:
      case UmbraServer:
         unreachable();
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
bool generatesCode() {
   switch (executionMode.get()) {
      case CodegenFlat:
      case CodegenFactorized:
      case CodegenFactorizedNaive:
         return true;
      case Materialized:
      case PushModel:
      case Graphviz:
      case FactorizationGain:
      case FTreeComposition:
      case FTreeDifference:
      case FTreePredecessorDifference:
      case FlatSizes:
      case Costs:
      case SQL:
      case UmbraLocal:
      case UmbraServer:
         return false;
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
bool needsTables(ExecutionMode mode) {
   switch (mode) {
      case Graphviz:
      case FactorizationGain:
      case FTreeComposition:
      case FTreeDifference:
      case FTreePredecessorDifference:
      case FlatSizes:
      case SQL:
      case UmbraLocal:
      case UmbraServer:
         return false;
      case CodegenFlat:
      case CodegenFactorized:
         return !pregenerateSetting.get();
      case CodegenFactorizedNaive:
      case Materialized:
      case PushModel:
         return true;
      case Costs:
         return false;
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
QueryParseContext::QueryParseContext(Database& db, const std::string& suffix) : database(db), queryIdx(++*SettingBase::getSetting<size_t>("globalQueryIdx"sv)), source(getPath(queryIdx, suffix)) {
   if (!SettingBase::getSetting<bool>("statistics.silent")->get())
      std::cout << "gen code: " << source << std::endl;
   if (generateCodeSetting.get() && generatesCode()) {
      if (!std::filesystem::is_directory(getOutputFolder()))
         std::filesystem::create_directories(getOutputFolder());
      stream = std::ofstream(source, ofstream::trunc);
      if (stream.fail())
         throw std::runtime_error("unable to open ofstream");
   }
}
// ---------------------------------------------------------------------------------------------------
FileWriter QueryParseContext::getFileWriter() {
   return {stream};
}
// ---------------------------------------------------------------------------------------------------
void QueryParseContext::generateCodeCodegenVisitor(std::unique_ptr<algebra::Operator> tree, const factDB::OrderedIUSet& requiredColumns, bool optimize) {
   assert(tree->getType() == algebra::Operator::Print || tree->getType() == algebra::Operator::Count);

   if (generateCodeSetting.get()) {
      auto actuallyRequired = requiredColumns;

      auto writer = getFileWriter();
      auto visitor = getCodegenVisitor(database, writer);

      static Setting<bool>& doOptimize = *SettingBase::getSetting<bool>("optimizer.doOptimize");
      if (optimize && doOptimize.get())
         tree = opt::QueryOptimizer::optimize(database, std::move(tree), actuallyRequired);

      compiler.compile(tree, actuallyRequired, *visitor);
      stream.close();
   }
}
// ---------------------------------------------------------------------------------------------------
void QueryParseContext::generateCode(const std::function<void(FileWriter&)>& query) {
   if (generateCodeSetting.get()) {
      auto writer = getFileWriter();
      query(writer);

      stream.close();
   }
}
// ---------------------------------------------------------------------------------------------------
RawHandle QueryParseContext::compile(bool useGtest) {
   cxxCompiler.useGtest(useGtest);
   return cxxCompiler.compile(source, true, false, CxxCompiler::RawHandleTag);
}
// ---------------------------------------------------------------------------------------------------
void QueryParseContext::executeVisitor(std::unique_ptr<algebra::Operator> tree, const factDB::OrderedIUSet& requiredColumns, factDB::Database& db, std::ostream& outStream, bool optimize) {
   std::unique_ptr<algebra::visitors::AlgebraVisitor> visitor;
   switch (executionMode.get()) {
      case Materialized: visitor = std::make_unique<algebra::visitors::MaterializedExecution>(db, outStream); break;
      case PushModel: visitor = std::make_unique<algebra::visitors::PushModelExecution>(db, outStream); break;
      case Graphviz: visitor = std::make_unique<algebra::visitors::GraphvizQueryPlan>(db, std::cout); break;
      case FactorizationGain: visitor = std::make_unique<algebra::visitors::FactorizationGain>(db, std::cout); break;
      case FTreeComposition: visitor = std::make_unique<algebra::visitors::FTreeCompositionVisualizer>(std::cout); break;
      case FTreeDifference: visitor = std::make_unique<algebra::visitors::FTreeDifference>(std::cout); break;
      case FTreePredecessorDifference: visitor = std::make_unique<algebra::visitors::FTreePredecessorDifferenceProcessor>(db, std::cout); break;
      case FlatSizes: visitor = std::make_unique<algebra::visitors::FlatSizes>(db, std::cout); break;
      case Costs: visitor = std::make_unique<opt::EstimateAlgebraPlanHelper>(db, std::cout); break;
      case SQL: visitor = std::make_unique<algebra::visitors::SQLGenerator>(std::cout, db); break;
      case UmbraLocal: visitor = std::make_unique<algebra::visitors::UmbraVisitorLocal>(db, outStream); break;
      case UmbraServer: visitor = std::make_unique<algebra::visitors::UmbraVisitorServer>(db, outStream); break;
      default: unreachable();
   }

   auto actuallyRequired = requiredColumns;

   static Setting<bool>& doOptimize = *SettingBase::getSetting<bool>("optimizer.doOptimize");
   if (optimize && doOptimize.get())
      tree = opt::QueryOptimizer::optimize(database, std::move(tree), actuallyRequired);

   auto silent = SettingBase::getSetting<bool>("statistics.silent")->get();
   // warmups
   for (size_t i = 0; i < runWarmupRuns.get(); i++) {
      compiler.compile(tree, actuallyRequired, *visitor);
      if (!silent) std::cout << "---------------------------------------------------------------------------" << std::endl;
   }

   // execution
   for (size_t i = 0; i < runRepeats.get(); i++) {
      auto start_execution = std::chrono::high_resolution_clock::now();
      compiler.compile(tree, actuallyRequired, *visitor);
      auto stop_execution = std::chrono::high_resolution_clock::now();
      if (!silent) std::cout << "---------------------------------------------------------------------------" << std::endl;
      record.addExecutionTime(chrono::duration<double>(stop_execution - start_execution).count());
   }
}
// ---------------------------------------------------------------------------------------------------
void QueryParseContext::prepareCode(std::unique_ptr<algebra::Operator> tree, const OrderedIUSet& requiredColumns, bool optimize) {
   auto start_compile = std::chrono::steady_clock::now();
   generateCodeCodegenVisitor(std::move(tree), requiredColumns, optimize);
   auto stop_compile = std::chrono::steady_clock::now();

   QueryImplementationGenerator::addQuery(QueryProperties::fromCurrentSettings());
   record.addCompilationTime(chrono::duration<double>(stop_compile - start_compile).count());
}
// ---------------------------------------------------------------------------------------------------
void QueryParseContext::compileAndExecuteVisitor(std::unique_ptr<algebra::Operator> tree, const factDB::OrderedIUSet& requiredColumns, factDB::Database& db, std::ostream& outStream, bool optimize) {
   // compile existing tree (incl. codegen)
   // compilation
   auto start_compile = std::chrono::steady_clock::now();
   generateCodeCodegenVisitor(std::move(tree), requiredColumns, optimize);
   auto rawHandle = compile();
   auto handle = rawHandle.loadQueryHandle();
   auto stop_compile = std::chrono::steady_clock::now();

   record.addCompilationTime(chrono::duration<double>(stop_compile - start_compile).count());

   // warmups
   for (size_t i = 0; i < runWarmupRuns.get(); i++) {
      handle(db, outStream, PerformanceRecorderExecution{});
      std::cout << "---------------------------------------------------------------------------" << std::endl;
   }

   // execution
   for (size_t i = 0; i < runRepeats.get(); i++) {
      handle(db, outStream, record.startExecutionRecorder());
      std::cout << "---------------------------------------------------------------------------" << std::endl;
   }
}
// ---------------------------------------------------------------------------------------------------
void QueryParseContext::compileAndExecute(unique_ptr<algebra::Operator> tree, const factDB::OrderedIUSet& requiredColumns, factDB::Database& db, std::ostream& outStream, bool optimize, bool cacheRelations) {
   // load relations
   algebra::visitors::RelationLoader loader(db);
   tree->produce(loader);
   if (needsTables(executionMode.get()))
      loader.load();

   switch (executionMode.get()) {
      case Materialized:
      case PushModel:
      case Graphviz:
      case FactorizationGain:
      case FTreeComposition:
      case FTreeDifference:
      case FTreePredecessorDifference:
      case FlatSizes:
      case Costs:
      case SQL:
      case UmbraLocal:
      case UmbraServer:
         executeVisitor(std::move(tree), requiredColumns, db, outStream, optimize);
         break;
      case CodegenFlat:
      case CodegenFactorized:
      case CodegenFactorizedNaive:
         if (pregenerateSetting.get())
            prepareCode(std::move(tree), requiredColumns, optimize);
         else
            compileAndExecuteVisitor(std::move(tree), requiredColumns, db, outStream, optimize);
         break;
   }

   if (needsTables(executionMode.get()) && !cacheRelations)
      loader.unload();
   record.printStatistics(std::cerr);
}
// ---------------------------------------------------------------------------------------------------
QueryParseContext::~QueryParseContext() {
   if (stream.is_open())
      stream.close();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::queryc
// ---------------------------------------------------------------------------------------------------