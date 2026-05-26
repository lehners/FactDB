// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/types/Numeric.hpp"

#include "TypeUtils.hpp"
#include "factDB/infra/RuntimeException.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <charconv>
#include <cmath>
#include <cstdlib>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------

RuntimeValue Numeric18_2::toRTV() const {
   RuntimeValue rtv;
   rtv.setType(schemac::Type::Numeric(18, 2));
   rtv.set<int64_t>(value); // store raw scaled value
   return rtv;
}
// ---------------------------------------------------------------------------------------------------
Numeric18_2 Numeric18_2::fromRTV(const RuntimeValue& rtv) {
   assert(rtv.getType() == schemac::Type::Numeric(18, 2));
   return Numeric18_2(rtv.get<int64_t>(), true);
}
// ---------------------------------------------------------------------------------------------------
Numeric18_2 Numeric18_2::castString(const char* str, uint32_t strLen) {
   auto iter = str, limit = str + strLen;

   // Trim WS
   while ((iter != limit) && ((*iter) == ' ')) ++iter;
   while ((iter != limit) && ((*(limit - 1)) == ' ')) --limit;

   // Check for a sign
   bool neg = false;
   if (iter != limit) {
      if ((*iter) == '-') {
         neg = true;
         ++iter;
      } else if ((*iter) == '+') {
         ++iter;
      }
   }

   // Parse
   if (iter == limit)
      throw factDB::RuntimeException(InvalidNumberFormat, "found non-integer characters");

   int64_t result = 0;
   unsigned digitsSeen = 0;
   unsigned digitsSinceComma = 0;
   bool commaFound = false;
   for (; iter != limit; ++iter) {
      ++digitsSinceComma;
      char c = *iter;
      if ((c >= '0') && (c <= '9')) {
         result = (result * 10) + (c - '0');
         ++digitsSeen;
      } else if (c == '.') {
         if (commaFound != 0)
            throw factDB::RuntimeException(InvalidNumberFormat, "saw multiple commas");
         commaFound = true;
         digitsSinceComma = 0;
      } else {
         throw factDB::RuntimeException(InvalidNumberFormat, "invalid character in integer string");
      }
   }

   if (digitsSinceComma == 0 || !commaFound)
      result *= 100;
   else if (digitsSinceComma == 1)
      result *= 10;
   else if (digitsSinceComma > 2)
      throw factDB::RuntimeException(InvalidNumberFormat, "too many digits after comma");

   size_t max_digits = 20;
   if (digitsSeen > max_digits) {
      throw factDB::RuntimeException(InvalidNumberFormat, fmt::format("too many characters (64 bit numerics can at most consist of {} numeric characters))", max_digits));
   }

   return Numeric18_2(neg ? -result : result, true);
}

// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
