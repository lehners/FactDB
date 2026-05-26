// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/util/StringOperations.hpp"
#include <algorithm>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
inline uint64_t Hash::hash(const void* data, uint64_t dataLength, uint64_t seed) noexcept {
   // Hash a byte range. Based on the Go standard library implementation of wyhash https://github.com/golang/go/blob/1a09d57de58a90987789ef637083aac21533eeb7/src/runtime/hash64.go
   // Our k4 is different from Go's constant as Go's constant 0x1d8e4e27c47d124f resulted in an unlucky hash with many leading 0s when hashing empty objects
   auto reader = static_cast<const char*>(data);

   uint64_t a, b;
   seed = seed ^ k0;

   if (dataLength > 8) {
      if (dataLength <= 16) {
         a = unalignedLoad<uint64_t>(reader);
         b = unalignedLoad<uint64_t>(reader + dataLength - 8);
      } else {
         auto l = dataLength;
         if (l > 48) {
            uint64_t see1 = seed, see2 = seed;
            for (; l > 48; l -= 48, reader += 48) {
               seed = mulFold(unalignedLoad<uint64_t>(reader) ^ k1,
                              unalignedLoad<uint64_t>(reader + 8) ^ seed);
               see1 = mulFold(unalignedLoad<uint64_t>(reader + 16) ^ k2,
                              unalignedLoad<uint64_t>(reader + 24) ^ see1);
               see2 = mulFold(unalignedLoad<uint64_t>(reader + 32) ^ k3,
                              unalignedLoad<uint64_t>(reader + 40) ^ see2);
            }
            seed ^= see1 ^ see2;
         }
         for (; l > 16; l -= 16, reader += 16)
            seed = mulFold(unalignedLoad<uint64_t>(reader) ^ k1, unalignedLoad<uint64_t>(reader + 8) ^ seed);
         a = unalignedLoad<uint64_t>(std::min(reader, reader + l - 8));
         b = unalignedLoad<uint64_t>(reader + l - 8);
      }
   } else {
      a = b = StringOperations::read8(reader, dataLength);
   }
   auto result = mulFold(k4 ^ dataLength, mulFold(a ^ k1, b ^ seed));
   return result;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
