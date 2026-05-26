// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/types/UInt.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include "factDB/infra/types/TypeUtils.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
template <std::unsigned_integral T>
RuntimeValue toRTVHelper(T& value, schemac::Type type) {
   RuntimeValue rtv;
   rtv.setType(std::move(type));
   rtv.set(value);
   return rtv;
}
// ---------------------------------------------------------------------------------------------------
RuntimeValue UInt64::toRTV() const {
   return toRTVHelper(value, schemac::Type::UInt64());
}
// ---------------------------------------------------------------------------------------------------
UInt64 UInt64::fromRTV(const factDB::RuntimeValue& rtv) {
   assert(rtv.getType() == schemac::Type::UInt64());
   return UInt64(rtv.get<int32_t>());
}
// ---------------------------------------------------------------------------------------------------
UInt32 UInt32::fromRTV(const factDB::RuntimeValue& rtv) {
   // assert(rtv.getType() == schemac::Type::Integer());
   return UInt32(rtv.get<int16_t>());
}
// ---------------------------------------------------------------------------------------------------
UInt16 UInt16::fromRTV(const factDB::RuntimeValue& rtv) {
   // assert(rtv.getType() == schemac::Type::Integer());
   return UInt16(rtv.get<int32_t>());
}
// ---------------------------------------------------------------------------------------------------
UInt16 UInt16::castString(const char* str, uint32_t strLen) {
   return UInt16(castStringToIntegral<UIntType>(str, strLen));
}
// ---------------------------------------------------------------------------------------------------
UInt16 UInt16::castString(std::string_view sv) {
   return UInt16(castStringToIntegral<UIntType>(sv));
}
// ---------------------------------------------------------------------------------------------------
UInt32 UInt32::castString(const char* str, uint32_t strLen) {
   return UInt32(castStringToIntegral<UIntType>(str, strLen));
}
// ---------------------------------------------------------------------------------------------------
UInt32 UInt32::castString(std::string_view sv) {
   return UInt32(castStringToIntegral<UIntType>(sv));
}
// ---------------------------------------------------------------------------------------------------
UInt64 UInt64::castString(const char* str, uint32_t strLen) {
   return UInt64(castStringToIntegral<UIntType>(str, strLen));
}
// ---------------------------------------------------------------------------------------------------
UInt64 UInt64::castString(std::string_view sv) {
   return UInt64(castStringToIntegral<UIntType>(sv));
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------