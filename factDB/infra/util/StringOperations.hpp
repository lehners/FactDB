#ifndef H_FACTDB_INFRA_UTIL_STRINGOPERATIONS_HPP
#define H_FACTDB_INFRA_UTIL_STRINGOPERATIONS_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Unaligned.hpp"
#include "factDB/infra/util/IntegerOperations.hpp"
#include <type_traits>
// ---------------------------------------------------------------------------------------------------
// Umbra
// (c) 2016 Thomas Neumann
// ---------------------------------------------------------------------------------------------------
/// Commonly used operations when manipulating string data
namespace factDB::StringOperations {
// ---------------------------------------------------------------------------------------------------
/// Read 8 bytes from a string, where all 8 bytes must be within the string
inline uint64_t read8Unchecked(const char* str) noexcept {
   return unalignedLoad<uint64_t>(str);
}
// ---------------------------------------------------------------------------------------------------
/// Safely read 8 bytes from a string, even if the string is shorter. Pads with \0
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
inline uint64_t
#if defined(__clang__)
   __attribute__((no_sanitize("address", "undefined")))
#else
   __attribute__((no_sanitize_address, no_sanitize_undefined))
#endif
   read8(const char* str, uintptr_t len) noexcept {
   if (!len) {
      return 0;
   } else if (len >= 8) {
      return reinterpret_cast<const Unaligned<uint64_t>*>(str)->value;
   } else if ((reinterpret_cast<uintptr_t>(str) & 63) <= 56) {
      uint64_t block = reinterpret_cast<const Unaligned<uint64_t>*>(str)->value;
      unsigned shift = len << 3;
      return block & (~(ByteOrder::bigEndian ? ((~0ull) >> shift) : ((~0ull) << shift)));
   } else {
      uint64_t block = reinterpret_cast<const Unaligned<uint64_t>*>(str + len - 8)->value;
      unsigned shift = 64 - (len << 3);
      return ByteOrder::bigEndian ? (block << shift) : (block >> shift);
   }
}
#pragma GCC diagnostic pop
// ---------------------------------------------------------------------------------------------------
/// Safely read 8 bytes from a string. Convenience function with a different signature
inline uint64_t read8(const char* str, const char* strLimit) noexcept {
   return read8(str, strLimit - str);
}
// ---------------------------------------------------------------------------------------------------
/// Write up to 8 bytes back into a string
inline void write8(char* str, uint64_t block, uintptr_t len) noexcept { // NOLINT
   if (len >= 8) {
      unalignedStore<uint64_t>(str, block);
   } else {
      if (len >= 4) {
         unalignedStore<uint32_t>(str, ByteOrder::bigEndian ? (block >> (64 - 32)) : block);
         block = ByteOrder::bigEndian ? (block << 32) : (block >> 32);
         str += 4;
         len -= 4;
      }
      if (len >= 2) {
         unalignedStore<uint16_t>(str, ByteOrder::bigEndian ? (block >> (64 - 16)) : block);
         block = ByteOrder::bigEndian ? (block << 16) : (block >> 16);
         str += 2;
         len -= 2;
      }
      if (len == 1)
         unalignedStore<uint8_t>(str, ByteOrder::bigEndian ? (block >> (64 - 8)) : block);
   }
}
// ---------------------------------------------------------------------------------------------------
/// Get an 8 byte block as little endian
inline uint64_t get8AsLittleEndian(uint64_t block) noexcept { // this line is unreachable, mis-identified by coverage as code
   return ByteOrder::bigEndian ? IntegerOperations<uint64_t>::bswap(block) : block;
}
// ---------------------------------------------------------------------------------------------------
/// Get an 8 byte block as big endian
inline uint64_t get8AsBigEndian(uint64_t block) noexcept {
   return ByteOrder::bigEndian ? block : IntegerOperations<uint64_t>::bswap(block);
}
// ---------------------------------------------------------------------------------------------------
/// Broadcast a character to a 8 byte block
inline constexpr uint64_t broadcast8(unsigned char c) noexcept {
   return (static_cast<uint64_t>(c) << 56) | (static_cast<uint64_t>(c) << 48) | (static_cast<uint64_t>(c) << 40) | (static_cast<uint64_t>(c) << 32) |
      (static_cast<uint64_t>(c) << 24) | (static_cast<uint64_t>(c) << 16) | (static_cast<uint64_t>(c) << 8) | c;
}
// ---------------------------------------------------------------------------------------------------
/// Find the position of multi-byte characters
inline constexpr uint64_t findMultibytePositions(uint64_t block) noexcept { return block & broadcast8(0x80); }
// ---------------------------------------------------------------------------------------------------
/// Find characters that are within a range in a 8 byte block
template <char lower, char upper>
inline constexpr uint64_t findCharactersInsideRange(uint64_t block) noexcept {
   static_assert((lower < upper) && (0 < lower) && (upper < 128), "character range not supported");
   constexpr uint64_t highBits = broadcast8(0x80);
   constexpr uint64_t lowOffset = broadcast8(128 - lower);
   constexpr uint64_t highOffset = broadcast8(128 - upper - 1);
   uint64_t blockLower = block & (~highBits);
   return (~(block & highBits)) & ((blockLower + lowOffset) & highBits) & (~((blockLower + highOffset) & highBits));
}
// ---------------------------------------------------------------------------------------------------
/// Find characters that are outside a range in a 8 byte block
template <char lower, char upper>
inline constexpr uint64_t findCharactersOutsideRange(uint64_t block) noexcept // coverage test problem, mark unreachable
{
   static_assert((lower < upper) && (0 < lower) && (upper < 128), "character range not supported");
   constexpr uint64_t highBits = broadcast8(0x80);
   constexpr uint64_t lowOffset = broadcast8(128 - lower);
   constexpr uint64_t highOffset = broadcast8(128 - upper - 1);
   uint64_t blockLower = block & (~highBits);
   return (block & highBits) | ((((blockLower + lowOffset) & highBits) & (~((blockLower + highOffset) & highBits))) ^ highBits);
}
// ---------------------------------------------------------------------------------------------------
/// Convert a 8 byte block to lower case
inline constexpr uint64_t getBlockAsLowerCase(uint64_t block) noexcept {
   return block + (findCharactersInsideRange<'A', 'Z'>(block) >> 2);
}
// ---------------------------------------------------------------------------------------------------
/// Convert a 8 byte block to upper case
inline constexpr uint64_t getBlockAsUpperCase(uint64_t block) noexcept {
   return block - (findCharactersInsideRange<'a', 'z'>(block) >> 2);
}
// ---------------------------------------------------------------------------------------------------
/// Interpret the result of a findCharacter operation
inline unsigned getFirstMatchPosition(uint64_t matches) noexcept { // this line is unreachable, mis-identified by coverage as code
   return ByteOrder::bigEndian ? (IntegerOperations<uint64_t>::clz(matches) >> 3) : (IntegerOperations<uint64_t>::ctz(matches) >> 3);
}
// ---------------------------------------------------------------------------------------------------
/// Interpret the result of a findCharacter operation
inline unsigned getFirstMatchPositionLittleEndian(uint64_t matches) noexcept {
   return IntegerOperations<uint64_t>::ctz(matches) >> 3;
}
// ---------------------------------------------------------------------------------------------------
/// Strip leading and trailing whitespace
inline void trimWhitespace(const char*& str, const char*& strLimit) noexcept {
   while ((str != strLimit) && ((*str) == ' ')) ++str;
   while ((str != strLimit) && ((strLimit[-1] == ' '))) --strLimit;
}
// ---------------------------------------------------------------------------------------------------
/// Handle the sign, return true if negative
inline bool handleSign(const char*& str, const char* strLimit) noexcept { // this line is unreachable, mis-identified by coverage as code
   unsigned char c = ((str == strLimit) ? '0' : *str);
   if (c < '0') {
      if (c == '-') {
         ++str;
         return true;
      }
      if (c == '+')
         ++str;
   }
   return false;
}
// ---------------------------------------------------------------------------------------------------
/// A table with all 100 2 digit combinations
extern const char twoDigitsTable[]; // NOLINT
// ---------------------------------------------------------------------------------------------------
/// Write two digits backwards into a target string
inline char* writeTwoDigitsBackwards(char* target, unsigned digits) noexcept {
   unalignedArrayStore<uint16_t>(target, -1, unalignedArrayLoad<uint16_t>(twoDigitsTable, digits));
   return target - 2;
}
// ---------------------------------------------------------------------------------------------------
/// Write an unsigned number backwards into a target string. Can write up to 20 digits
char* writeUnsignedBackwards(char* target, uint64_t value) noexcept;
/// Write an unsigned number backwards into a target string, with a minimum length. Can write up to 20 digits (plus padding)
char* writePadded0UnsignedBackwards(char* target, uint64_t value, unsigned minLen) noexcept;
/// Write an unsigned number that has been scaled by (10^scale) into a target string. Can write up to max(scale+2,21) digits
char* writeScaledUnsignedBackwards(char* target, uint64_t value, unsigned scale) noexcept;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::StringOperations
// ---------------------------------------------------------------------------------------------------
#endif
