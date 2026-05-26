#include "factDB/infra/alloc/BumpAllocator.hpp"
#include "factDB/infra/Unaligned.hpp"
#include "factDB/infra/util/AddressSanitizer.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <utility>
#include <sys/mman.h>
// ---------------------------------------------------------------------------------------------------
// Adapted from Umbra
// (c) 2022 Altan Birler
// ---------------------------------------------------------------------------------------------------
using namespace std;
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
BumpAllocator allocator;
// ---------------------------------------------------------------------------------------------------
static std::byte* getAlloc(byte* cur, size_t alignment, size_t size) noexcept {
   // Get the prospective position of the allocated region
   auto c = reinterpret_cast<uintptr_t>(cur);
   return reinterpret_cast<byte*>((size > c ? 0 : c - size) & ~(alignment - 1));
}
// ---------------------------------------------------------------------------------------------------
void BumpAllocator::clearAllocators() noexcept {
   allocator.releaseAllMemory();
}
// ---------------------------------------------------------------------------------------------------
void BumpAllocator::releaseAllMemory() noexcept {
   // Release all allocated memory
   auto* curFooter = currentFooter;
   while (curFooter) {
      CompilerToolchain::unpoisonMemoryRegion(curFooter, footerSize);
      /// Check guard
      assert((!CompilerToolchain::addressSanitizerActive) || ((unalignedLoad<uint64_t>(curFooter + guardOffset) & guardMask) == guard));
      auto prevFooter = unalignedLoad<byte*>(curFooter + prevPtrOffset);

      auto blockSize = (1ull << unalignedLoad<uint8_t>(curFooter + sizeShiftOffset));
      auto block = curFooter + footerSize - blockSize;
      CompilerToolchain::unpoisonMemoryRegion(block, blockSize - footerSize);
      free(block);
      curFooter = prevFooter;
   }
   endByte = nullptr;
   currentByte = nullptr;
   currentFooter = nullptr;
   currentBlockShift = initialBlockSizeShift;
}
// ---------------------------------------------------------------------------------------------------
BumpAllocator::BumpAllocator() noexcept {
   std::unique_lock locked(allocatorCollectorMutex);
   allocatorList.emplace_back(this);
}
// ---------------------------------------------------------------------------------------------------
BumpAllocator::BumpAllocator(BumpAllocator&& o) noexcept {
   *this = std::move(o);
}
// ---------------------------------------------------------------------------------------------------
BumpAllocator& BumpAllocator::operator=(BumpAllocator&& o) noexcept
// Move assignment
{
   releaseAllMemory();
   endByte = o.endByte;
   currentByte = o.currentByte;
   currentFooter = o.currentFooter;
   currentBlockShift = o.currentBlockShift;
   o.endByte = nullptr;
   o.currentByte = nullptr;
   o.currentFooter = nullptr;
   o.currentBlockShift = initialBlockSizeShift;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
void* BumpAllocator::allocate(size_t alignment, size_t size) noexcept
// Allocate aligned memory
{
   while (true) {
      std::atomic_ref curByteAtomic(currentByte);
      std::atomic_ref endByteAtomic(endByte);
      auto curByteRead = curByteAtomic.load();
      std::byte* curEndByte;
      while (true) {
         curEndByte = endByteAtomic.load();
         auto ptr = getAlloc(curByteRead, alignment, size);
         if (!curEndByte || (ptr < curEndByte)) [[unlikely]]
            break;
         bool success = curByteAtomic.compare_exchange_weak(curByteRead, ptr);
         if (success) {
            assert((reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0);
            // Unpoison allocated region
            CompilerToolchain::unpoisonMemoryRegion(ptr, size);
            return ptr;
         }
      }

      std::lock_guard locked(slowPathMutex);
      // acquire some lock... sleep...
      // check if still need to alloc memory. Has the curEndByte changed in the mean time? if yes, we can retry the fast path, else need to do slow path
      if (curEndByte != endByteAtomic.load()) { // somebody already allocated memory => try fast path again
         continue;
      }

      // Slow path. We need a new block
      // Find a size that is large enough
      currentBlockShift++;
      for (; (1ull << currentBlockShift) < size + footerSize; currentBlockShift++) {}

      // The minimum alignment of 8 acts both as a heuristic for better alignment for future allocations and
      // helps with more accurate ASAN poisoning of regions
      auto* newBlock = reinterpret_cast<byte*>(aligned_alloc(max<size_t>(alignment, 8), 1ull << currentBlockShift));
#ifdef __linux__
      madvise(newBlock, 1ull << currentBlockShift, MADV_HUGEPAGE);
#endif
      auto currentByteUpdated = newBlock + (1ull << currentBlockShift) - footerSize;

      unalignedStore<uint64_t>(currentByteUpdated + guardOffset, guard);
      unalignedStore<uint8_t>(currentByteUpdated + sizeShiftOffset, currentBlockShift);
      unalignedStore<byte*>(currentByteUpdated + prevPtrOffset, currentFooter);
      currentFooter = currentByteUpdated;
      assert((unalignedLoad<uint64_t>(currentFooter + guardOffset) & guardMask) == guard);
      // Poison entire block. As the user allocates, those regions will be unpoisoned
      CompilerToolchain::poisonMemoryRegion(newBlock, (1ull << currentBlockShift));
      curByteAtomic.store(currentByteUpdated);
      endByteAtomic.store(newBlock);

      auto ptr = getAlloc(currentByteUpdated, alignment, size);
      assert(ptr >= endByteAtomic.load());
      bool success = curByteAtomic.compare_exchange_weak(currentByteUpdated, ptr);
      if (success) {
         assert((reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0);
         // Unpoison allocated region
         CompilerToolchain::unpoisonMemoryRegion(ptr, size);
         assert(ptr + size <= currentFooter);
         return ptr;
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
