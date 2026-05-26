#include "factDB/SQLExecution.hpp"
#include "factDB/config.h"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/query.hpp"
#include "factDB/queryc/PerformanceRecord.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "tbb/tbb.h"
#include "test/factDB/util/TranslationTest.hpp"
#include <chrono>
#include <iostream>
#include <list>
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
int main() {
   oneapi::tbb::task_scheduler_handle handle; // CodegenExpanded.cpp(143:70)
   handle = oneapi::tbb::task_scheduler_handle{oneapi::tbb::attach{}}; // CodegenExpanded.cpp(144:95)

   auto statShort = SettingBase::getSetting<bool>("statistics.short")->setTemporary(true);
   auto statTimeU = SettingBase::getSetting<queryc::TimeUnits::UnitsEmum>("statistics.TimeUnit")->setTemporary(queryc::TimeUnits::s);

   auto artificialPath = CURRENT_SRC_DIR "/bench/artificial/data/uniform_10000x10000x10000_1000x1000x1000/load.sql";
   Database db = DatabaseLoadUtil::genById(generated::getSchemaIdentifier(), generated::getRequiredRelations(), true, artificialPath);

   queryc::PerformanceRecord record;
   std::cout << "warmup query ---------------------------------------------------------" << std::endl;
   for (size_t i = 0; i != 5; ++i) {
      factDB::generated::query(db, std::cout, queryc::PerformanceRecorderExecution{});
   }
   std::cout << "run    query ---------------------------------------------------------" << std::endl;
   for (size_t i = 0; i != 10; ++i) {
      factDB::generated::query(db, std::cout, record.startExecutionRecorder());
   }
   auto tmp = SettingBase::getSetting<bool>("statistics.short")->setTemporary(false);
   record.printStatistics(std::cout);

   try { // CodegenExpanded.cpp(151:33)
      oneapi::tbb::finalize(handle); // CodegenExpanded.cpp(152:61)
      // oneTBB worker threads are terminated at this point. // CodegenExpanded.cpp(153:85)
   } catch (const oneapi::tbb::unsafe_wait&) { // CodegenExpanded.cpp(154:71)
      std::cout << "Failed to terminate the worker threads." << std::endl; // CodegenExpanded.cpp(155:102)
   } // CodegenExpanded.cpp(156:29)

   return 0;
}
// ---------------------------------------------------------------------------------------------------
