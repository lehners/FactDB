// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/types/Varchar.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include "factDB/infra/util/Hash.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
// Hash
template <unsigned kMaxLen>
uint64_t Varchar<kMaxLen>::hash() const {
   return Hash::hashString(value, kMaxLen);
}
// ---------------------------------------------------------------------------------------------------
// Comparison
template <unsigned kMaxLen>
bool Varchar<kMaxLen>::operator<(const Varchar& other) const {
   int c = memcmp(value, other.value, std::min(len, other.len));
   if (c < 0) return true;
   if (c > 0) return false;
   return len < other.len;
}
// ---------------------------------------------------------------------------------------------------
// Comparison
template <unsigned kMaxLen>
bool Varchar<kMaxLen>::operator>(const Varchar& other) const {
   int c = memcmp(value, other.value, std::min(len, other.len));
   if (c < 0) return false;
   if (c > 0) return true;
   return len > other.len;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
