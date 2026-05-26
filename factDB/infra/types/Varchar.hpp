// ---------------------------------------------------------------------------------------------------
// Adapted from:
// HyPer
// (c) Thomas Neumann 2010
// ---------------------------------------------------------------------------------------------------
#ifndef IMLAB_VARCHAR_H
#define IMLAB_VARCHAR_H
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/types/RuntimeValue.hpp"
#include "factDB/schemac/Type.hpp"
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
/// A variable length string
template <unsigned kMaxLen>
class Varchar {
   public:
   /// The length
   uint64_t len;
   /// The value
   char value[kMaxLen];

   public:
   /// The length
   [[nodiscard]] unsigned length() const { return len; }
   /// Hash
   [[nodiscard]] inline uint64_t hash() const;
   /// The first character
   char* begin() { return value; }
   /// Behind the last character
   char* end() { return value + length(); }
   /// The first character
   [[nodiscard]] const char* begin() const { return value; }
   /// Behind the last character
   [[nodiscard]] const char* end() const { return value + length(); }

   /// Comparison
   bool operator==(const char* other) const { return strncmp(value, other, len) == 0; }
   /// Comparison
   bool operator==(const Varchar& other) const { return (len == other.len) && (memcmp(value, other.value, len) == 0); }
   /// Comparison
   bool operator<(const Varchar& other) const;
   /// Comparison
   bool operator>(const Varchar& other) const;

   /// Build
   static Varchar build(const char* value) {
      Varchar result;
      strncpy(&result.value, &value, kMaxLen);
      result.len = strnlen(value, kMaxLen);
      return result;
   }
   /// Cast
   static Varchar<kMaxLen> castString(const char* str, uint32_t strLen) {
      assert(strLen <= kMaxLen);
      Varchar<kMaxLen> result;
      result.len = std::min(strLen, kMaxLen);
      memcpy(result.value, str, result.len);
      memset(result.value + result.len, 0, kMaxLen - result.len);
      return result;
   }
   /// Cast
   static Varchar<kMaxLen> castString(std::string_view sv) { return castString(sv.cbegin(), sv.length()); }

   /// Extract a Varchar from an input text
   static std::tuple<Varchar<kMaxLen>, const char*> extract(const char* it, const char* end, char delimiter) {
      auto start = it;
      while (*it != delimiter && it != end) { ++it; }
      auto len = std::distance(start, it);
      assert(len <= kMaxLen);
      return {Varchar<kMaxLen>::castString(start, len), ++it};
   }

   /// Convert to RuntimeValue
   [[nodiscard]] RuntimeValue toRTV() const;
   /// Convert back from RuntimeValue
   static Varchar<kMaxLen> fromRTV(const RuntimeValue& rtv);
};
// ---------------------------------------------------------------------------------------------------
// Output
template <unsigned kMaxLen>
std::ostream& operator<<(std::ostream& out, const Varchar<kMaxLen>& value) {
   for (auto iter = value.begin(), limit = value.end(); iter != limit; ++iter) {
      out << (*iter);
   }
   return out;
}
// ---------------------------------------------------------------------------------------------------
template <unsigned kMaxLen>
RuntimeValue Varchar<kMaxLen>::toRTV() const {
   RuntimeValue rtv;
   rtv.setType(schemac::Type::Varchar(kMaxLen));
   rtv.setString({value, value + len});
   return rtv;
}
// ---------------------------------------------------------------------------------------------------
template <unsigned kMaxLen>
Varchar<kMaxLen> Varchar<kMaxLen>::fromRTV(const RuntimeValue& rtv) {
   assert(rtv.getType() == schemac::Type::Varchar(kMaxLen));
   RuntimeString rts = rtv.get<RuntimeString>();
   Varchar<kMaxLen> v;
   v.len = std::min(static_cast<uint64_t>(kMaxLen), rts.getLength());
   std::memcpy(v.value, rts.getPointer(), v.len);
   return v;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif //IMLAB_VARCHAR_H
// ---------------------------------------------------------------------------------------------------
