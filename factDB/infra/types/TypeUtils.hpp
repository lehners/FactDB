#ifndef H_FACTDB_FACT_DB_INFRA_TYPES_TYPEUTILS_HPP
#define H_FACTDB_FACT_DB_INFRA_TYPES_TYPEUTILS_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/RuntimeException.hpp"
#include "fmt/format.h"
#include <math.h>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
template <std::integral T>
T castStringToIntegral(const char* str, uint32_t strLen) {
   auto iter = str, limit = str + strLen;

   // Trim WS
   while ((iter != limit) && ((*iter) == ' ')) ++iter;
   while ((iter != limit) && ((*(limit - 1)) == ' ')) --limit;

   // Check for a sign
   bool neg = false;
   if (iter != limit) {
      if constexpr (std::is_signed<T>::value) {
         if ((*iter) == '-') {
            neg = true;
            ++iter;
         } else if ((*iter) == '+') {
            ++iter;
         }
      } else {
         if ((*iter) == '-' || (*iter) == '+')
            throw factDB::RuntimeException(InvalidNumberFormat, "found sign for UInt type");
      }
   }

   // Parse
   if (iter == limit)
      throw factDB::RuntimeException(InvalidNumberFormat, "found non-integer characters");

   T result = 0;
   unsigned digitsSeen = 0;
   for (; iter != limit; ++iter) {
      char c = *iter;
      if ((c >= '0') && (c <= '9')) {
         result = (result * 10) + (c - '0');
         ++digitsSeen;
      } else if (c == '.') {
         break;
      } else {
         throw factDB::RuntimeException(InvalidNumberFormat, "invalid character in integer string");
      }
   }

   size_t max_digits = 0;
   switch (sizeof(T)) {
      case 2: max_digits = 5; break;
      case 4: max_digits = 10; break;
      case 8: max_digits = 20; break;
   }
   if (digitsSeen > max_digits) {
      throw factDB::RuntimeException(InvalidNumberFormat, fmt::format("too many characters ({} bit integers can at most consist of {} numeric characters))", sizeof(T) * 8, max_digits));
   }

   return neg ? -result : result;
}
// ---------------------------------------------------------------------------------------------------
template <std::integral T>
T castStringToIntegral(std::string_view sv) {
   return castStringToIntegral<T>(sv.cbegin(), sv.length());
}
// ---------------------------------------------------------------------------------------------------
/*template <unsigned size>
struct LengthSwitch {};
template <>
struct LengthSwitch<1> {
   using type = uint8_t;
};
template <>
struct LengthSwitch<2> {
   using type = uint16_t;
};
template <>
struct LengthSwitch<4> {
   using type = uint32_t;
};*/
// ---------------------------------------------------------------------------------------------------
template <unsigned kMaxLen>
struct LengthIndicator {
   using type = uint64_t;
   // typename LengthSwitch<((kMaxLen < 256) ? 1 : (kMaxLen < 65536) ? 2 : 4)>::type;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACT_DB_INFRA_TYPES_TYPEUTILS_HPP
