// ---------------------------------------------------------------------------------------------------
// HyPer
// (c) Thomas Neumann 2010
// ---------------------------------------------------------------------------------------------------
#ifndef H_FACTDB_FACT_DB_INFRA_TYPES_INTEGER_HPP
#define H_FACTDB_FACT_DB_INFRA_TYPES_INTEGER_HPP
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
/// Integer class
/// UIntBase class
template <std::signed_integral T>
class IntBase {
   public:
   using IntType = T;
   T value = 0;

   IntBase() = default;
   explicit IntBase(T valueParam) : value(valueParam) {}
   ~IntBase() = default;

   /// Comparison
   [[nodiscard]] inline bool operator==(const IntBase& n) const { return value == n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator!=(const IntBase& n) const { return value != n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator<(const IntBase& n) const { return value < n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator<=(const IntBase& n) const { return value <= n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator>(const IntBase& n) const { return value > n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator>=(const IntBase& n) const { return value >= n.value; }
   /// Add
   [[nodiscard]] inline IntBase operator+(const IntBase& n) const {
      IntBase r;
      r.value = value + n.value;
      return r;
   }
   /// Add
   inline IntBase& operator+=(const IntBase& n) {
      value += n.value;
      return *this;
   }
   /// Sub
   inline IntBase operator-(const IntBase& n) const {
      IntBase r;
      r.value = value - n.value;
      return r;
   }
   /// Mul
   inline IntBase operator*(const IntBase& n) const {
      IntBase r;
      r.value = value * n.value;
      return r;
   }
   /// Modulo
   inline IntBase operator%(const IntBase& n) const {
      assert(n.value);
      IntBase r;
      r.value = value % n.value;
      return r;
   }
   /// Modulo
   inline IntBase operator%(int32_t n) const {
      assert(n);
      IntBase r;
      r.value = value % n;
      return r;
   }
};
// ---------------------------------------------------------------------------------------------------
class Integer : public IntBase<int32_t> {
   public:
   Integer() = default;
   explicit Integer(int32_t valueParam) : IntBase(valueParam) {}
   ~Integer() = default;

   /// Convert to RuntimeValue
   [[nodiscard]] RuntimeValue toRTV() const;
   /// Convert back from RuntimeValue
   static Integer fromRTV(const RuntimeValue& rtv);

   static Integer castString(const char* str, uint32_t strLen);
   static Integer castString(std::string_view sv);
};
// ---------------------------------------------------------------------------------------------------
class Int16 : public IntBase<int16_t> {
   public:
   Int16() = default;
   explicit Int16(int16_t valueParam) : IntBase(valueParam) {}
   ~Int16() = default;

   /// Convert to RuntimeValue
   [[nodiscard]] RuntimeValue toRTV() const;
   /// Convert back from RuntimeValue
   static Int16 fromRTV(const RuntimeValue& rtv);

   /// Cast
   static Int16 castString(const char* str, uint32_t strLen);
   static Int16 castString(std::string_view sv);
};
// ---------------------------------------------------------------------------------------------------
class Int32 : public IntBase<int32_t> {
   public:
   Int32() = default;
   explicit Int32(int32_t valueParam) : IntBase(valueParam) {}
   ~Int32() = default;

   /// Convert to RuntimeValue
   [[nodiscard]] RuntimeValue toRTV() const;
   /// Convert back from RuntimeValue
   static Int32 fromRTV(const RuntimeValue& rtv);

   /// Cast
   static Int32 castString(const char* str, uint32_t strLen);
   static Int32 castString(std::string_view sv);
};
// ---------------------------------------------------------------------------------------------------
class Int64 : public IntBase<int64_t> {
   public:
   Int64() = default;
   explicit Int64(int64_t valueParam) : IntBase(valueParam) {}
   ~Int64() = default;

   /// Convert to RuntimeValue
   [[nodiscard]] RuntimeValue toRTV() const;
   /// Convert back from RuntimeValue
   static Int64 fromRTV(const RuntimeValue& rtv);

   /// Cast
   static Int64 castString(const char* str, uint32_t strLen);
   static Int64 castString(std::string_view sv);
};
// ---------------------------------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& out, const Integer& value) { return out << value.value; }
// ---------------------------------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& out, const Int16& value) { return out << value.value; }
// ---------------------------------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& out, const Int32& value) { return out << value.value; }
// ---------------------------------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& out, const Int64& value) { return out << value.value; }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACT_DB_INFRA_TYPES_INTEGER_HPP