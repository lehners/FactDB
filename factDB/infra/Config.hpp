#ifndef H_FACTDB_INFRA_CONFIG_HPP
#define H_FACTDB_INFRA_CONFIG_HPP
// ---------------------------------------------------------------------------------------------------
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <version> // TODO remove once libc++ ranges workaround is not needed
// ---------------------------------------------------------------------------------------------------
// Umbra
// (c) 2016 Thomas Neumann
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
#ifdef __clang__
#define CLANG_REINITIALIZES [[clang::reinitializes]]
#define CLANG_NO_SANITIZE_FUNCTION [[clang::no_sanitize("function")]]
#else
#define CLANG_REINITIALIZES
#define CLANG_NO_SANITIZE_FUNCTION
#endif
// ---------------------------------------------------------------------------------------------------
#ifndef SCALABLE_ALLOC
#define SCALABLE_ALLOC 1
#endif // SCALABLE_ALLOC
// ---------------------------------------------------------------------------------------------------
constexpr const bool debugMode =
#ifdef NDEBUG
   false
#else
   true
#endif
   ;
// ---------------------------------------------------------------------------------------------------
/// Unreachable code
[[noreturn]] inline void unreachable() noexcept {
   assert(false && "unreachable");
   __builtin_unreachable();
}
// ---------------------------------------------------------------------------------------------------
/// Not implemented code
[[noreturn]] inline void not_implemented() noexcept {
   assert(false && "not implemented yet");
   __builtin_unreachable();
}
// ---------------------------------------------------------------------------------------------------
/// Assert an axiom. Hints the compiler to the truth of an expression.
inline constexpr void assertAxiom(bool truth) noexcept {
   if (!truth) {
      assert(truth);
      __builtin_unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
/// A 128bit data Type
struct Data128T {
   uint64_t values[2]; // NOLINT
};
// ---------------------------------------------------------------------------------------------------
/// A 128bit signed integer Type
using Int128T = __int128;
// ---------------------------------------------------------------------------------------------------
/// A 128bit unsigned integer Type
using uint128T = unsigned __int128;
// ---------------------------------------------------------------------------------------------------
namespace ByteOrder {
/// Are we using a big endian machine?
constexpr const bool bigEndian =
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
   true
#else
   false
#endif
   ;
} // namespace ByteOrder
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif
