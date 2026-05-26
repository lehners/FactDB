// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/types/Timestamp.hpp"
#include "factDB/infra/RuntimeException.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include "fmt/format.h"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
using namespace std;
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
// Algorithm from the Calendar FAQ
static unsigned mergeJulianDay(unsigned year, unsigned month, unsigned day) { // NOLINT
   unsigned a = (14 - month) / 12;
   unsigned y = year + 4800 - a;
   unsigned m = month + (12 * a) - 3;

   return day + ((153 * m + 2) / 5) + (365 * y) + (y / 4) - (y / 100) + (y / 400) - 32045;
}
// ---------------------------------------------------------------------------------------------------
static const uint64_t msPerDay = 24ull * 60ull * 60ull * 1000ull;
// ---------------------------------------------------------------------------------------------------
// Merge into ms since midnight
static unsigned mergeTime(unsigned hour, unsigned minute, unsigned second, unsigned ms) {
   return ms + (1000 * second) + (60 * 1000 * minute) + (60 * 60 * 1000 * hour);
}
// ---------------------------------------------------------------------------------------------------
// Algorithm from the Calendar FAQ
static void splitJulianDay(unsigned jd, unsigned& year, unsigned& month, unsigned& day) { // NOLINT
   unsigned a = jd + 32044;
   unsigned b = (4 * a + 3) / 146097;
   unsigned c = a - ((146097 * b) / 4);
   unsigned d = (4 * c + 3) / 1461;
   unsigned e = c - ((1461 * d) / 4);
   unsigned m = (5 * e + 2) / 153;

   day = e - ((153 * m + 2) / 5) + 1;
   month = m + 3 - (12 * (m / 10));
   year = (100 * b) + d - 4800 + (m / 10);
}
// ---------------------------------------------------------------------------------------------------
// Split ms since midnight
static void splitTime(unsigned value, unsigned& hour, unsigned& minute, unsigned& second, unsigned& ms) { // NOLINT
   ms = value % 1000;
   value /= 1000;
   second = value % 60;
   value /= 60;
   minute = value % 60;
   value /= 60;
   hour = value % 24;
}
// ---------------------------------------------------------------------------------------------------
const char* readChar(const char* iter, const char* limit, const char should) {
   char c = *iter;
   if (iter == limit) {
      throw RuntimeException(InvalidTimestampFormat, "unexpected end of string");
   } else if (c == should) {
      return ++iter;
   } else {
      throw RuntimeException(InvalidTimestampFormat, fmt::format("expected {}, got {}", should, c));
   }
}
// ---------------------------------------------------------------------------------------------------
const char* processNumber(size_t& val, const char* iter, const char* limit, std::string_view error_msg, size_t digits_min, size_t digits_max = 0) {
   for (size_t i = 0; i < std::max(digits_min, digits_max); i++) {
      if (iter == limit)
         throw RuntimeException(InvalidTimestampFormat, fmt::format("invalid {} string, string to short", error_msg));
      char c = *(iter++);
      if (c == '-') break;
      if ((c >= '0') && (c <= '9')) {
         val = 10 * val + (c - '0');
      } else if (i >= digits_min) {
         --iter;
         break;
      } else {
         throw RuntimeException(InvalidTimestampFormat, fmt::format("invalid {} string, invalid character {}", error_msg, c));
      }
   }
   return iter;
}
// ---------------------------------------------------------------------------------------------------
std::pair<uint64_t, uint64_t> castTimestampString(std::string_view format_string, const char* str, uint32_t strLen) {
   auto c_in = str, c_in_limit = str + strLen;
   auto c_fs = format_string.cbegin(), c_fs_limit = format_string.cbegin() + format_string.size();

   size_t year = 0, month = 0, day = 0;
   size_t hour = 0, minute = 0, second = 0, milliseconds = 0, timezone = 0;

   // Trim WS
   while ((c_in != c_in_limit) && ((*c_in) == ' ')) ++c_in;
   while ((c_in != c_in_limit) && ((*(c_in_limit - 1)) == ' ')) --c_in_limit;

   //   if ((strLen == 4) && (strncmp(str, "NULL", 4) == 0))
   //      return null();

   while (c_fs != c_fs_limit) {
      const char* cn_fs = c_fs + 1;

      if (*c_fs == '%') {
         switch (*cn_fs) {
            case '\0':
               throw RuntimeException(InvalidTimestampFormat, "invalid timestamp format string");
            case '%':
               c_in = readChar(c_in, c_in_limit, '%');
               break;
            case 'Y':
               c_in = processNumber(year, c_in, c_in_limit, "year", 0, 4);
               break;
            case 'm':
               c_in = processNumber(month, c_in, c_in_limit, "month", 2);
               break;
            case 'd':
               c_in = processNumber(day, c_in, c_in_limit, "day", 2);
               break;
            case 'H':
               c_in = processNumber(hour, c_in, c_in_limit, "hour", 1, 2);
               break;
            case 'M':
               c_in = processNumber(minute, c_in, c_in_limit, "minute", 2);
               break;
            case 'S':
               c_in = processNumber(second, c_in, c_in_limit, "second", 2);
               if (c_in != c_in_limit && *c_in == '.')
                  c_in = processNumber(milliseconds, ++c_in, c_in_limit, "milliseconds", 3);
               break;
            case 'Z':
               c_in = processNumber(timezone, c_in, c_in_limit, "timezone", 2);
               break;
               timezone *= 60;
               c_in = readChar(c_in, c_in_limit, ':');
               c_in = processNumber(timezone, c_in, c_in_limit, "timezone", 3);
               break;
            default:
               throw RuntimeException(InvalidTimestampFormat, fmt::format("%{} currently not implemented", *cn_fs));
         }
         ++c_fs;
      } else {
         c_in = readChar(c_in, c_in_limit, *c_fs);
      }
      // ++c_in;
      ++c_fs;
   }

   // Range check day
   if ((year > 9999) || (month < 1) || (month > 12) || (day < 1) || (day > 31))
      throw RuntimeException(InvalidTimestampFormat, "out of range");

   uint64_t date = mergeJulianDay(year, month, day);

   // Range check time
   if ((hour >= 24) || (minute >= 60) || (second >= 60) || (milliseconds >= 1000))
      throw std::runtime_error("invalid timestamp format");
   uint64_t time = mergeTime(hour, minute, second, milliseconds);

   return {date, time};
}
// ---------------------------------------------------------------------------------------------------
} // anonymous namespace
// ---------------------------------------------------------------------------------------------------
Timestamp Timestamp::castString(const char* str, uint32_t strLen) {
   Timestamp result;
   auto [date, time] = castTimestampString("%Y-%m-%d"sv, str, strLen);
   result.value = (date * msPerDay) + time;
   return result;
}
// ---------------------------------------------------------------------------------------------------
Timestamp Timestamp::castString(std::string_view sv, std::string_view format_string) {
   Timestamp result;
   auto [date, time] = castTimestampString(format_string, sv.cbegin(), sv.length());
   result.value = (date * msPerDay) + time;
   return result;
}
// ---------------------------------------------------------------------------------------------------
// NULL
Timestamp Timestamp::null() {
   Timestamp result;
   result.value = 0;
   return result;
}
// ---------------------------------------------------------------------------------------------------
RuntimeValue Timestamp::toRTV() const {
   RuntimeValue rv;
   rv.setType(schemac::Type::Timestamp());
   rv.set(value);
   return rv;
}
// ---------------------------------------------------------------------------------------------------
Timestamp Timestamp::fromRTV(const RuntimeValue& rtv) {
   assert(rtv.getType() == schemac::Type::Timestamp());
   uint64_t val = rtv.get<uint64_t>();
   return Timestamp(val);
}
// ---------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Timestamp& value) {
   if (value == Timestamp::null()) {
      out << "NULL";
   }

   unsigned year, month, day;
   splitJulianDay(value.value / msPerDay, year, month, day);
   unsigned hour, minute, second, ms;
   splitTime(value.value % msPerDay, hour, minute, second, ms);

   char buffer[50]; // NOLINT
   if (ms) {
      snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u %u:%02u:%02u.%03u", year, month, day, hour, minute, second, ms);
   } else {
      snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u %u:%02u:%02u", year, month, day, hour, minute, second);
   }
   return out << buffer;
}
// ---------------------------------------------------------------------------------------------------
Date Date::castString(const char* str, uint32_t strLen) {
   Date result;
   auto [date, time] = castTimestampString("%Y-%m-%d"sv, str, strLen);
   assert(time == 0);
   result.value = (date * msPerDay);
   return result;
}
// ---------------------------------------------------------------------------------------------------
Date Date::castString(std::string_view sv, std::string_view format_string) {
   Date result;
   auto [date, time] = castTimestampString(format_string, sv.cbegin(), sv.length());
   if (time != 0)
      throw RuntimeException(ErrorCode::InvalidTimestampFormat, "Date cannot have a time component");
   result.value = (date * msPerDay) + time;
   return result;
}
// ---------------------------------------------------------------------------------------------------
// NULL
Date Date::null() {
   Date result;
   result.value = 0;
   return result;
}
// ---------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const Date& value) {
   if (value == Date::null()) {
      out << "NULL";
   }

   unsigned year, month, day;
   splitJulianDay(value.value / msPerDay, year, month, day);

   char buffer[50]; // NOLINT
   snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u", year, month, day);
   return out << buffer;
}
// ---------------------------------------------------------------------------------------------------
RuntimeValue Date::toRTV() const {
   RuntimeValue rv;
   rv.setType(schemac::Type::Date());
   rv.set(value);
   return rv;
}
// ---------------------------------------------------------------------------------------------------
Date Date::fromRTV(const RuntimeValue& rtv) {
   assert(rtv.getType() == schemac::Type::Date());
   uint64_t val = rtv.get<uint64_t>();
   return Date(val);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------