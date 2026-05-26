// ---------------------------------------------------------------------------------------------------
// HyPer
// Numeric<18,2>
// ---------------------------------------------------------------------------------------------------
#ifndef H_FACTDB_FACT_DB_INFRA_TYPES_NUMERIC_HPP
#define H_FACTDB_FACT_DB_INFRA_TYPES_NUMERIC_HPP
// ---------------------------------------------------------------------------------------------------
#include <cassert>
#include <cmath>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string_view>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct RuntimeValue;
// ---------------------------------------------------------------------------------------------------
class Numeric18_2 {
   public:
   // Underlying storage: scaled integer to store two decimal places
   // Range: ±9999999999999999.99 (16 digits before decimal)
   int64_t value; // value * 100

   Numeric18_2() = default;
   explicit Numeric18_2(double val) : value(static_cast<int64_t>(std::round(val * 100.0))) {}
   explicit Numeric18_2(int64_t rawVal, bool raw) : value(rawVal) { (void) raw; } // raw storage constructor

   // Comparison operators
   [[nodiscard]] inline bool operator==(const Numeric18_2& n) const { return value == n.value; }
   [[nodiscard]] inline bool operator!=(const Numeric18_2& n) const { return value != n.value; }
   [[nodiscard]] inline bool operator<(const Numeric18_2& n) const { return value < n.value; }
   [[nodiscard]] inline bool operator<=(const Numeric18_2& n) const { return value <= n.value; }
   [[nodiscard]] inline bool operator>(const Numeric18_2& n) const { return value > n.value; }
   [[nodiscard]] inline bool operator>=(const Numeric18_2& n) const { return value >= n.value; }

   // Arithmetic operators
   [[nodiscard]] inline Numeric18_2 operator+(const Numeric18_2& n) const {
      return Numeric18_2(value + n.value, true);
   }

   [[nodiscard]] inline Numeric18_2 operator-(const Numeric18_2& n) const {
      return Numeric18_2(value - n.value, true);
   }

   [[nodiscard]] inline Numeric18_2 operator*(const Numeric18_2& n) const {
      // Multiply and adjust scale
      __int128_t temp = static_cast<__int128_t>(value) * n.value;
      return Numeric18_2(static_cast<int64_t>(temp / 100), true);
   }

   [[nodiscard]] inline Numeric18_2 operator/(const Numeric18_2& n) const {
      if (n.value == 0) throw std::runtime_error("Division by zero");
      __int128_t temp = (static_cast<__int128_t>(value) * 100);
      return Numeric18_2(static_cast<int64_t>(temp / n.value), true);
   }

   inline Numeric18_2& operator+=(const Numeric18_2& n) {
      value += n.value;
      return *this;
   }
   inline Numeric18_2& operator-=(const Numeric18_2& n) {
      value -= n.value;
      return *this;
   }
   inline Numeric18_2& operator*=(const Numeric18_2& n) {
      *this = *this * n;
      return *this;
   }
   inline Numeric18_2& operator/=(const Numeric18_2& n) {
      *this = *this / n;
      return *this;
   }

   [[nodiscard]] double toDouble() const { return static_cast<double>(value) / 100.0; }

   // Conversion to RuntimeValue
   [[nodiscard]] RuntimeValue toRTV() const;
   static Numeric18_2 fromRTV(const RuntimeValue& rtv);

   // Parse from string
   static Numeric18_2 castString(const char* str, uint32_t strLen);
   static Numeric18_2 castString(std::string_view sv) { return castString(sv.data(), sv.size()); }
};

// Stream output
inline std::ostream& operator<<(std::ostream& out, const Numeric18_2& n) {
   out << n.toDouble();
   return out;
}

// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACT_DB_INFRA_TYPES_NUMERIC_HPP
