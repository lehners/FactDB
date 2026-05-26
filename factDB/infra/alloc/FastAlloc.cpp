// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/alloc/FastAlloc.hpp"
#include "factDB/infra/util/AddressSanitizer.hpp"
#include <atomic>
#include <cassert>
#include <cstdlib>
#include <stdlib.h>
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
size_t currentBlockShift = 30;
// ---------------------------------------------------------------------------------------------------
FastAlloc::FastAlloc() noexcept {
   start = memory = std::aligned_alloc(8, 1ull << currentBlockShift);
   CompilerToolchain::poisonMemoryRegion(start, (1ull << currentBlockShift));
}
// ---------------------------------------------------------------------------------------------------
#if 0
FastAlloc::FastAlloc(FastAlloc&& other) noexcept : memory(other.memory), start(other.start) {
   other.start = nullptr;
   other.memory = nullptr;
}
// ---------------------------------------------------------------------------------------------------
FastAlloc& FastAlloc::operator=(FastAlloc&& o) noexcept {
   memory = o.memory;
   start = o.start;
   o.start = nullptr;
   o.memory = nullptr;
   return *this;
}
#endif
// ---------------------------------------------------------------------------------------------------
void FastAlloc::clearAllocators() noexcept {
   fastAlloc.releaseMemory();

   fastAlloc.start = fastAlloc.memory = std::aligned_alloc(8, 1ull << currentBlockShift);
   CompilerToolchain::poisonMemoryRegion(fastAlloc.start, (1ull << currentBlockShift));
}
// ---------------------------------------------------------------------------------------------------
void FastAlloc::releaseMemory() noexcept {
   free(start);
   memory = nullptr;
}
// ---------------------------------------------------------------------------------------------------
std::mutex someMutex;
// ---------------------------------------------------------------------------------------------------
void* FastAlloc::allocate(size_t alignment, size_t size) noexcept {
   std::atomic_ref<void*> atomicMem(memory);

   auto curMem = atomicMem.load();

   while (true) {
      assert(curMem != nullptr);
      auto ptr = (reinterpret_cast<uintptr_t>(curMem) & ~(alignment - 1)) + alignment;
      std::lock_guard lc(someMutex);
      auto end_ptr = reinterpret_cast<void*>(ptr + size);
      assert(reinterpret_cast<uintptr_t>(end_ptr) <= (reinterpret_cast<uintptr_t>(start) + (1ull << currentBlockShift)));
      bool success = atomicMem.compare_exchange_weak(curMem, end_ptr, std::memory_order_release);
      if (success) {
         assert((ptr & (alignment - 1)) == 0);
         auto ptr_type = reinterpret_cast<void*>(ptr);
         CompilerToolchain::unpoisonMemoryRegion(ptr_type, size);

         return ptr_type;
      }
   }
}
// ---------------------------------------------------------------------------------------------------
namespace factDB {
FastAlloc fastAlloc;
}; // namespace factDB
// ---------------------------------------------------------------------------------------------------
