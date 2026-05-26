#ifndef H_FACTDB_INFRA_UNALIGNED_HPP
#define H_FACTDB_INFRA_UNALIGNED_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include <cstdint>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
/// Wrapper for Unaligned data types
template <class T>
struct [[gnu::packed]] Unaligned {
   T value;
   /// Load the value
   [[nodiscard]] constexpr T get() const noexcept { return value; }
   /// Implicit conversion to the value
   [[nodiscard]] constexpr operator T() const noexcept { return value; }
   /// Implicit converting constructor
   [[nodiscard]] constexpr Unaligned(T valueParam) noexcept : value(valueParam) {}
   /// Get the potentially Unaligned address
   [[nodiscard]] void* getPtr() noexcept { return this; }
   /// Get the potentially Unaligned address
   [[nodiscard]] const void* getPtr() const noexcept { return this; }
};
static_assert(alignof(Unaligned<void*>) == 1, "Unaligned should be packed");
// ---------------------------------------------------------------------------------------------------
/// Wrapper for Unaligned arrays
template <class T, std::size_t N>
struct [[gnu::packed]] UnalignedArray {
   T value[N]; // NOLINT
   /// Array operator
   Unaligned<T>& operator[](std::size_t pos) noexcept {
      assertAxiom(pos < N);
      return *reinterpret_cast<Unaligned<T>*>(&value[pos]);
   }
   /// Array operator
   const Unaligned<T>& operator[](std::size_t pos) const noexcept {
      assertAxiom(pos < N);
      return *reinterpret_cast<const Unaligned<T>*>(&value[pos]);
   }
   /// Implicit pointer decay of the array
   operator const Unaligned<T>*() const noexcept { return reinterpret_cast<const Unaligned<T>*>(value); }
   /// Iterator
   const Unaligned<T>* begin() const noexcept { return reinterpret_cast<const Unaligned<T>*>(value); }
   /// Iterator
   const Unaligned<T>* end() const noexcept { return reinterpret_cast<const Unaligned<T>*>(value + N); }
   /// Implicit pointer decay of the array
   operator Unaligned<T>*() noexcept { return reinterpret_cast<Unaligned<T>*>(value); }
   /// Iterator
   Unaligned<T>* begin() noexcept { return reinterpret_cast<const Unaligned<T>*>(value); }
   /// Iterator
   Unaligned<T>* end() noexcept { return reinterpret_cast<const Unaligned<T>*>(value + N); }
};
// ---------------------------------------------------------------------------------------------------
/// Unaligned load
template <class T>
[[gnu::always_inline]] inline T unalignedLoad(const void* ptr) noexcept { return reinterpret_cast<const Unaligned<T>*>(ptr)->value; }
template <class T>
[[gnu::always_inline]] inline T unalignedArrayLoad(const void* ptr, intptr_t index) noexcept { return reinterpret_cast<const Unaligned<T>*>(ptr)[index].value; }
// ---------------------------------------------------------------------------------------------------
/// Store Unaligned
template <class T>
[[gnu::always_inline]] inline void unalignedStore(void* ptr, T value) noexcept { reinterpret_cast<Unaligned<T>*>(ptr)->value = value; }
template <class T>
[[gnu::always_inline]] inline void unalignedArrayStore(void* ptr, intptr_t index, T value) noexcept { reinterpret_cast<Unaligned<T>*>(ptr)[index].value = value; }
// ---------------------------------------------------------------------------------------------------
/// Adjust a pointer value by an absolute offset
template <class T>
[[gnu::always_inline]] inline void adjustPointer(T*& ptr, intptr_t delta) { ptr = reinterpret_cast<T*>(reinterpret_cast<char*>(ptr) + delta); }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_INFRA_UNALIGNED_HPP
