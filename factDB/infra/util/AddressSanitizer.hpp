#ifndef H_infra_util_AddressSanitizer
#define H_infra_util_AddressSanitizer
// ---------------------------------------------------------------------------------------------------
#include <cstddef>
#include <mutex>
// ---------------------------------------------------------------------------------------------------
// Adapted from Umbra
// (c) 2022 Altan Birler
// ---------------------------------------------------------------------------------------------------
#if defined(__has_feature)
#if __has_feature(address_sanitizer)
#define ADDRESS_SANITIZER_ACTIVE
#endif
#elif defined(__SANITIZE_ADDRESS__)
#define ADDRESS_SANITIZER_ACTIVE
#endif
// ---------------------------------------------------------------------------------------------------
#ifdef ADDRESS_SANITIZER_ACTIVE
extern "C" void __asan_poison_memory_region(void const volatile* p, size_t size); // NOLINT(bugprone-reserved-identifier)
extern "C" void __asan_unpoison_memory_region(void const volatile* p, size_t size); // NOLINT(bugprone-reserved-identifier)
#endif
// ---------------------------------------------------------------------------------------------------
namespace factDB::CompilerToolchain {
/// Is the address sanitizer compiled into this binary?
constexpr const bool addressSanitizerActive =
#ifdef ADDRESS_SANITIZER_ACTIVE
   true
#else
   false
#endif
   ;

#ifdef ADDRESS_SANITIZER_ACTIVE
static std::mutex poisonMutex;
// ---------------------------------------------------------------------------------------------------
[[gnu::always_inline]] static inline void poisonMemoryRegion(void const volatile* p, size_t size)
// Poison a region of memory
{
   std::unique_lock locked(poisonMutex);
   __asan_poison_memory_region(p, size);
}
// ---------------------------------------------------------------------------------------------------
[[gnu::always_inline]] static inline void unpoisonMemoryRegion(void const volatile* p, size_t size)
// Unpoison a region of memory
{
   std::unique_lock locked(poisonMutex);
   __asan_unpoison_memory_region(p, size);
}
#else
[[gnu::always_inline]] static inline void poisonMemoryRegion(void const volatile* /*p*/, size_t /*size*/)
// Poison a region of memory
{
}
// ---------------------------------------------------------------------------------------------------
[[gnu::always_inline]] static inline void unpoisonMemoryRegion(void const volatile* /*p*/, size_t /*size*/)
// Unpoison a region of memory
{
}
#endif
} // namespace factDB::CompilerToolchain
// ---------------------------------------------------------------------------------------------------
#endif
