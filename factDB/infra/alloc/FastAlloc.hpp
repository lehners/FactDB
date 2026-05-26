#pragma once
// ---------------------------------------------------------------------------------------------------
#include <cstddef>
#include <type_traits>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class FastAlloc {
   void* memory;
   void* start;

   public:
   FastAlloc() noexcept;
   ~FastAlloc() noexcept { releaseMemory(); }

   void releaseMemory() noexcept;
   static void clearAllocators() noexcept;

   [[gnu::malloc]] void* allocate(size_t alignment, size_t size) noexcept;
   void deallocate(void*) noexcept {}
};
// ---------------------------------------------------------------------------------------------------
extern FastAlloc fastAlloc;
// ---------------------------------------------------------------------------------------------------
template <typename T>
class FastAllocInterface {
   public:
   using value_type = T;
   using pointer = value_type*;
   using const_pointer = const value_type*;
   using reference = value_type&;
   using const_reference = const value_type&;
   using difference_type = std::ptrdiff_t;
   using size_type = std::size_t;

   using is_always_equal = std::true_type;

   template <typename U>
   struct rebind {
      using other = FastAllocInterface<U>;
   };
   pointer address(reference x) const { return &x; }
   const_pointer address(const_reference x) const { return &x; }

   FastAllocInterface() noexcept = default;

   T* allocate(size_t n) {
      return reinterpret_cast<T*>(fastAlloc.allocate(alignof(T), sizeof(T) * n));
   }
   void deallocate(void*, size_t) noexcept {}
};
// ---------------------------------------------------------------------------------------------------
}; // namespace factDB
// ---------------------------------------------------------------------------------------------------
