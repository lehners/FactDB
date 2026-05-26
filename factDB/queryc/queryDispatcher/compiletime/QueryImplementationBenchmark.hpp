// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include "factDB/gen/QueryHandler.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/query.hpp"
#include "factDB/queryc/PerformanceRecord.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include <regex>
// ---------------------------------------------------------------------------------------------------
namespace factDB::generated {
// ---------------------------------------------------------------------------------------------------
template <queryc::ExecutionMode execution, Query q>
void benchmarkQuery(Database& db) {
   auto warmups = factDB::SettingBase::getSetting<size_t>("run.warmup")->get();
   auto runs = factDB::SettingBase::getSetting<size_t>("run.repeat")->get();
   auto printQueryname = factDB::SettingBase::getSetting<bool>("queryname.print")->get();
   queryc::PerformanceRecord record;
   using ActualQuery = QuerySelector<execution, q>;

   auto curQueryname = std::string(ActualQuery::name());
   auto& pattern = SettingBase::getSetting<std::string>("queryname.filter")->get();
   if (!pattern.empty() && !std::regex_search(curQueryname, std::regex(pattern))) {
      if (printQueryname)
         std::cout << curQueryname << " skipped, not matching regex" << std::endl;
      return;
   }
   auto ts = SettingBase::getSetting<std::string>("queryname")->setTemporary(curQueryname);
   if (printQueryname)
      std::cerr << "queryname: " << curQueryname << std::endl;

   for (auto rel : ActualQuery::getRequiredRelations()) {
      if (!db.getTable(rel).loaded)
         db.loadRelation(rel);
   }

   std::cout << "warmup query ---------------------------------------------------------" << std::endl;
   for (size_t i = 0; i != warmups; ++i) {
      ActualQuery::query(db, std::cout, queryc::PerformanceRecorderExecution{});
   }
   std::cout << "run    query ---------------------------------------------------------" << std::endl;
   for (size_t i = 0; i != runs; ++i) {
      ActualQuery::query(db, std::cout, record.startExecutionRecorder());
   }
   auto tmp = SettingBase::getSetting<bool>("statistics.short")->setTemporary(false);
   record.printStatistics(std::cerr);

   for (auto rel : ActualQuery::getRequiredRelations()) {
      //if (!db.getTable(rel).loaded)
      auto relId = db.getRelationID(rel);
      if (!curQueryname.starts_with("ssb"))
         db.unloadRelation(relId);
   }
}
// ---------------------------------------------------------------------------------------------------
void runBenchmarks(queryc::ExecutionMode execution, Database& db);
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::generated
// ---------------------------------------------------------------------------------------------------
