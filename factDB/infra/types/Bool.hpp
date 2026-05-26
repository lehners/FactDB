// ---------------------------------------------------------------------------------------------------
// HyPer
// (c) Thomas Neumann 2010
// ---------------------------------------------------------------------------------------------------
#ifndef H_FACTDB_FACT_DB_INFRA_TYPES_BOOL_HPP
#define H_FACTDB_FACT_DB_INFRA_TYPES_BOOL_HPP
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
class Bool {
   public:
   bool value;

   Bool() = default;
   explicit Bool(bool valueParam) : value(valueParam) {}

   /// Comparison
   [[nodiscard]] inline bool operator==(const Bool& n) const { return value == n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator!=(const Bool& n) const { return value != n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator<(const Bool& n) const { return value < n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator<=(const Bool& n) const { return value <= n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator>(const Bool& n) const { return value > n.value; }
   /// Comparison
   [[nodiscard]] inline bool operator>=(const Bool& n) const { return value >= n.value; }
   /// Add
   [[nodiscard]] inline Bool operator+(const Bool& n) const {
      Bool r;
      r.value = value + n.value;
      return r;
   }
   inline Bool& operator&=(const Bool& n) {
      value &= n.value;
      return *this;
   }
   inline Bool& operator|=(const Bool& n) {
      value |= n.value;
      return *this;
   }
   inline Bool operator&&(const Bool& n) const {
      Bool r;
      r.value = value && n.value;
      return r;
   }
   inline Bool operator||(const Bool& n) const {
      Bool r;
      r.value = value || n.value;
      return r;
   }

   [[nodiscard]] bool toBool() const { return value; }

   /// Convert to RuntimeValue
   [[nodiscard]] RuntimeValue toRTV() const;
   /// Convert back from RuntimeValue
   static Bool fromRTV(const RuntimeValue& rtv);

   static Bool castString(const char* str, uint32_t strLen);
   static Bool castString(std::string_view sv) { return castString(sv.cbegin(), sv.length()); }
};
// ---------------------------------------------------------------------------------------------------
inline std::ostream& operator<<(std::ostream& out, const Bool& value) { return out << value.value; }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACT_DB_INFRA_TYPES_BOOL_HPP