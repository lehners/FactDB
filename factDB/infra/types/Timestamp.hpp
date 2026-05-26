// ---------------------------------------------------------------------------------------------------
// Adapted from:
// HyPer
// (c) Thomas Neumann 2010
// ---------------------------------------------------------------------------------------------------
#ifndef H_FACTDB_FACT_DB_INFRA_TYPES_TIMESTAMP_HPP
#define H_FACTDB_FACT_DB_INFRA_TYPES_TIMESTAMP_HPP
// ---------------------------------------------------------------------------------------------------
#include <cstdint>
#include <ostream>
#include <string_view>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct RuntimeValue;
// ---------------------------------------------------------------------------------------------------
/// A date
class Date {
   public:
   /// The value
   uint64_t value;

   /// Constructor
   Date() = default;
   explicit Date(uint64_t val) : value(val) {}

   /// NULL
   static Date null();

   /// The value
   [[nodiscard]] uint64_t getRaw() const { return value; }

   /// Comparison
   [[nodiscard]] bool operator==(const Date& t) const { return value == t.value; }
   /// Comparison
   [[nodiscard]] bool operator!=(const Date& t) const { return value != t.value; }
   /// Comparison
   [[nodiscard]] bool operator<(const Date& t) const { return value < t.value; }
   [[nodiscard]] bool operator<=(const Date& t) const { return value <= t.value; }
   [[nodiscard]] bool operator>=(const Date& t) const { return value >= t.value; }
   [[nodiscard]] bool operator>(const Date& t) const { return value > t.value; }
   /// Cast Timestamp from string
   [[nodiscard]] static Date castString(const char* str, uint32_t strLen);
   [[nodiscard]] static Date castString(std::string_view sv, std::string_view format_string = "%Y-%m-%d");

   /// Convert to RuntimeValue
   [[nodiscard]] RuntimeValue toRTV() const;
   /// Convert back from RuntimeValue
   [[nodiscard]] static Date fromRTV(const RuntimeValue& rtv);
};
// ---------------------------------------------------------------------------------------------------
/// A timestamp
class Timestamp {
   public:
   /// The value
   uint64_t value;

   /// Constructor
   Timestamp() = default;
   explicit Timestamp(uint64_t val) : value(val) {}

   /// NULL
   static Timestamp null();

   /// The value
   [[nodiscard]] uint64_t getRaw() const { return value; }

   /// Comparison
   bool operator==(const Timestamp& t) const { return value == t.value; }
   /// Comparison
   bool operator!=(const Timestamp& t) const { return value != t.value; }
   /// Comparison
   bool operator<(const Timestamp& t) const { return value < t.value; }
   bool operator<=(const Timestamp& t) const { return value <= t.value; }
   bool operator>=(const Timestamp& t) const { return value >= t.value; }
   bool operator>(const Timestamp& t) const { return value > t.value; }
   /// Cast Timestamp from string
   static Timestamp castString(const char* str, uint32_t strLen);
   static Timestamp castString(std::string_view sv, std::string_view format_string = "%Y-%m-%d %H:%M:%S");

   /// Convert to RuntimeValue
   [[nodiscard]] RuntimeValue toRTV() const;
   /// Convert back from RuntimeValue
   static Timestamp fromRTV(const RuntimeValue& rtv);
};
// ---------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Timestamp& value);
// ---------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Date& value);
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACT_DB_INFRA_TYPES_TIMESTAMP_HPP
// ---------------------------------------------------------------------------------------------------
