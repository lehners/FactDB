#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/PerformanceRecord.hpp"
#include <iostream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::util {
// ---------------------------------------------------------------------------------------------------
class BenchmarkTimer {
   std::string prefix;
   std::vector<std::chrono::steady_clock::time_point> times;
   static constexpr size_t maxPoints = 100;

   public:
   BenchmarkTimer(std::string prefix_ = "") : prefix(std::move(prefix_)) {
      times.reserve(maxPoints);
   }

   ~BenchmarkTimer() {
      printStatistics();
   }

   void time() { times.emplace_back(std::chrono::steady_clock::now()); }

   void printStatistics(std::ostream& outStream = std::cout) const {
      for (size_t i = 1; i != times.size(); ++i) {
         outStream << prefix << i << ": " << std::chrono::duration<double>(times[i] - times[i - 1]).count() * 1000 << " ms" << std::endl;
      }
   }
};
// ---------------------------------------------------------------------------------------------------
class BenchmarkLoopTimer {
   std::string prefix;
   std::vector<double> totalTimes;
   size_t iterations = 0;
   std::vector<std::chrono::steady_clock::time_point> times;
   static constexpr size_t maxPoints = 100;

   public:
   BenchmarkLoopTimer(std::string prefix_ = "") : prefix(std::move(prefix_)) {
      times.reserve(maxPoints);
      totalTimes.reserve(maxPoints);
   }

   ~BenchmarkLoopTimer() {
      printStatistics();
   }

   void time() { times.emplace_back(std::chrono::steady_clock::now()); }

   void finalizeLoop(bool incIter) {
      if (totalTimes.size() < times.size()) totalTimes.resize(times.size() - 1);
      iterations += incIter;
      for (size_t i = 1; i != times.size(); ++i) {
         totalTimes[i - 1] += std::chrono::duration<double>(times[i] - times[i - 1]).count() * 1000;
      }
      times.clear();
   }

   void printStatistics(std::ostream& outStream = std::cout) const {
      for (size_t i = 0; i != totalTimes.size(); ++i) {
         outStream << prefix << i + 1 << ": " << totalTimes[i] << " ms" << std::endl;
      }
      outStream << prefix << "loop iteartions: " << iterations << std::endl;
   }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::util
// ---------------------------------------------------------------------------------------------------