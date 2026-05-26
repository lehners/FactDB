// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/types/Integer.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include "factDB/infra/types/TypeUtils.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
template <std::signed_integral T>
RuntimeValue toRTVHelper(T& value, schemac::Type type) {
   RuntimeValue rtv;
   rtv.setType(std::move(type));
   rtv.set(value);
   return rtv;
}
// ---------------------------------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------------------------------
RuntimeValue Integer::toRTV() const {
   return toRTVHelper(value, schemac::Type::Integer());
}
// ---------------------------------------------------------------------------------------------------
Integer Integer::fromRTV(const factDB::RuntimeValue& rtv) {
   assert(rtv.getType() == schemac::Type::Integer());
   return Integer(rtv.get<int32_t>());
}
// ---------------------------------------------------------------------------------------------------
Int16 Int16::fromRTV(const factDB::RuntimeValue& rtv) {
   // assert(rtv.getType() == schemac::Type::Integer());
   return Int16(rtv.get<int16_t>());
}
// ---------------------------------------------------------------------------------------------------
Int32 Int32::fromRTV(const factDB::RuntimeValue& rtv) {
   // assert(rtv.getType() == schemac::Type::Integer());
   return Int32(rtv.get<int32_t>());
}
// ---------------------------------------------------------------------------------------------------
Int64 Int64::fromRTV(const factDB::RuntimeValue& rtv) {
   // assert(rtv.getType() == schemac::Type::Integer());
   return Int64(rtv.get<int64_t>());
}
// ---------------------------------------------------------------------------------------------------
Int64 Int64::castString(const char* str, uint32_t strLen) {
   return Int64(castStringToIntegral<IntType>(str, strLen));
}
// ---------------------------------------------------------------------------------------------------
Int64 Int64::castString(std::string_view sv) {
   return Int64(castStringToIntegral<IntType>(sv));
}
// ---------------------------------------------------------------------------------------------------
Int32 Int32::castString(const char* str, uint32_t strLen) {
   return Int32(castStringToIntegral<IntType>(str, strLen));
}
// ---------------------------------------------------------------------------------------------------
Int32 Int32::castString(std::string_view sv) {
   return Int32(castStringToIntegral<IntType>(sv));
}
// ---------------------------------------------------------------------------------------------------
Int16 Int16::castString(const char* str, uint32_t strLen) {
   return Int16(castStringToIntegral<IntType>(str, strLen));
}
// ---------------------------------------------------------------------------------------------------
Int16 Int16::castString(std::string_view sv) {
   return Int16(castStringToIntegral<IntType>(sv));
}
// ---------------------------------------------------------------------------------------------------
Integer Integer::castString(const char* str, uint32_t strLen) {
   return Integer(castStringToIntegral<IntType>(str, strLen));
}
// ---------------------------------------------------------------------------------------------------
Integer Integer::castString(std::string_view sv) {
   return Integer(castStringToIntegral<IntType>(sv));
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------