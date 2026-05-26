// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/queryDispatcher/runtime/QueryDispatcher.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/threading/LocalState.hpp"
#include "factDB/queryc/PerformanceRecord.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/util/Perfetto.hpp"
#include <regex>
// ---------------------------------------------------------------------------------------------------
using namespace factDB::queryc;
// ---------------------------------------------------------------------------------------------------
static std::vector<QueryDispatcher::QueryDescriptor> queryDescriptors;
// ---------------------------------------------------------------------------------------------------
void QueryDispatcher::registerQuery(const QueryProperties& qp, generated::QueryFunctionType query, generated::RequiredRelationsFunctionType requiredRelations, generated::SchemaIdentifierFunctionType schemaIdentifier) {
   QueryDispatcher::QueryDescriptor desc(qp, query, requiredRelations, schemaIdentifier);
   registerQuery(desc);
}
// ---------------------------------------------------------------------------------------------------
void QueryDispatcher::registerQuery(QueryDescriptor descriptor) {
   queryDescriptors.emplace_back(descriptor);
}
// ---------------------------------------------------------------------------------------------------
const std::vector<QueryDispatcher::QueryDescriptor>& QueryDispatcher::getQueryDescriptors() {
   return queryDescriptors;
}
// ---------------------------------------------------------------------------------------------------
const QueryDispatcher::QueryDescriptor& QueryDispatcher::getQueryDescriptor(const QueryProperties& qp) {
   for (auto& desc : queryDescriptors) {
      if (desc.qp == qp)
         return desc;
   }
   throw std::runtime_error("No query found for the given properties.");
}
// ---------------------------------------------------------------------------------------------------
void QueryDispatcher::benchmarkQuery(factDB::Database& db, const QueryDescriptor& query) {
   auto warmups = factDB::SettingBase::getSetting<size_t>("run.warmup")->get();
   auto runs = factDB::SettingBase::getSetting<size_t>("run.repeat")->get();
   auto printQueryname = factDB::SettingBase::getSetting<bool>("queryname.print")->get();
   auto& queryname = query.qp.queryname;
   queryc::PerformanceRecord record;

   auto& pattern = SettingBase::getSetting<std::string>("queryname.filter")->get();
   if (!pattern.empty() && !std::regex_search(queryname, std::regex(pattern))) {
      if (printQueryname)
         std::cout << queryname << " skipped, not matching regex" << std::endl;
      return;
   }
   auto ts = SettingBase::getSetting<std::string>("queryname")->setTemporary(queryname);
   if (printQueryname)
      std::cerr << "queryname: " << queryname << std::endl;

   for (auto rel : query.requiredRelations()) {
      if (!db.getTable(rel).loaded)
         db.loadRelation(rel);
   }

   std::cout << "warmup query ---------------------------------------------------------" << std::endl;
   for (size_t i = 0; i != warmups; ++i) {
      PerfettoTracer::Trace trace(std::string_view(query.qp.queryname));
      query.query(db, std::cout, queryc::PerformanceRecorderExecution{});
   }
   std::cout << "run    query ---------------------------------------------------------" << std::endl;
   for (size_t i = 0; i != runs; ++i) {
      PerfettoTracer::Trace trace(std::string_view(query.qp.queryname));
      query.query(db, std::cout, record.startExecutionRecorder());
   }
   auto tmp = SettingBase::getSetting<bool>("statistics.short")->setTemporary(false);
   record.printStatistics(std::cerr);

   for (auto rel : query.requiredRelations()) {
      //if (!db.getTable(rel).loaded)
      auto relId = db.getRelationID(rel);
      if (!queryname.starts_with("ssb"))
         db.unloadRelation(relId);
   }
}
// ---------------------------------------------------------------------------------------------------
void QueryDispatcher::runBenchmarks(ExecutionMode execution, Database& db) {
   LocalState localState;
   auto qp = QueryProperties::fromCurrentSettings();
   for (auto& descriptor : getQueryDescriptors()) {
      if (descriptor.qp.mode == execution && descriptor.qp.getFlagUInt() == qp.getFlagUInt()) {
         benchmarkQuery(db, descriptor);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
