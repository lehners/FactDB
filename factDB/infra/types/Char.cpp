// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/types/Char.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include "factDB/infra/types/TypeUtils.hpp"
#include "factDB/infra/util/Hash.hpp"
#include "factDB/schemac/Type.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
template <unsigned kMaxLen>
Char<kMaxLen> Char<kMaxLen>::fromRTV(const factDB::RuntimeValue& rtv) {
   assert(rtv.getType() == schemac::Type::Char(kMaxLen));
   RuntimeString rts = rtv.get<RuntimeString>();
   Char<kMaxLen> v;
   v.len = std::min(static_cast<uint64_t>(kMaxLen), rts.getLength());
   std::memcpy(v.value, rts.getPointer(), v.len);
   return v;
}
// ---------------------------------------------------------------------------------------------------
RuntimeValue Char<1>::toRTV() const {
   RuntimeValue rtv;
   rtv.setType(schemac::Type::Char(1));
   rtv.set(value);
   return rtv;
}
//---------------------------------------------------------------------------
Char<1> Char<1>::fromRTV(const RuntimeValue& rtv) {
   assert(rtv.getType() == schemac::Type::Char(1));
   Char<1> c;
   c.value = rtv.get<char>();
   return c;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------