#ifndef H_infra_util_IntegerOperations
#define H_infra_util_IntegerOperations
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
// ---------------------------------------------------------------------------------------------------
// Umbra
// (c) 2016 Thomas Neumann
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
/// Builtin integer operations
template <class T>
struct IntegerOperations {};
/// Builtin integer operations
template <>
struct IntegerOperations<int8_t> {
   /// Count leading zeroes
   [[gnu::always_inline]] static constexpr unsigned clz(uint8_t a) noexcept {
      assert(a);
      return __builtin_clz(a) - (8 * (sizeof(unsigned) - sizeof(uint8_t)));
   }
   /// Count trailing zeroes
   [[gnu::always_inline]] static constexpr unsigned ctz(uint8_t a) noexcept {
      assert(a);
      return __builtin_ctz(a);
   }
   /// Count number of set bits
   [[gnu::always_inline]] static constexpr unsigned popcount(uint8_t a) noexcept {
      return __builtin_popcount(a);
   }
   /// Compute log2, rounding down. Undefined for a <= 0
   [[gnu::always_inline]] static constexpr unsigned log2(uint8_t a) noexcept {
      assert(a);
      return (8 * sizeof(unsigned) - 1) - __builtin_clz(a);
   }
   /// Compute log2, rounding up. Undefined for a <= 1
   [[gnu::always_inline]] static constexpr unsigned log2ceil(uint8_t a) noexcept {
      assert(a > 1);
      return (8 * sizeof(unsigned)) - __builtin_clz(a - 1);
   }
   /// Bswap
   [[gnu::always_inline]] static constexpr int8_t bswap(int8_t a) noexcept { return a; }
   /// As little endian
   [[gnu::always_inline]] static constexpr int8_t host2le(int8_t a) noexcept { return a; }
   /// As big endian
   [[gnu::always_inline]] static constexpr int8_t host2be(int8_t a) noexcept { return a; }
   /// From little endian
   [[gnu::always_inline]] static constexpr int8_t le2host(int8_t a) noexcept { return a; }
   /// From big endian
   [[gnu::always_inline]] static constexpr int8_t be2host(int8_t a) noexcept { return a; }
   /// Rotate left
   [[gnu::always_inline]] static constexpr int8_t rotl(uint8_t a, unsigned shift) noexcept { return (a << shift) | (a >> (8 - shift)); } // NOLINT
   /// Rotate right
   [[gnu::always_inline]] static constexpr int8_t rotr(uint8_t a, unsigned shift) noexcept { return (a >> shift) | (a << (8 - shift)); } // NOLINT
   /// The high word of a multiplication
   [[gnu::always_inline]] static constexpr int8_t mulHigh(int8_t a, int8_t b) noexcept {
      return (a * b) >> 8; // NOLINT
   }
};
/// Builtin integer operations
template <>
struct IntegerOperations<uint8_t> {
   /// Count leading zeroes
   [[gnu::always_inline]] static constexpr unsigned clz(uint8_t a) noexcept {
      assert(a);
      return __builtin_clz(a) - (8 * (sizeof(unsigned) - sizeof(uint8_t)));
   }
   /// Count trailing zeroes
   [[gnu::always_inline]] static constexpr unsigned ctz(uint8_t a) noexcept {
      assert(a);
      return __builtin_ctz(a);
   }
   /// Count number of set bits
   [[gnu::always_inline]] static constexpr unsigned popcount(uint8_t a) noexcept {
      return __builtin_popcount(a);
   }
   /// Compute log2, rounding down. Undefined for 0
   [[gnu::always_inline]] static constexpr unsigned log2(uint8_t a) noexcept {
      assert(a);
      return (8 * sizeof(unsigned) - 1) - __builtin_clz(a);
   }
   /// Compute log2, rounding up. Undefined for a <= 1
   [[gnu::always_inline]] static constexpr unsigned log2ceil(uint8_t a) noexcept {
      assert(a > 1);
      return (8 * sizeof(unsigned)) - __builtin_clz(a - 1);
   }
   /// Bswap
   [[gnu::always_inline]] static constexpr uint8_t bswap(uint8_t a) noexcept { return a; }
   /// As little endian
   [[gnu::always_inline]] static constexpr uint8_t host2le(uint8_t a) noexcept { return a; }
   /// As big endian
   [[gnu::always_inline]] static constexpr uint8_t host2be(uint8_t a) noexcept { return a; }
   /// From little endian
   [[gnu::always_inline]] static constexpr uint8_t le2host(uint8_t a) noexcept { return a; }
   /// From big endian
   [[gnu::always_inline]] static constexpr uint8_t be2host(uint8_t a) noexcept { return a; }
   /// Rotate left
   [[gnu::always_inline]] static constexpr uint8_t rotl(uint8_t a, unsigned shift) noexcept { return (a << shift) | (a >> (8 - shift)); }
   /// Rotate right
   [[gnu::always_inline]] static constexpr uint8_t rotr(uint8_t a, unsigned shift) noexcept { return (a >> shift) | (a << (8 - shift)); }
   /// The high word of a multiplication
   [[gnu::always_inline]] static constexpr uint8_t mulHigh(uint8_t a, uint8_t b) noexcept {
      return (a * b) >> 8;
   }
};
/// Builtin integer operations
template <>
struct IntegerOperations<int16_t> {
   /// Count leading zeroes
   [[gnu::always_inline]] static constexpr unsigned clz(uint16_t a) noexcept {
      assert(a);
      return __builtin_clz(a) - (8 * (sizeof(unsigned) - sizeof(uint16_t)));
   }
   /// Count trailing zeroes
   [[gnu::always_inline]] static constexpr unsigned ctz(uint16_t a) noexcept {
      assert(a);
      return __builtin_ctz(a);
   }
   /// Count number of set bits
   [[gnu::always_inline]] static constexpr unsigned popcount(uint16_t a) noexcept {
      return __builtin_popcount(a);
   }
   /// Compute log2, rounding down. Undefined for a <= 0
   [[gnu::always_inline]] static constexpr unsigned log2(uint16_t a) noexcept {
      assert(a);
      return (8 * sizeof(unsigned) - 1) - __builtin_clz(a);
   }
   /// Compute log2, rounding up. Undefined for a <= 1
   [[gnu::always_inline]] static constexpr unsigned log2ceil(uint16_t a) noexcept {
      assert(a > 1);
      return (8 * sizeof(unsigned)) - __builtin_clz(a - 1);
   }
   /// Bswap
   [[gnu::always_inline]] static constexpr int16_t bswap(int16_t a) noexcept { return __builtin_bswap16(a); } // NOLINT
   /// As little endian
   [[gnu::always_inline]] static constexpr int16_t host2le(int16_t a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// As big endian
   [[gnu::always_inline]] static constexpr int16_t host2be(int16_t a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// From little endian
   [[gnu::always_inline]] static constexpr int16_t le2host(int16_t a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// From big endian
   [[gnu::always_inline]] static constexpr int16_t be2host(int16_t a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// Rotate left
   [[gnu::always_inline]] static constexpr int16_t rotl(uint16_t a, unsigned shift) noexcept { return (a << shift) | (a >> (16 - shift)); } // NOLINT
   /// Rotate right
   [[gnu::always_inline]] static constexpr int16_t rotr(uint16_t a, unsigned shift) noexcept { return (a >> shift) | (a << (16 - shift)); } // NOLINT
   /// The high word of a multiplication
   [[gnu::always_inline]] static constexpr int16_t mulHigh(int16_t a, int16_t b) noexcept {
      return (a * b) >> 16; // NOLINT
   }
};
/// Builtin integer operations
template <>
struct IntegerOperations<uint16_t> {
   /// Count leading zeroes
   [[gnu::always_inline]] static constexpr unsigned clz(uint16_t a) noexcept {
      assert(a);
      return __builtin_clz(a) - (8 * (sizeof(unsigned) - sizeof(uint16_t)));
   }
   /// Count trailing zeroes
   [[gnu::always_inline]] static constexpr unsigned ctz(uint16_t a) noexcept {
      assert(a);
      return __builtin_ctz(a);
   }
   /// Count number of set bits
   [[gnu::always_inline]] static constexpr unsigned popcount(uint16_t a) noexcept {
      return __builtin_popcount(a);
   }
   /// Compute log2, rounding down. Undefined for 0
   [[gnu::always_inline]] static constexpr unsigned log2(uint16_t a) noexcept {
      assert(a);
      return (8 * sizeof(unsigned) - 1) - __builtin_clz(a);
   }
   /// Compute log2, rounding up. Undefined for a <= 1
   [[gnu::always_inline]] static constexpr unsigned log2ceil(uint16_t a) noexcept {
      assert(a > 1);
      return (8 * sizeof(unsigned)) - __builtin_clz(a - 1);
   }
   /// Bswap
   [[gnu::always_inline]] static constexpr uint16_t bswap(uint16_t a) noexcept { return __builtin_bswap16(a); }
   /// As little endian
   [[gnu::always_inline]] static constexpr uint16_t host2le(uint16_t a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// As big endian
   [[gnu::always_inline]] static constexpr uint16_t host2be(uint16_t a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// From little endian
   [[gnu::always_inline]] static constexpr uint16_t le2host(uint16_t a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// From big endian
   [[gnu::always_inline]] static constexpr uint16_t be2host(uint16_t a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// Rotate left
   [[gnu::always_inline]] static constexpr uint16_t rotl(uint16_t a, unsigned shift) noexcept { return (a << shift) | (a >> (16 - shift)); }
   /// Rotate right
   [[gnu::always_inline]] static constexpr uint16_t rotr(uint16_t a, unsigned shift) noexcept { return (a >> shift) | (a << (16 - shift)); }
   /// The high word of a multiplication
   [[gnu::always_inline]] static constexpr uint16_t mulHigh(uint16_t a, uint16_t b) noexcept {
      return (static_cast<uint32_t>(a) * static_cast<uint32_t>(b)) >> 16;
   }
};
/// Builtin integer operations
template <>
struct IntegerOperations<int> {
   /// Count leading zeroes
   [[gnu::always_inline]] static constexpr unsigned clz(unsigned a) noexcept {
      assert(a);
      return __builtin_clz(a);
   }
   /// Count trailing zeroes
   [[gnu::always_inline]] static constexpr unsigned ctz(unsigned a) noexcept {
      assert(a);
      return __builtin_ctz(a);
   }
   /// Count number of set bits
   [[gnu::always_inline]] static constexpr unsigned popcount(unsigned a) noexcept {
      return __builtin_popcount(a);
   }
   /// Compute log2, rounding down. Undefined for a <= 0
   [[gnu::always_inline]] static constexpr unsigned log2(unsigned a) noexcept {
      assert(a);
      return (8 * sizeof(unsigned) - 1) - __builtin_clz(a);
   }
   /// Compute log2, rounding up. Undefined for a <= 1
   [[gnu::always_inline]] static constexpr unsigned log2ceil(unsigned a) noexcept {
      assert(a > 1);
      return (8 * sizeof(unsigned)) - __builtin_clz(a - 1);
   }
   /// Bswap
   [[gnu::always_inline]] static constexpr int bswap(int a) noexcept { return __builtin_bswap32(a); } // NOLINT
   /// As little endian
   [[gnu::always_inline]] static constexpr int host2le(int a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// As big endian
   [[gnu::always_inline]] static constexpr int host2be(int a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// From little endian
   [[gnu::always_inline]] static constexpr int le2host(int a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// From big endian
   [[gnu::always_inline]] static constexpr int be2host(int a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// Rotate left
   [[gnu::always_inline]] static constexpr int rotl(unsigned a, unsigned shift) noexcept { return (a << shift) | (a >> ((8 * sizeof(unsigned)) - shift)); } // NOLINT
   /// Rotate right
   [[gnu::always_inline]] static constexpr int rotr(unsigned a, unsigned shift) noexcept { return (a >> shift) | (a << ((8 * sizeof(unsigned)) - shift)); } // NOLINT
   /// The high word of a multiplication
   [[gnu::always_inline]] static constexpr int mulHigh(int a, int b) noexcept {
      return (static_cast<long long>(a) * static_cast<long long>(b)) >> (8 * sizeof(int)); // NOLINT
   }
};
/// Builtin integer operations
template <>
struct IntegerOperations<unsigned> {
   /// Count leading zeroes
   [[gnu::always_inline]] static constexpr unsigned clz(unsigned a) noexcept {
      assert(a);
      return __builtin_clz(a);
   }
   /// Count trailing zeroes
   [[gnu::always_inline]] static constexpr unsigned ctz(unsigned a) noexcept {
      assert(a);
      return __builtin_ctz(a);
   }
   /// Count number of set bits
   [[gnu::always_inline]] static constexpr unsigned popcount(unsigned a) noexcept {
      return __builtin_popcount(a);
   }
   /// Compute log2, rounding down. Undefined for 0
   [[gnu::always_inline]] static constexpr unsigned log2(unsigned a) noexcept {
      assert(a);
      return (8 * sizeof(unsigned) - 1) - __builtin_clz(a);
   }
   /// Compute log2, rounding up. Undefined for a <= 1
   [[gnu::always_inline]] static constexpr unsigned log2ceil(unsigned a) noexcept {
      assert(a > 1);
      return (8 * sizeof(unsigned)) - __builtin_clz(a - 1);
   }
   /// Bswap
   [[gnu::always_inline]] static constexpr unsigned bswap(unsigned a) noexcept { return __builtin_bswap32(a); }
   /// As little endian
   [[gnu::always_inline]] static constexpr unsigned host2le(unsigned a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// As big endian
   [[gnu::always_inline]] static constexpr unsigned host2be(unsigned a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// From little endian
   [[gnu::always_inline]] static constexpr unsigned le2host(unsigned a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// From big endian
   [[gnu::always_inline]] static constexpr unsigned be2host(unsigned a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// Rotate left
   [[gnu::always_inline]] static constexpr unsigned rotl(unsigned a, unsigned shift) noexcept { return (a << shift) | (a >> ((8 * sizeof(unsigned)) - shift)); }
   /// Rotate right
   [[gnu::always_inline]] static constexpr unsigned rotr(unsigned a, unsigned shift) noexcept { return (a >> shift) | (a << ((8 * sizeof(unsigned)) - shift)); }
   /// The high word of a multiplication
   [[gnu::always_inline]] static constexpr unsigned mulHigh(unsigned a, unsigned b) noexcept {
      return (static_cast<unsigned long long>(a) * static_cast<unsigned long long>(b)) >> (8 * sizeof(unsigned));
   }
};
/// Builtin integer operations
template <>
struct IntegerOperations<long> {
   /// Count leading zeroes
   [[gnu::always_inline]] static constexpr unsigned clz(unsigned long a) noexcept {
      assert(a);
      return __builtin_clzl(a);
   }
   /// Count trailing zeroes
   [[gnu::always_inline]] static constexpr unsigned ctz(unsigned long a) noexcept {
      assert(a);
      return __builtin_ctzl(a);
   }
   /// Count number of set bits
   [[gnu::always_inline]] static constexpr unsigned popcount(unsigned long a) noexcept {
      return __builtin_popcountl(a);
   }
   /// Compute log2, rounding down. Undefined for a <= 0
   [[gnu::always_inline]] static constexpr unsigned log2(unsigned long a) noexcept {
      assert(a);
      return (8 * sizeof(unsigned long) - 1) - __builtin_clzl(a);
   }
   /// Compute log2, rounding up. Undefined for a <= 1
   [[gnu::always_inline]] static constexpr unsigned log2ceil(unsigned long a) noexcept {
      assert(a > 1);
      return (8 * sizeof(unsigned long)) - __builtin_clzl(a - 1);
   }
   /// Bswap
   [[gnu::always_inline]] static constexpr long bswap(long a) noexcept { return (sizeof(long) == 4) ? __builtin_bswap32(a) : __builtin_bswap64(a); } // NOLINT
   /// As little endian
   [[gnu::always_inline]] static constexpr long host2le(long a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// As big endian
   [[gnu::always_inline]] static constexpr long host2be(long a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// From little endian
   [[gnu::always_inline]] static constexpr long le2host(long a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// From big endian
   [[gnu::always_inline]] static constexpr long be2host(long a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// Rotate left
   [[gnu::always_inline]] static constexpr long rotl(unsigned long a, unsigned shift) noexcept { return (a << shift) | (a >> ((8 * sizeof(unsigned long)) - shift)); } // NOLINT
   /// Rotate right
   [[gnu::always_inline]] static constexpr long rotr(unsigned long a, unsigned shift) noexcept { return (a >> shift) | (a << ((8 * sizeof(unsigned long)) - shift)); } // NOLINT
   /// The high word of a multiplication
   [[gnu::always_inline]] static constexpr long mulHigh(long a, long b) noexcept {
      return (static_cast<__int128>(a) * static_cast<__int128>(b)) >> (8 * sizeof(long)); // NOLINT
   }
};
/// Builtin integer operations
template <>
struct IntegerOperations<unsigned long> {
   /// Count leading zeroes
   [[gnu::always_inline]] static constexpr unsigned clz(unsigned long a) noexcept {
      assert(a);
      return __builtin_clzl(a);
   }
   /// Count trailing zeroes
   [[gnu::always_inline]] static constexpr unsigned ctz(unsigned long a) noexcept {
      assert(a);
      return __builtin_ctzl(a);
   }
   /// Count number of set bits
   [[gnu::always_inline]] static constexpr unsigned popcount(unsigned long a) noexcept {
      return __builtin_popcountl(a);
   }
   /// Compute log2, rounding down. Undefined for 0
   [[gnu::always_inline]] static constexpr unsigned log2(unsigned long a) noexcept {
      assert(a);
      return (8 * sizeof(unsigned long) - 1) - __builtin_clzl(a);
   }
   /// Compute log2, rounding up. Undefined for a <= 1
   [[gnu::always_inline]] static constexpr unsigned log2ceil(unsigned long a) noexcept {
      assert(a > 1);
      return (8 * sizeof(unsigned long)) - __builtin_clzl(a - 1);
   }
   /// Bswap
   [[gnu::always_inline]] static constexpr unsigned long bswap(unsigned long a) noexcept { return (sizeof(unsigned long) == 4) ? __builtin_bswap32(a) : __builtin_bswap64(a); }
   /// As little endian
   [[gnu::always_inline]] static constexpr unsigned long host2le(unsigned long a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// As big endian
   [[gnu::always_inline]] static constexpr unsigned long host2be(unsigned long a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// From little endian
   [[gnu::always_inline]] static constexpr unsigned long le2host(unsigned long a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// From big endian
   [[gnu::always_inline]] static constexpr unsigned long be2host(unsigned long a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// Rotate left
   [[gnu::always_inline]] static constexpr unsigned long rotl(unsigned long a, unsigned shift) noexcept { return (a << shift) | (a >> ((8 * sizeof(unsigned long)) - shift)); }
   /// Rotate right
   [[gnu::always_inline]] static constexpr unsigned long rotr(unsigned long a, unsigned shift) noexcept { return (a >> shift) | (a << ((8 * sizeof(unsigned long)) - shift)); }
   /// The high word of a multiplication
   [[gnu::always_inline]] static constexpr unsigned long mulHigh(unsigned long a, unsigned long b) noexcept {
      return (static_cast<unsigned __int128>(a) * static_cast<unsigned __int128>(b)) >> (8 * sizeof(unsigned long));
   }
};
/// Builtin integer operations
template <>
struct IntegerOperations<long long> {
   /// Count leading zeroes
   [[gnu::always_inline]] static constexpr unsigned clz(unsigned long long a) noexcept {
      assert(a);
      return __builtin_clzll(a);
   }
   /// Count trailing zeroes
   [[gnu::always_inline]] static constexpr unsigned ctz(unsigned long long a) noexcept {
      assert(a);
      return __builtin_ctzll(a);
   }
   /// Count number of set bits
   [[gnu::always_inline]] static constexpr unsigned popcount(unsigned long long a) noexcept {
      return __builtin_popcountll(a);
   }
   /// Compute log2, rounding down. Undefined for a <= 0
   [[gnu::always_inline]] static constexpr unsigned log2(unsigned long long a) noexcept {
      assert(a);
      return (8 * sizeof(unsigned long long) - 1) - __builtin_clzll(a);
   }
   /// Compute log2, rounding up. Undefined for a <= 1
   [[gnu::always_inline]] static constexpr unsigned log2ceil(unsigned long long a) noexcept {
      assert(a > 1);
      return (8 * sizeof(unsigned long long)) - __builtin_clzll(a - 1);
   }
   /// Bswap
   [[gnu::always_inline]] static constexpr long long bswap(long long a) noexcept { return __builtin_bswap64(a); } // NOLINT
   /// As little endian
   [[gnu::always_inline]] static constexpr long long host2le(long long a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// As big endian
   [[gnu::always_inline]] static constexpr long long host2be(long long a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// From little endian
   [[gnu::always_inline]] static constexpr long long le2host(long long a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// From big endian
   [[gnu::always_inline]] static constexpr long long be2host(long long a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// Rotate left
   [[gnu::always_inline]] static constexpr long long rotl(unsigned long long a, unsigned shift) noexcept { return (a << shift) | (a >> ((8 * sizeof(unsigned long long)) - shift)); } // NOLINT
   /// Rotate right
   [[gnu::always_inline]] static constexpr long long rotr(unsigned long long a, unsigned shift) noexcept { return (a >> shift) | (a << ((8 * sizeof(unsigned long long)) - shift)); } // NOLINT
   /// The high word of a multiplication
   [[gnu::always_inline]] static constexpr long long mulHigh(long long a, long long b) noexcept {
      return (static_cast<__int128>(a) * static_cast<__int128>(b)) >> (8 * sizeof(long long)); // NOLINT
   }
};
/// Builtin integer operations
template <>
struct IntegerOperations<unsigned long long> {
   /// Count leading zeroes
   [[gnu::always_inline]] static constexpr unsigned clz(unsigned long long a) noexcept {
      assert(a);
      return __builtin_clzll(a);
   }
   /// Count trailing zeroes
   [[gnu::always_inline]] static constexpr unsigned ctz(unsigned long long a) noexcept {
      assert(a);
      return __builtin_ctzll(a);
   }
   /// Count number of set bits
   [[gnu::always_inline]] static constexpr unsigned popcount(unsigned long long a) noexcept {
      return __builtin_popcountll(a);
   }
   /// Compute log2, rounding down. Undefined for 0
   [[gnu::always_inline]] static constexpr unsigned log2(unsigned long long a) noexcept {
      assert(a);
      return (8 * sizeof(unsigned long long) - 1) - __builtin_clzll(a);
   }
   /// Compute log2, rounding up. Undefined for a <= 1
   [[gnu::always_inline]] static constexpr unsigned log2ceil(unsigned long long a) noexcept {
      assert(a > 1);
      return (8 * sizeof(unsigned long long)) - __builtin_clzll(a - 1);
   }
   /// Bswap
   [[gnu::always_inline]] static constexpr unsigned long long bswap(unsigned long long a) noexcept { return __builtin_bswap64(a); }
   /// As little endian
   [[gnu::always_inline]] static constexpr unsigned long long host2le(unsigned long long a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// As big endian
   [[gnu::always_inline]] static constexpr unsigned long long host2be(unsigned long long a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// From little endian
   [[gnu::always_inline]] static constexpr unsigned long long le2host(unsigned long long a) noexcept { return ByteOrder::bigEndian ? bswap(a) : a; }
   /// From big endian
   [[gnu::always_inline]] static constexpr unsigned long long be2host(unsigned long long a) noexcept { return ByteOrder::bigEndian ? a : bswap(a); }
   /// Rotate left
   [[gnu::always_inline]] static constexpr unsigned long long rotl(unsigned long long a, unsigned shift) noexcept { return (a << shift) | (a >> ((8 * sizeof(unsigned long long)) - shift)); }
   /// Rotate right
   [[gnu::always_inline]] static constexpr unsigned long long rotr(unsigned long long a, unsigned shift) noexcept { return (a >> shift) | (a << ((8 * sizeof(unsigned long long)) - shift)); }
   /// The high word of a multiplication
   [[gnu::always_inline]] static constexpr unsigned long long mulHigh(unsigned long long a, unsigned long long b) noexcept {
      return (static_cast<unsigned __int128>(a) * static_cast<unsigned __int128>(b)) >> (8 * sizeof(long long));
   }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif
