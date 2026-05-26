// ---------------------------------------------------------------------------------------------------
// HyPer
// (c) Thomas Neumann 2010
// ---------------------------------------------------------------------------------------------------
#ifndef H_FACTDB_FACT_DB_INFRA_TYPES_UINT_HPP
#define H_FACTDB_FACT_DB_INFRA_TYPES_UINT_HPP
// ---------------------------------------------------------------------------------------------------
#include <cassert>
#include <cstdint>
#include <ostream>
#include <string_view>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct RuntimeValue;
// ---------------------------------------------------------------------------------------------------
/// UIntBase class
template <std::unsigned_integral T>
class UIntBase {
   public:
   using UIntType = T;
   T value;

   UIntBase() = default;
   explicit UIntBase(T valueParam) : value(valueParam) {}

   /// Comparison
   [[nodiscard]] inline bool operator==(const UIntBase& n) const { return value == n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator!=(const UIntBase& n) const { return value != n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator<(const UIntBase& n) const { return value < n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator<=(const UIntBase& n) const { return value <= n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator>(const UIntBase& n) const { return value > n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator>=(const UIntBase& n) const { return value >= n.value; }
   /// Add
   [[nodiscard]] inline UIntBase operator+(const UIntBase& n) const {
      UIntBase r;
      r.value = value + n.value;
      return r;
   }
   /// Add
   inline UIntBase& operator+=(const UIntBase& n) {
      value += n.value;
      return *this;
   }
   /// Sub
   inline UIntBase operator-(const UIntBase& n) const {
      UIntBase r;
      r.value = value - n.value;
      return r;
   }
   /// Mul
   inline UIntBase operator*(const UIntBase& n) const {
      UIntBase r;
      r.value = value * n.value;
      return r;
   }
   /// Modulo
   inline UIntBase operator%(const UIntBase& n) const {
      assert(n.value);
      UIntBase r;
      r.value = value % n.value;
      return r;
   }
   /// Modulo
   inline UIntBase operator%(int32_t n) const {
      assert(n);
      UIntBase r;
      r.value = value % n;
      return r;
   }

   // protected:
   // static T castString(const char* str, uint32_t strLen, RawTag) { return castStringToIntegral<T>(str, strLen); };
   // static T castString(std::string_view sv, RawTag) { return castStringToIntegral<T>(sv); }
};
// ---------------------------------------------------------------------------------------------------
class UInt16 : public UIntBase<uint16_t> {
   public:
   UInt16() = default;
   explicit UInt16(uint64_t valueParam) : UIntBase<uint16_t>(valueParam) {}

   /// Convert to RuntimeValue
   [[nodiscard]] RuntimeValue toRTV() const;
   /// Convert back from RuntimeValue
   static UInt16 fromRTV(const RuntimeValue& rtv);

   /// Cast
   static UInt16 castString(const char* str, uint32_t strLen);
   static UInt16 castString(std::string_view sv);
};
// ---------------------------------------------------------------------------------------------------
class UInt32 : public UIntBase<uint32_t> {
   public:
   UInt32() = default;
   explicit UInt32(uint64_t valueParam) : UIntBase<uint32_t>(valueParam) {}

   /// Convert to RuntimeValue
   [[nodiscard]] RuntimeValue toRTV() const;
   /// Convert back from RuntimeValue
   static UInt32 fromRTV(const RuntimeValue& rtv);

   /// Cast
   static UInt32 castString(const char* str, uint32_t strLen);
   static UInt32 castString(std::string_view sv);
};
// ---------------------------------------------------------------------------------------------------
class UInt64 : public UIntBase<uint64_t> {
   public:
   UInt64() = default;
   explicit UInt64(uint64_t valueParam) : UIntBase<uint64_t>(valueParam) {}

   /// Convert to RuntimeValue
   [[nodiscard]] RuntimeValue toRTV() const;
   /// Convert back from RuntimeValue
   static UInt64 fromRTV(const RuntimeValue& rtv);

   /// Cast
   static UInt64 castString(const char* str, uint32_t strLen);
   static UInt64 castString(std::string_view sv);
};
// ---------------------------------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& out, const UInt16& value) { return out << value.value; }
// ---------------------------------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& out, const UInt32& value) { return out << value.value; }
// ---------------------------------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& out, const UInt64& value) { return out << value.value; }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACT_DB_INFRA_TYPES_UINT_HPP