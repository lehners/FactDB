#ifndef H_infra_BumpAllocator
#define H_infra_BumpAllocator
// ---------------------------------------------------------------------------------------------------
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <vector>
// ---------------------------------------------------------------------------------------------------
// Adapted from Umbra
// (c) 2022 Altan Birler
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class BumpAllocator;
// ---------------------------------------------------------------------------------------------------
static std::vector<BumpAllocator*> allocatorList = {};
static std::mutex allocatorCollectorMutex;
// ---------------------------------------------------------------------------------------------------
/// A bump allocator. Really fast allocations and no deallocations. Does not support concurrency
class BumpAllocator {
   /// Size shift of the initial inline block
   static constexpr size_t initialBlockSizeShift = 30;
   /// A block's structure:
   ///
   /// end          <- current        (   ---    footer      ---    )
   ///  | unused data ... | used data | guard | sizeShift | prevPtr |
   ///
   /// guard contains 7 bytes of fixed data to check against overflows
   /// A block's size is (1ull << sizeShift). sizeShift is 1 byte in size.
   /// A prevPtr points to the footer of the previous block

   /// Guard value
   static constexpr size_t guard = 0x00'deadbeef3EBDD9ull;
   /// Mask for checking guard
   static constexpr size_t guardMask = 0x00'FFFFFFFFFFFFFFull;
   /// Footer size
   static constexpr size_t footerSize = sizeof(guard) + sizeof(void*);
   /// Offset of guard within footer
   static constexpr size_t guardOffset = 0;
   /// Offset of sizeShift within footer
   static constexpr size_t sizeShiftOffset = 7;
   /// Offset of prevPtr within footer
   static constexpr size_t prevPtrOffset = 8;

   /// The end of the current block
   std::byte* endByte = nullptr;
   /// The current byte of the current block (the byte that was last allocated)
   std::byte* currentByte = nullptr;
   /// The current block's footer
   std::byte* currentFooter = nullptr;
   /// The allocated size of the current block
   uint8_t currentBlockShift = initialBlockSizeShift;

   std::mutex slowPathMutex;

   public:
   /// Constructor
   BumpAllocator() noexcept;
   /// Move constructor
   BumpAllocator(BumpAllocator&& o) noexcept;
   /// Destructor
   ~BumpAllocator() noexcept { releaseAllMemory(); }

   /// Move assignment
   BumpAllocator& operator=(BumpAllocator&& o) noexcept;

   /// Release all allocated memory
   void releaseAllMemory() noexcept;

   /// Allocate aligned memory
   [[gnu::malloc]] void* allocate(size_t alignment, size_t size) noexcept;
   /// Deallocate memory. Noop
   void deallocate(void*) noexcept {}

   static void clearAllocators() noexcept;
};
// ---------------------------------------------------------------------------------------------------
extern BumpAllocator allocator;
// ---------------------------------------------------------------------------------------------------
template <typename T>
class BumpAllocatorInterface {
   public:
   using value_type = T;
   using pointer = T*;
   using const_pointer = const T*;
   using size_type = std::size_t;
   using difference_type = std::ptrdiff_t;

   BumpAllocatorInterface() noexcept = default;
   template <typename U>
   BumpAllocatorInterface(const BumpAllocatorInterface<U>&) noexcept {}

   T* allocate(size_t n) {
      T* ptr = nullptr;
      ptr = reinterpret_cast<T*>(allocator.allocate(alignof(T), sizeof(T) * n));
#if WRITE_FIRST
      assert(sizeof(int) < size);
      *reinterpret_cast<int*>(ptr) = 0;
#endif
      return ptr;
   }
   void deallocate(void*, size_t) noexcept {}

   // Comparisons for allocator equality (required)
   bool operator==(const BumpAllocatorInterface&) const noexcept { return true; }
   bool operator!=(const BumpAllocatorInterface&) const noexcept { return false; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_infra_BumpAllocator
