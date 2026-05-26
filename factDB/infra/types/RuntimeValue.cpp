#include "factDB/infra/types/RuntimeValue.hpp"
#include "../Config.hpp"
#include "factDB/infra/RuntimeException.hpp"
#include "factDB/infra/Types.hpp"
#include "factDB/infra/util/Hash.hpp"
#include "fmt/format.h"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
RuntimeValue::RuntimeValue(const RuntimeValue& other) : rawValue(other.rawValue), type(other.type) {
   if (other.stringValue) {
      setString(other.get<RuntimeString>().getStringView());
   }
}
// ---------------------------------------------------------------------------------------------------
RuntimeValue& RuntimeValue::operator=(const RuntimeValue& other) {
   if (this != &other) {
      if (other.stringValue) {
         setString(other.get<RuntimeString>().getStringView());
      } else {
         rawValue = other.rawValue;
         type = other.type;
      }
   }
   return *this;
}
// ---------------------------------------------------------------------------------------------------
void RuntimeValue::setString(std::string_view input) {
   stringValue = std::make_unique<char[]>(input.size());
   strncpy(stringValue.get(), input.data(), input.size());
   RuntimeString runtimeString;
   runtimeString.len = input.size();
   runtimeString.ptr = stringValue.get();
   set(runtimeString);
}
// ---------------------------------------------------------------------------------------------------
uint64_t RuntimeValue::hash() const {
   switch (getType().tclass) {
      case schemac::Type::KUndefined: unreachable();
      case schemac::Type::KInteger: return Hash::hashTuple(std::make_tuple(Integer::fromRTV(*this)));
      case schemac::Type::KUInt64: return Hash::hashTuple(std::make_tuple(UInt64::fromRTV(*this)));
      case schemac::Type::KTimestamp: return Hash::hashTuple(std::make_tuple(Timestamp::fromRTV(*this)));
      case schemac::Type::KDate: return Hash::hashTuple(std::make_tuple(Date::fromRTV(*this)));
      case schemac::Type::KBool: return Hash::hashTuple(std::make_tuple(Bool::fromRTV(*this)));
      case schemac::Type::KNumeric: not_implemented();
      case schemac::Type::KChar:
      case schemac::Type::KVarchar:
         return Hash::hashString(get<RuntimeString>().getStringView());
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
bool RuntimeValue::operator==(const factDB::RuntimeValue& o) const {
   if (o.type != type)
      throw RuntimeException(RuntimeError, fmt::format("types {} and {} are not comparable for RuntimeValue", type.toString(), o.type.toString()));

   switch (o.type.tclass) {
      case schemac::Type::KUndefined: unreachable();
      case schemac::Type::KBool: return Bool::fromRTV(*this) == Bool::fromRTV(o);
      case schemac::Type::KInteger: return Integer::fromRTV(*this) == Integer::fromRTV(o);
      case schemac::Type::KUInt64: return UInt64::fromRTV(*this) == UInt64::fromRTV(o);
      case schemac::Type::KTimestamp: return Timestamp::fromRTV(*this) == Timestamp::fromRTV(o);
      case schemac::Type::KDate: return Date::fromRTV(*this) == Date::fromRTV(o);
      case schemac::Type::KNumeric: not_implemented();
      case schemac::Type::KChar:
      case schemac::Type::KVarchar:
         return get<RuntimeString>() == o.get<RuntimeString>();
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
bool RuntimeValue::operator<(const factDB::RuntimeValue& o) const {
   if (type != o.type)
      throw RuntimeException(RuntimeError, fmt::format("types {} and {} are not comparable for RuntimeValue", type.toString(), o.type.toString()));
   switch (o.type.tclass) {
      case schemac::Type::KUndefined: unreachable();
      case schemac::Type::KBool: return Bool::fromRTV(*this) < Bool::fromRTV(o);
      case schemac::Type::KInteger: return Integer::fromRTV(*this) < Integer::fromRTV(o);
      case schemac::Type::KUInt64: return UInt64::fromRTV(*this) < UInt64::fromRTV(o);
      case schemac::Type::KTimestamp: return Timestamp::fromRTV(*this) < Timestamp::fromRTV(o);
      case schemac::Type::KDate: return Date::fromRTV(*this) < Date::fromRTV(o);
      case schemac::Type::KNumeric: not_implemented();
      case schemac::Type::KChar:
      case schemac::Type::KVarchar:
         return get<RuntimeString>() < o.get<RuntimeString>();
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
bool RuntimeValue::operator>(const factDB::RuntimeValue& o) const {
   if (type != o.type)
      throw RuntimeException(RuntimeError, fmt::format("types {} and {} are not comparable for RuntimeValue", type.toString(), o.type.toString()));
   switch (o.type.tclass) {
      case schemac::Type::KUndefined: unreachable();
      case schemac::Type::KBool: return Bool::fromRTV(*this) > Bool::fromRTV(o);
      case schemac::Type::KInteger: return Integer::fromRTV(*this) > Integer::fromRTV(o);
      case schemac::Type::KUInt64: return UInt64::fromRTV(*this) > UInt64::fromRTV(o);
      case schemac::Type::KTimestamp: return Timestamp::fromRTV(*this) > Timestamp::fromRTV(o);
      case schemac::Type::KDate: return Date::fromRTV(*this) > Date::fromRTV(o);
      case schemac::Type::KNumeric: not_implemented();
      case schemac::Type::KChar:
      case schemac::Type::KVarchar:
         return get<RuntimeString>() > o.get<RuntimeString>();
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
bool RuntimeValue::operator<=(const factDB::RuntimeValue& o) const {
   if (type != o.type)
      throw RuntimeException(RuntimeError, fmt::format("types {} and {} are not comparable for RuntimeValue", type.toString(), o.type.toString()));
   switch (o.type.tclass) {
      case schemac::Type::KUndefined: unreachable();
      case schemac::Type::KBool: return Bool::fromRTV(*this) <= Bool::fromRTV(o);
      case schemac::Type::KInteger: return Integer::fromRTV(*this) <= Integer::fromRTV(o);
      case schemac::Type::KUInt64: return UInt64::fromRTV(*this) <= UInt64::fromRTV(o);
      case schemac::Type::KTimestamp: return Timestamp::fromRTV(*this) <= Timestamp::fromRTV(o);
      case schemac::Type::KDate: return Date::fromRTV(*this) <= Date::fromRTV(o);
      case schemac::Type::KNumeric: not_implemented();
      case schemac::Type::KChar:
      case schemac::Type::KVarchar:
         return get<RuntimeString>() <= o.get<RuntimeString>();
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
bool RuntimeValue::operator>=(const factDB::RuntimeValue& o) const {
   if (type != o.type)
      throw RuntimeException(RuntimeError, fmt::format("types {} and {} are not comparable for RuntimeValue", type.toString(), o.type.toString()));
   switch (o.type.tclass) {
      case schemac::Type::KUndefined: unreachable();
      case schemac::Type::KBool: return Bool::fromRTV(*this) >= Bool::fromRTV(o);
      case schemac::Type::KInteger: return Integer::fromRTV(*this) >= Integer::fromRTV(o);
      case schemac::Type::KUInt64: return UInt64::fromRTV(*this) >= UInt64::fromRTV(o);
      case schemac::Type::KTimestamp: return Timestamp::fromRTV(*this) >= Timestamp::fromRTV(o);
      case schemac::Type::KDate: return Date::fromRTV(*this) >= Date::fromRTV(o);
      case schemac::Type::KNumeric: not_implemented();
      case schemac::Type::KChar:
      case schemac::Type::KVarchar:
         return get<RuntimeString>() >= o.get<RuntimeString>();
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
Bool RuntimeValue::operator&&(const factDB::RuntimeValue& o) const {
   if (type != o.type)
      throw RuntimeException(RuntimeError, fmt::format("types {} and {} are not comparable for RuntimeValue", type.toString(), o.type.toString()));
   switch (o.type.tclass) {
      case schemac::Type::KUndefined: unreachable();
      case schemac::Type::KBool: return Bool::fromRTV(*this) && Bool::fromRTV(o);
      case schemac::Type::KInteger:
      case schemac::Type::KUInt64:
      case schemac::Type::KTimestamp:
      case schemac::Type::KDate:
      case schemac::Type::KNumeric:
      case schemac::Type::KChar:
      case schemac::Type::KVarchar:
         not_implemented();
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
Bool RuntimeValue::operator||(const factDB::RuntimeValue& o) const {
   if (type != o.type)
      throw RuntimeException(RuntimeError, fmt::format("types {} and {} are not comparable for RuntimeValue", type.toString(), o.type.toString()));
   switch (o.type.tclass) {
      case schemac::Type::KUndefined: unreachable();
      case schemac::Type::KBool: return Bool::fromRTV(*this) || Bool::fromRTV(o);
      case schemac::Type::KInteger:
      case schemac::Type::KUInt64:
      case schemac::Type::KTimestamp:
      case schemac::Type::KDate:
      case schemac::Type::KNumeric:
      case schemac::Type::KChar:
      case schemac::Type::KVarchar:
         not_implemented();
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const RuntimeValue& val) {
   switch (val.getType().tclass) {
      case schemac::Type::KUndefined: unreachable();
      case schemac::Type::KInteger: return out << Integer::fromRTV(val);
      case schemac::Type::KUInt64: return out << UInt64::fromRTV(val);
      case schemac::Type::KTimestamp: return out << Timestamp::fromRTV(val);
      case schemac::Type::KDate: return out << Date::fromRTV(val);
      case schemac::Type::KNumeric: not_implemented();
      case schemac::Type::KChar:
      case schemac::Type::KVarchar:
         return out << val.get<RuntimeString>().getStringView();
      case schemac::Type::KBool: return out << Bool::fromRTV(val);
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------