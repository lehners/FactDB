#include "factDB/infra/alloc/BumpAllocator.hpp"
#include "gtest/gtest.h"
#include "tbb/tbb.h"
#include <cstring>
// ---------------------------------------------------------------------------------------------------
// Adapted from Umbra
// (c) 2022 Altan Birler
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
template <typename T>
void typedTest(size_t count) {
   BumpAllocator allocator;
   std::vector<T*> values;

   for (size_t i = 0; i < count; i++) {
      BumpAllocator curAlloc(std::move(allocator));
      auto ptr = reinterpret_cast<T*>(curAlloc.allocate(alignof(T), sizeof(T)));
      *ptr = static_cast<T>(i);
      values.push_back(ptr);
      allocator = std::move(curAlloc);
   }

   for (size_t i = 0; i < count; i++) {
      ASSERT_EQ(*values[i], static_cast<T>(i));
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------------------------------
TEST(BumpAllocator, Basic) {
   BumpAllocator ba;
   auto* ptr = ba.allocate(8, 8);
   ASSERT_NE(ptr, nullptr);
   ASSERT_NO_THROW(ba.deallocate(ptr));
}
// ---------------------------------------------------------------------------------------------------
TEST(BumpAllocator, Byte1) {
   typedTest<uint8_t>(400);
}
// ---------------------------------------------------------------------------------------------------
TEST(BumpAllocator, Byte8) {
   typedTest<uint64_t>(50);
}
// ---------------------------------------------------------------------------------------------------
TEST(BumpAllocator, Byte1Many) {
   typedTest<uint8_t>(2048);
}
// ---------------------------------------------------------------------------------------------------
TEST(BumpAllocator, Byte8Many) {
   typedTest<uint64_t>(2048);
}
// ---------------------------------------------------------------------------------------------------
TEST(BumpAllocator, Huge) {
   BumpAllocator allocator;

   char cmp1[4096];
   auto ptr1 = allocator.allocate(32, 4096);
   ASSERT_EQ((reinterpret_cast<uintptr_t>(ptr1) & 31), 0);

   char cmp2[2049];
   auto ptr2 = allocator.allocate(16, 2049);
   ASSERT_EQ((reinterpret_cast<uintptr_t>(ptr2) & 15), 0);

   char cmp3[12345];
   auto ptr3 = allocator.allocate(64, 12345);
   ASSERT_EQ((reinterpret_cast<uintptr_t>(ptr3) & 63), 0);

   memset(ptr1, 1, 4096);
   memset(cmp1, 1, 4096);
   memset(ptr2, 2, 2049);
   memset(cmp2, 2, 2049);
   memset(ptr3, 3, 12345);
   memset(cmp3, 3, 12345);

   ASSERT_EQ(memcmp(cmp1, ptr1, 4096), 0);
   ASSERT_EQ(memcmp(cmp2, ptr2, 2049), 0);
   ASSERT_EQ(memcmp(cmp3, ptr3, 12345), 0);
}
// ---------------------------------------------------------------------------------------------------
TEST(BumpAllocator, CornerCase) {
   BumpAllocator allocator;
   allocator.allocate(1, 1016);
}
// ---------------------------------------------------------------------------------------------------
TEST(BumpAllocator, Parallel) {
   oneapi::tbb::task_scheduler_handle handle; // CodegenExpanded.cpp(143:70)
   handle = oneapi::tbb::task_scheduler_handle{oneapi::tbb::attach{}}; // CodegenExpanded.cpp(144:95)

   size_t countPerThread = 100;
   std::vector<std::vector<int*>> allocations;
   allocations.resize(100);

   tbb::parallel_for(0, 100, [&allocations, countPerThread](auto threadVal) {
      BumpAllocatorInterface<int> allocator;
      for (size_t i = 0; i < countPerThread; i++) {
         auto ptr = allocator.allocate(1);
         *ptr = threadVal + i;
         allocations[threadVal].emplace_back(ptr);
      }
   });

   for (size_t i = 0; i != allocations.size(); ++i) {
      ASSERT_EQ(allocations[i].size(), countPerThread);
      for (size_t j = 0; j != allocations[i].size(); ++j) {
         ASSERT_EQ(*allocations[i][j], i + j);
      }
   }
   oneapi::tbb::finalize(handle); // CodegenExpanded.cpp(152:61)
}
// ---------------------------------------------------------------------------------------------------
