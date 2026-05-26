#pragma once
// ---------------------------------------------------------------------------------------------------
#include "tbb/task_scheduler_observer.h"
#include <iostream>
// ---------------------------------------------------------------------------------------------------
class TBBThreadCounter : public tbb::task_scheduler_observer {
   private:
   std::atomic<std::size_t> thread_count; // Thread-safe counter
   std::atomic<std::size_t> total_count; // Thread-safe counter
   std::atomic<std::size_t> max_parallel; // Thread-safe counter

   public:
   TBBThreadCounter() : tbb::task_scheduler_observer(), thread_count(0), total_count(0), max_parallel(0) {
      observe(true); // Start observing
   }

   TBBThreadCounter(tbb::task_arena& arena) : tbb::task_scheduler_observer(arena), thread_count(0), total_count(0), max_parallel(0) {
      observe(true); // Start observing
   }

   ~TBBThreadCounter() {
      observe(false); // Stop observing
   }

   void on_scheduler_entry(bool) override {
      ++total_count;
      auto curCount = ++thread_count;
      auto curMax = max_parallel.load();

      while (curMax < curCount && !max_parallel.compare_exchange_weak(curMax, curCount))
         ;
   }

   void on_scheduler_exit(bool) override {
      --thread_count;
   }

   std::size_t get_thread_count() const { return thread_count; }
   std::size_t get_total_thread_count() const { return total_count; }
   std::size_t get_max_thread_count() const { return max_parallel; }

   void printStats() {
      std::cout << "total threads: " << get_thread_count() << ", total thread count: " << get_total_thread_count() << ", max running in parallel: " << get_max_thread_count() << std::endl;
   }
};