#ifndef H_FACTDB_FACTDB_QUERYC_PERFORMANCERECORD_HPP
#define H_FACTDB_FACTDB_QUERYC_PERFORMANCERECORD_HPP
// ---------------------------------------------------------------------------------------------------
#include <chrono>
#include <iostream>
#include <string_view>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::queryc {
// ---------------------------------------------------------------------------------------------------
struct TimeUnits {
   enum UnitsEmum { s,
                    ms,
                    µs,
                    ns };
   inline static std::string_view toString(UnitsEmum unit);

   static std::string_view getUnitString();

   static UnitsEmum fromString(std::string_view unit_str);

   static double scaleTime(double time);

   template <class Clock>
   [[nodiscard]] static double scaleTime(const std::chrono::time_point<Clock>& begin, const std::chrono::time_point<Clock>& end) {
      return scaleTime(std::chrono::duration<double>(end - begin).count());
   }
};
// ---------------------------------------------------------------------------------------------------
class PerformanceRecord;
// ---------------------------------------------------------------------------------------------------
class PerformanceRecorderExecution {
   PerformanceRecord* record = nullptr;
   size_t idx = 0;
   bool invalidated = false;
   std::vector<std::chrono::steady_clock::time_point> times;

   public:
   explicit PerformanceRecorderExecution() = default;
   explicit PerformanceRecorderExecution(PerformanceRecord& record_) : record(&record_) {}

   void time() { times[idx++] = std::chrono::steady_clock::now(); }
   void finalize() const;
   void start() {
      times.resize(100);
      time();
   }
   void stop() {
      time();
      finalize();
   }
   void inv() {
      invalidated = true;
   }
};
// ---------------------------------------------------------------------------------------------------
class PerformanceRecord {
   private:
   friend class PerformanceRecorderExecution;
   struct stats {
      /// Times of the individual repetitions
      std::vector<double> times;

      /// Minimum time
      double min = 0;
      /// Maximum time
      double max = 0;
      /// Median time
      double median = 0;
      /// Median absolute deviation of the time
      double medianAbosulteDeviation = 0;
      /// Mean time
      double mean = 0;
      /// Standard deviation of time
      double standardDeviation = 0;

      void calcStats();
      std::string genTimingString() const;
      inline double relMadTime() const { return median == 0 ? 0 : medianAbosulteDeviation / median; }
   };

   stats statsCompilation;
   stats statsExecution;

   std::vector<stats> intermediateStats;

   public:
   /// Add a compilation time
   void addCompilationTime(double compilationTime) { statsCompilation.times.push_back(compilationTime); }
   /// Add a execution time
   void addExecutionTime(double executionTime) { statsExecution.times.push_back(executionTime); }

   /// Return the minimum recorded query compilation time
   [[nodiscard]] double getMinimumCompilationTime() const { return TimeUnits::scaleTime(statsCompilation.min); }
   /// Return the maximum recorded query compilation time
   [[nodiscard]] double getMaximumCompilationTime() const { return TimeUnits::scaleTime(statsCompilation.max); }
   /// Return the median recorded query compilation time
   [[nodiscard]] double getMedianCompilationTime() const { return TimeUnits::scaleTime(statsCompilation.median); }
   /// Return the mean recorded query compilation time
   [[nodiscard]] double getMeanCompilationTime() const { return TimeUnits::scaleTime(statsCompilation.mean); }
   /// Return the relative median absolute deviation of the query compilation time
   [[nodiscard]] double getRelativeMadCompilationTime() const { return statsCompilation.relMadTime(); }
   /// Return the standard deviation of the query compilation times
   [[nodiscard]] double getCompilationTimeStandardDeviation() const { return statsCompilation.standardDeviation; }
   /// Return the individual recorded query compilation times
   [[nodiscard]] const std::vector<double>& getCompilationTimes() const { return statsCompilation.times; }
   /// Return the minimum recorded query execution time
   [[nodiscard]] double getMinimumExecutionTime() const { return TimeUnits::scaleTime(statsExecution.min); }
   /// Return the maximum recorded query execution time
   [[nodiscard]] double getMaximumExecutionTime() const { return TimeUnits::scaleTime(statsExecution.max); }
   /// Return the median recorded query execution time
   [[nodiscard]] double getMedianExecutionTime() const { return TimeUnits::scaleTime(statsExecution.median); }
   /// Return the mean recorded query execution time
   [[nodiscard]] double getMeanExecutionTime() const { return TimeUnits::scaleTime(statsExecution.mean); }
   /// Return the relative median absolute deviation of the query execution time
   [[nodiscard]] double getRelativeMadExecutionTime() const { return statsExecution.relMadTime(); }
   /// Return the standard deviation of the query execution times
   [[nodiscard]] double getExecutionTimeStandardDeviation() const { return statsExecution.standardDeviation; }
   /// Return the individual recorded query execution times
   [[nodiscard]] const std::vector<double>& getExecutionTimes() const { return statsExecution.times; }

   /// Start recording of execution time. Has to be started & stopped manually.
   PerformanceRecorderExecution startExecutionRecorder() { return PerformanceRecorderExecution(*this); }

   /// Print all collected values
   void printTimes(std::ostream& stream);
   /// Print the statistics
   void printStatistics(std::ostream& stream);

   private:
   /// Compute the statistics
   void computeStatistics();
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::queryc
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_QUERYC_PERFORMANCERECORD_HPP
