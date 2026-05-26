// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/PerformanceRecord.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Setting.hpp"
#include "fmt/format.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::queryc {
// ---------------------------------------------------------------------------------------------------
using namespace std;
// ---------------------------------------------------------------------------------------------------
static constexpr auto timeUnitParser = settinghelper::makeEnumParser(
   std::tuple{TimeUnits::s, "s", 's'},
   std::tuple{TimeUnits::ms, "ms", 'm'},
   std::tuple{TimeUnits::µs, "µs", 'i'},
   std::tuple{TimeUnits::ns, "ns", 'n'});
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<TimeUnits::UnitsEmum> statisticsTimeUnit("statistics.TimeUnit", TimeUnits::s, &timeUnitParser);
static factDB::Setting<bool> statisticsShort("statistics.short", false);
static factDB::Setting<bool> statisticsIndividuals("statistics.individuals", false);
static factDB::Setting<bool> statisticsSilent("statistics.silent", false);
// ---------------------------------------------------------------------------------------------------
std::string_view TimeUnits::toString(factDB::queryc::TimeUnits::UnitsEmum unit) {
   switch (unit) {
      case s: return "s";
      case ms: return "ms";
      case µs: return "µs";
      case ns: return "ns";
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
std::string_view TimeUnits::getUnitString() {
   return TimeUnits::toString(statisticsTimeUnit.get());
}
// ---------------------------------------------------------------------------------------------------
TimeUnits::UnitsEmum TimeUnits::fromString(std::string_view unit_str) {
   if (unit_str == "s"sv) {
      return s;
   } else if (unit_str == "ms"sv) {
      return ms;
   } else if (unit_str == "µs"sv) {
      return µs;
   } else if (unit_str == "ns"sv) {
      return ns;
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
double TimeUnits::scaleTime(double time) {
   switch (statisticsTimeUnit.get()) {
      case s: return time * 1;
      case ms: return time * 1000;
      case µs: return time * 1000000;
      case ns: return time * 1000000000;
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
std::string PerformanceRecord::stats::genTimingString() const {
   return fmt::format("{:.7f} min, {:7f} max, {:7f} median, {:2f}% relMAD, {:7f} avg, {:7f} sdev",
                      TimeUnits::scaleTime(min), TimeUnits::scaleTime(max), TimeUnits::scaleTime(median),
                      TimeUnits::scaleTime(relMadTime()) * 100, TimeUnits::scaleTime(mean), TimeUnits::scaleTime(standardDeviation));
}
// ---------------------------------------------------------------------------------------------------
void PerformanceRecord::stats::calcStats() {
   if (times.empty()) return;
   auto times_copy = times;
   sort(times_copy.begin(), times_copy.end());
   min = times_copy.front();
   max = times_copy.back();
   median = times_copy[times.size() / 2];

   double sum = 0, squaredSum = 0;
   vector<double> deviations;
   deviations.reserve(times.size());

   for (const auto t : times) {
      sum += t;
      squaredSum += t * t;
      deviations.push_back(abs(t - median));
   }

   // Take the median of the deviations to get a robust measure for the variability
   nth_element(deviations.begin(), deviations.begin() + (deviations.size() / 2), deviations.end()); // NOLINT
   medianAbosulteDeviation = deviations[deviations.size() / 2];

   mean = sum / times.size(); // NOLINT
   standardDeviation = sqrt(squaredSum / times.size() - mean * mean); // NOLINT
}
// ---------------------------------------------------------------------------------------------------
void PerformanceRecord::computeStatistics() {
   statsCompilation.calcStats();
   statsExecution.calcStats();

   for (auto& intermediate : intermediateStats)
      intermediate.calcStats();
}
// ---------------------------------------------------------------------------------------------------
void PerformanceRecord::printTimes(std::ostream& stream) {
   for (size_t idx = 0; auto& time : statsExecution.times) {
      stream << idx++ << ": " << time << std::endl;
   }
}
// ---------------------------------------------------------------------------------------------------
void PerformanceRecord::printStatistics(std::ostream& stream) {
   if (statisticsSilent.get() || statsExecution.times.empty())
      return;
   computeStatistics();

   auto timeUnit = TimeUnits::toString(statisticsTimeUnit.get());
   std::string runCountInfoExecution, runCountInfoCompile;
   if (!statisticsShort.get()) {
      auto warmups = SettingBase::getSetting<size_t>("run.warmup"sv, false);
      auto repetitions = factDB::SettingBase::getSetting<size_t>("run.repeat"sv, false);

      runCountInfoExecution = fmt::format("{} warmups, {} runs, ", !warmups ? 0 : warmups->get(), getExecutionTimes().size());
      runCountInfoCompile = fmt::format("{} warmups, {} runs, ", !repetitions ? 0 : repetitions->get(), getCompilationTimes().size());
   }

   stream << fmt::format(" [{}] execution: ({}{})", timeUnit, runCountInfoExecution, statsExecution.genTimingString())
          << fmt::format(" [{}] compilation: ({}{})", timeUnit, runCountInfoCompile, statsCompilation.genTimingString()) << std::endl;
   for (size_t idx = 0; auto& intermediate : intermediateStats)
      stream << "    --" << ++idx << "(" << intermediate.genTimingString() << ")" << std::endl;
   stream << std::endl;

   if (statisticsIndividuals.get())
      for (auto a : getExecutionTimes())
         std::cout << "ex: " << TimeUnits::scaleTime(a) << std::endl;
}
// ---------------------------------------------------------------------------------------------------
void PerformanceRecorderExecution::finalize() const {
   if (record != nullptr && !invalidated) {
      record->addExecutionTime(std::chrono::duration<double>(times[idx - 1] - times[0]).count());
      if (record->intermediateStats.empty())
         record->intermediateStats.resize(idx - 1);
      for (size_t i = 0; i < idx - 1; ++i) {
         record->intermediateStats[i].times.push_back(std::chrono::duration<double>(times[i + 1] - times[i]).count());
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::queryc
