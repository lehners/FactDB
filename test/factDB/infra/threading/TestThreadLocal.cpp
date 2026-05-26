#include "factDB/infra/threading/ThreadLocal.hpp"
#include "gtest/gtest.h"
#include <thread>
#include <unordered_set>
//---------------------------------------------------------------------------
// Umbra
// (c) 2021 Michael Freitag
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace factDB::test {
//---------------------------------------------------------------------------
TEST(factDB_infra_threading, OwningThreadLocal_SingleInstance) {
   // SECTION("single instance") {
   // SECTION("uninitialized in main thread")
   {
      OwningThreadLocal<int> threadLocal;
      ASSERT_FALSE(threadLocal.hasStateInCurrentThread());

      {
         bool hit = false;
         threadLocal.doForEachThreadState([&](auto& /*state*/) {
            hit = true;
            return true;
         });
         ASSERT_FALSE(hit);
      }
      {
         bool hit = false;
         static_cast<const OwningThreadLocal<int>&>(threadLocal).doForEachThreadState([&](auto& /*state*/) {
            hit = true;
            return true;
         });
         ASSERT_FALSE(hit);
      }
   }

   //SECTION("initialized in main thread")
   {
      OwningThreadLocal<int> threadLocal;
      *threadLocal = 42;

      ASSERT_TRUE(threadLocal.hasStateInCurrentThread());
      ASSERT_EQ(*threadLocal, 42);

      {
         bool hit = false;
         threadLocal.doForEachThreadState([&](auto& /*state*/) {
            hit = true;
            return true;
         });
         ASSERT_TRUE(hit);
      }
      {
         bool hit = false;
         static_cast<const OwningThreadLocal<int>&>(threadLocal).doForEachThreadState([&](const auto& /*state*/) {
            hit = true;
            return true;
         });
         ASSERT_TRUE(hit);
      }

      threadLocal.doForEachThreadState([&](auto& state) {
         state = 21;
         return false;
      });
      ASSERT_EQ(*threadLocal, 21);

      int value = -1;
      static_cast<const OwningThreadLocal<int>&>(threadLocal).doForEachThreadState([&](const auto& state) {
         value = state;
         return false;
      });
      ASSERT_EQ(value, 21);
   }
   // SECTION("initialized in other threads")
   {
      OwningThreadLocal<int> threadLocal;
      vector<std::thread> threads;
      for (unsigned i = 0; i < 4; ++i) {
         threads.emplace_back([&, i]() {
            LocalState ls;
            *threadLocal = i;
         });
      }

      for (auto& thread : threads)
         thread.join();

      ASSERT_FALSE(threadLocal.hasStateInCurrentThread());

      {
         std::unordered_set<int> values;
         threadLocal.doForEachThreadState([&](const auto& state) {
            values.insert(state);
            return true;
         });

         ASSERT_EQ(values.size(), 4);
         ASSERT_TRUE(values.count(0));
         ASSERT_TRUE(values.count(1));
         ASSERT_TRUE(values.count(2));
         ASSERT_TRUE(values.count(3));
      }

      threadLocal.eraseOrphans([](const auto& state) {
         return (state % 2) == 0;
      });

      {
         std::unordered_set<int> values;
         threadLocal.doForEachThreadState([&](const auto& state) {
            values.insert(state);
            return true;
         });

         ASSERT_EQ(values.size(), 2);
         ASSERT_TRUE(values.count(1));
         ASSERT_TRUE(values.count(3));
      }

      threadLocal.eraseOrphans();

      {
         std::unordered_set<int> values;
         threadLocal.doForEachThreadState([&](const auto& state) {
            values.insert(state);
            return true;
         });

         ASSERT_TRUE(values.empty());
      }
   }
}
//---------------------------------------------------------------------------
#if 0
TEST(factDB_infra_threading, OwningThreadLocal_ManyInstances) {
   vector<OwningThreadLocal<int>> instances(64);
   for (unsigned i = 0; i < 8; ++i) {
      vector<std::thread> threads;
      for (unsigned j = 0; j < 8; ++j) {
         threads.emplace_back([&, i, j]() {
            *instances[8 * i + j] = 8 * i + j;

            REQUIRE_THREAD_SAFE(instances[8 * i + j].hasStateInCurrentThread());
            REQUIRE_THREAD_SAFE(*instances[8 * i + j] == static_cast<int>(8 * i + j));
         });
      }

      for (auto& thread : threads)
         thread.join();
   }
}
#endif
//---------------------------------------------------------------------------
} // namespace factDB::test
//---------------------------------------------------------------------------
