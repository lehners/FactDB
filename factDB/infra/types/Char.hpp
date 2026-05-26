// ---------------------------------------------------------------------------------------------------
// Adapted from:
// HyPer
// (c) Thomas Neumann 2010
// ---------------------------------------------------------------------------------------------------
#ifndef H_FACTDB_FACT_DB_INFRA_TYPES_CHAR_HPP
#define H_FACTDB_FACT_DB_INFRA_TYPES_CHAR_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/types/RuntimeValue.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <ostream>
#include <string_view>
#include <tuple>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
/// A fixed length string
template <unsigned kMaxLen>
class Char {
   private:
   public:
   /// The length
   // typename LengthIndicator<kMaxLen>::type len;
   uint64_t len;
   /// The data
   char value[kMaxLen];

   public:
   /// The length
   [[nodiscard]] unsigned length() const { return len; }
   /// Hash
   [[nodiscard]] inline uint64_t hash() const;
   /// The first character
   [[nodiscard]] char* begin() { return value; }
   /// Behind the last character
   [[nodiscard]] char* end() { return value + length(); }
   /// The first character
   [[nodiscard]] const char* begin() const { return value; }
   /// Behind the last character
   [[nodiscard]] const char* end() const { return value + length(); }

   /// Comparison
   [[nodiscard]] bool operator==(const char* other) const {
      return (other[0] == value[0]) && (len == strlen(other)) && (strncmp(value, other, len) == 0);
   }
   /// Comparison
   [[nodiscard]] bool operator!=(const char* other) const { return (len != strlen(other)) || (strncmp(value, other, len) != 0); }
   /// Comparison
   [[nodiscard]] bool operator==(const Char& other) const { return (len == other.len) && (memcmp(value, other.value, len) == 0); }
   /// Comparison
   [[nodiscard]] bool operator<(const Char& other) const;
   /// Comparison
   [[nodiscard]] bool operator>(const Char& other) const;
   /// Comparison
   [[nodiscard]] bool operator<=(const Char& other) const { return !(*this > other); }
   /// Comparison
   [[nodiscard]] bool operator>=(const Char& other) const { return !(*this < other); }

   /// Comparison
   [[nodiscard]] bool operator<=(const char* other) const { return *this <= castString(other, kMaxLen); }
   /// Comparison
   [[nodiscard]] bool operator>=(const char* other) const { return *this >= castString(other, kMaxLen); }

   /// Build from pointer with kMaxLen
   static Char build(const char* value) {
      Char result;
      memcpy(result.value, value, kMaxLen);
      result.len = strnlen(result.value, kMaxLen);
      return result;
   }
   /// Cast from pointer and explicit length
   static Char castString(const char* str, uint32_t strLen) {
      while ((*str) == ' ') {
         str++;
         strLen--;
      }
      assert(strLen <= kMaxLen);
      Char<kMaxLen> result;
      result.len = std::min(strLen, kMaxLen);
      memcpy(result.value, str, result.len);
      memset(result.value + result.len, 0, kMaxLen - result.len);
      return result;
   }
   /// Cast from string_view
   static Char castString(std::string_view sv) { return castString(sv.cbegin(), sv.length()); }

   /// Extract a Char from an input text
   static std::tuple<Char<kMaxLen>, const char*> extract(const char* it, const char* end, char delimiter) {
      auto start = it;
      while (*it != delimiter && it != end) { ++it; }
      auto len = std::distance(start, it);
      assert(len == kMaxLen);
      return {Char<kMaxLen>::castString(start, len), ++it};
   }

   /// Convert to RuntimeValue
   [[nodiscard]] RuntimeValue toRTV() const;
   /// Convert back from RuntimeValue
   static Char<kMaxLen> fromRTV(const RuntimeValue& rtv);
};
// ---------------------------------------------------------------------------------------------------
/// A fixed length string
template <>
class Char<1> {
   public:
   /// The value
   char value;

   public:
   /// The length
   [[nodiscard]] unsigned length() const { return value != ' '; }
   /// The first character
   [[nodiscard]] char* begin() { return &value; }
   /// Behind the last character
   [[nodiscard]] char* end() { return &value + length(); }
   /// The first character
   [[nodiscard]] const char* begin() const { return &value; }
   /// Behind the last character
   [[nodiscard]] const char* end() const { return &value + length(); }

   /// Comparison
   [[nodiscard]] bool operator==(const char* other) const { return (value == other[0]) && (strlen(other) == 1); }
   /// Comparison
   [[nodiscard]] bool operator==(const Char& other) const { return value == other.value; }
   /// Comparison
   [[nodiscard]] bool operator<(const Char& other) const { return value < other.value; }

   /// Build from pointer
   static Char build(const char* value) {
      Char result;
      result.value = *value;
      return result;
   }
   /// Cast from pointer and length
   static Char<1> castString(const char* str, [[maybe_unused]] uint32_t strLen) {
      assert(strLen);
      Char<1> x;
      x.value = str[0];
      return x;
   }
   /// Cast from string_view
   static Char<1> castString(std::string_view sv) { return castString(sv.cbegin(), sv.length()); }

   /// Extract a Char from an input text
   static std::tuple<Char<1>, const char*> extract(const char* it, const char* end, char delimiter) {
      auto start = it;
      while (*it != delimiter && it != end) { ++it; }
      auto len = std::distance(start, it);
      assert(len >= 1);
      return {Char<1>::castString(start, len), ++it};
   }

   /// Convert to RuntimeValue
   [[nodiscard]] RuntimeValue toRTV() const;
   /// Convert back from RuntimeValue
   static Char<1> fromRTV(const RuntimeValue& rtv);
};
// ---------------------------------------------------------------------------------------------------
// Output
template <unsigned kMaxLen>
std::ostream& operator<<(std::ostream& out, const Char<kMaxLen>& value) {
   for (auto iter = value.begin(), limit = value.end(); iter != limit; ++iter) {
      out << (*iter);
   }
   return out;
}
// ---------------------------------------------------------------------------------------------------
template <unsigned kMaxLen>
RuntimeValue Char<kMaxLen>::toRTV() const {
   RuntimeValue rtv;
   rtv.setType(schemac::Type::Char(kMaxLen));
   rtv.setString({value, value + len});
   return rtv;
}
// ---------------------------------------------------------------------------------------------------
// Comparison
template <unsigned kMaxLen>
bool Char<kMaxLen>::operator<(const Char& other) const {
   int c = memcmp(value, other.value, std::min(len, other.len));
   if (c < 0) return true;
   if (c > 0) return false;
   return len < other.len;
}
// ---------------------------------------------------------------------------------------------------
// Comparison
template <unsigned kMaxLen>
bool Char<kMaxLen>::operator>(const Char& other) const {
   int c = memcmp(value, other.value, std::min(len, other.len));
   if (c < 0) return false;
   if (c > 0) return true;
   return len > other.len;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACT_DB_INFRA_TYPES_CHAR_HPP
// ---------------------------------------------------------------------------------------------------
