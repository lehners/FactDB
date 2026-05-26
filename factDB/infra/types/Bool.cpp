// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/types/Bool.hpp"
#include "factDB/infra/RuntimeException.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
RuntimeValue Bool::toRTV() const {
   RuntimeValue rtv;
   rtv.setType(schemac::Type::Bool());
   rtv.set<bool>(value);
   return rtv;
}
// ---------------------------------------------------------------------------------------------------
Bool Bool::fromRTV(const factDB::RuntimeValue& rtv) {
   assert(rtv.getType() == schemac::Type::Bool());
   return Bool(rtv.get<bool>());
}
// ---------------------------------------------------------------------------------------------------
Bool Bool::castString(const char* str, uint32_t strLen) {
   if (strLen == 1) {
      switch (*str) {
         case '1':
         case 't':
         case 'T':
            return Bool(true);
         case '0':
         case 'f':
         case 'F':
            return Bool(false);
         default:
            throw RuntimeException(InvalidBoolFormat, std::string(str));
      }
   } else {
      std::string content(str);
      if (content == "true") {
         return Bool(true);
      } else if (content == "false") {
         return Bool(false);
      } else {
         throw RuntimeException(InvalidBoolFormat, std::string(str));
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------