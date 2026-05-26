#pragma once

#include <deque>

namespace factDB::infra::list {

// ---------------------------------------------------------------------------------------------------
struct STLDequeStats {
   public:
   inline static std::deque<size_t> sizesOwningStlDeque;
   inline static std::deque<size_t> sizesOwningStlDequeLocking;
   inline static std::deque<size_t> sizesNonOwningStlDeque;
   inline static std::deque<size_t> ptrsNonOwningStlDeque;

   static void printStats() {
      printStatsNormal();
      printStatsLocking();
      printStatsNonOwning();
   }

   static void calcStats(auto& name, auto& list) {
      size_t total = 0, tmin = -1, tmax = 0;
      for (auto s : list) {
         total += s;
         tmin = std::min(tmin, s);
         tmax = std::max(tmax, s);
      }
      auto avg = list.empty() ? 0 : (total / list.size());
      std::cout << name << ":"
                << "  total: " << total
                << "  count: " << list.size()
                << "  avg:   " << avg
                << "  min:   " << tmin
                << "  max:   " << tmax << std::endl;
      list.clear();
   }

   static void printStatsNormal() {
      calcStats("Stats Owning", sizesOwningStlDeque);
   }
   static void printStatsLocking() {
      calcStats("Stats Owning Lockable", sizesOwningStlDequeLocking);
   }
   static void printStatsNonOwning() {
      calcStats("Stats Non Owning Values", sizesNonOwningStlDeque);
      calcStats("Stats Non Owning Pointers", ptrsNonOwningStlDeque);
   }
};
} // namespace factDB::infra::list
