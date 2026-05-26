#ifndef H_FACTDB_INFRA_UTIL_HASH_HPP
#define H_FACTDB_INFRA_UTIL_HASH_HPP
// ---------------------------------------------------------------------------------------------------
#include <cstdint>
#include <string>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB {

// simple hasher for std::unordered maps
struct HashTuple {
   template <typename... Ts>
   size_t operator()(const std::tuple<Ts...>& tuple) const {
      return hash_tuple_impl(tuple, std::index_sequence_for<Ts...>{});
   }

   private:
   template <typename Tuple, size_t... Is>
   static size_t hash_tuple_impl(const Tuple& tuple, std::index_sequence<Is...>) {
      size_t seed = 0;
      (..., hash_combine(seed, std::get<Is>(tuple)));
      return seed;
   }

   template <typename T>
   static void hash_combine(size_t& seed, const T& value) {
      seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
   }
};

class Hash {
   private:
   /// Pseudorandom "secret" constants from wyhash
   static constexpr uint64_t k0 = 0xa0761d6478bd642full;
   static constexpr uint64_t k1 = 0xe7037ed1a0b428dbull;
   static constexpr uint64_t k2 = 0x8ebc6af09c88c6e3ull;
   static constexpr uint64_t k3 = 0x589965cc75374cc3ull;
   static constexpr uint64_t k4 = k3;

   /// Full 128 bit multiplication folded into 64bit
   constexpr static inline uint64_t mulFold(uint64_t a, uint64_t b) noexcept {
      auto m = static_cast<unsigned __int128>(a) * static_cast<unsigned __int128>(b);
      return static_cast<uint64_t>(m >> 64) ^ static_cast<uint64_t>(m);
   }

   public:
   static uint64_t inline hash(const void* data, uint64_t dataLength, uint64_t seed) noexcept;

   static uint64_t hashString(std::string& str, uint64_t seed = 0) { return hash(str.data(), str.length(), seed); }
   static uint64_t hashString(const std::string_view& str, uint64_t seed = 0) { return hash(str.data(), str.length(), seed); }
   static uint64_t hashInt64(int64_t val, uint64_t seed = 0) { return hash(&val, sizeof(int64_t), seed); }
   static uint64_t hashUInt64(uint64_t val, uint64_t seed = 0) { return hash(&val, sizeof(int64_t), seed); }

   template <typename... T>
   static uint64_t hashTuple(const std::tuple<T...>& tuple, uint64_t seed = 0) { return hash(static_cast<const void*>(&tuple), sizeof(std::tuple<T...>), seed); }

   template <typename... T>
   struct HashTuple {
      size_t operator()(const std::tuple<T...>& tuple) const {
         return hashTuple(tuple);
      }
   };
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/util/Hash.tpp"
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_INFRA_UTIL_HASH_HPP
// ---------------------------------------------------------------------------------------------------
