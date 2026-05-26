#ifndef H_FACTDB_FACTDB_INFRA_BITSET_HPP
#define H_FACTDB_FACTDB_INFRA_BITSET_HPP
// ---------------------------------------------------------------------------------------------------
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra {
// ---------------------------------------------------------------------------------------------------
class BitSetVar;
// ---------------------------------------------------------------------------------------------------
class BitSet64 {
   friend class BitSetVar;

   public:
   static constexpr size_t capacity = sizeof(uint64_t) * 8;
   enum class RangeEnum { Range };

   private:
   uint64_t set = 0;

   enum class FromSet { FromSet };
   constexpr BitSet64(uint64_t setParam, FromSet) : set(setParam) {}

   public:
   /// Constructor
   constexpr BitSet64() noexcept = default;
   /// Initializer
   explicit BitSet64(std::initializer_list<unsigned> elements) noexcept;
   explicit BitSet64(unsigned begin, unsigned end, RangeEnum) noexcept;
   explicit BitSet64(unsigned element) noexcept : BitSet64({element}) {}

   void insert(unsigned val) noexcept;
   constexpr void clear() noexcept { set = 0; }
   void print() noexcept;

   [[nodiscard]] constexpr bool count(unsigned v) const noexcept { return v < sizeof(uint64_t) * 8 ? set & (1ull << v) : false; }
   [[nodiscard]] constexpr bool contains(unsigned v) const noexcept { return count(v); }
   [[nodiscard]] constexpr bool empty() const noexcept { return set == 0; }
   [[nodiscard]] constexpr size_t size() const noexcept { return std::popcount(set); }

   [[nodiscard]] constexpr bool isSubsetOf(const BitSet64& superSet) const noexcept { return (set & superSet.set) == set; }
   [[nodiscard]] constexpr bool isSubsetNotEqualOf(const BitSet64& superSet) const noexcept { return (set & superSet.set) == set && superSet.set != set; }
   [[nodiscard]] constexpr bool doesIntersectWith(const BitSet64& other) const noexcept { return (set & other.set) != 0; }

   constexpr void swap(BitSet64& other) noexcept {
      set ^= other.set;
      other.set ^= set;
      set ^= other.set;
   }

   // union
   [[nodiscard]] constexpr BitSet64 operator+(const BitSet64& other) const noexcept { return {set | other.set, FromSet::FromSet}; }
   // intersection
   [[nodiscard]] constexpr BitSet64 operator&(const BitSet64& other) const noexcept { return {set & other.set, FromSet::FromSet}; }
   // difference
   [[nodiscard]] constexpr BitSet64 operator-(const BitSet64& other) const noexcept { return {set & (~other.set), FromSet::FromSet}; }

   constexpr BitSet64& operator+=(const BitSet64& other) noexcept {
      set |= other.set;
      return *this;
   }
   BitSet64& operator+=(const BitSetVar& other) noexcept;
   constexpr BitSet64& operator&=(const BitSet64& other) noexcept {
      set &= other.set;
      return *this;
   }
   constexpr BitSet64& operator-=(const BitSet64& other) noexcept {
      set &= ~other.set;
      return *this;
   }

   [[nodiscard]] bool operator==(const BitSet64& other) const noexcept { return set == other.set; }
   [[nodiscard]] bool operator!=(const BitSet64& other) const noexcept { return set != other.set; }

   [[nodiscard]] bool compareNumeric(const BitSet64& other) const noexcept;
   [[nodiscard]] std::string toBitString() const noexcept;

   [[nodiscard]] BitSet64 nextPermutation(size_t relations);

   struct hasher {
      constexpr std::size_t operator()(BitSet64 a) const noexcept { return a.set; }
   };

   struct iterator {
      private:
      friend class BitSet64;
      uint64_t set = 0;
      constexpr explicit iterator(uint64_t s) noexcept : set(s) {}

      public:
      constexpr iterator() noexcept = default;
      /// Access the element
      constexpr unsigned operator*() const noexcept { return std::countr_zero(set); }
      /// Advance to the next element
      constexpr iterator& operator++() noexcept {
         set &= (set - 1);
         return *this;
      }

      using difference_type = ptrdiff_t;
      using value_type = unsigned;

      [[nodiscard]] constexpr bool operator==(const iterator& o) const noexcept { return set == o.set; }
      [[nodiscard]] constexpr bool operator!=(const iterator& o) const noexcept { return set != o.set; }
   };

   [[nodiscard]] constexpr iterator begin() const noexcept { return iterator(set); }
   [[nodiscard]] constexpr iterator end() const noexcept { return iterator(0); }
   [[nodiscard]] constexpr unsigned front() const { return *begin(); }
   [[nodiscard]] constexpr unsigned back() const {
      assert(!empty());
      return capacity - 1 - std::countl_zero(set);
   }
};
// ---------------------------------------------------------------------------------------------------
class BitSetVar {
   friend class BitSet64;
   std::vector<BitSet64> sets;

   [[nodiscard]] std::pair<size_t, size_t> calcSlot(unsigned val) const;

   public:
   /// Initializer
   explicit BitSetVar(size_t size, std::initializer_list<unsigned> elements) noexcept;
   explicit BitSetVar(size_t size, unsigned element) noexcept : BitSetVar(size, {element}) {}
   explicit BitSetVar(size_t size) noexcept : BitSetVar(size, {}) {}

   void insert(unsigned val) noexcept;
   void clear() noexcept;

   [[nodiscard]] bool contains(unsigned v) const noexcept { return count(v); }
   [[nodiscard]] bool count(unsigned v) const noexcept;
   [[nodiscard]] bool empty() const noexcept;
   [[nodiscard]] size_t size() const noexcept;

   [[nodiscard]] bool isSubsetOf(const BitSetVar& superSet) const noexcept;
   [[nodiscard]] bool doesIntersectWith(const BitSetVar& other) const noexcept;

   // union
   [[nodiscard]] BitSetVar operator+(const BitSetVar& other) const noexcept;
   // intersection
   [[nodiscard]] BitSetVar operator&(const BitSetVar& other) const noexcept;
   // difference
   [[nodiscard]] BitSetVar operator-(const BitSetVar& other) const noexcept;

   BitSetVar& operator+=(const BitSetVar& other) noexcept;
   BitSetVar& operator&=(const BitSetVar& other) noexcept;
   BitSetVar& operator-=(const BitSetVar& other) noexcept;

   [[nodiscard]] bool operator==(const BitSetVar& other) const noexcept { return sets == other.sets; }
   [[nodiscard]] bool operator!=(const BitSetVar& other) const noexcept { return sets != other.sets; }
   [[nodiscard]] bool operator<(const BitSetVar& other) const noexcept { return isSubsetOf(other); }

   [[nodiscard]] bool compareNumeric(const BitSetVar& other) const noexcept;
   [[nodiscard]] std::string toBitString() const noexcept;

   class iterator {
      private:
      /// The set
      const BitSetVar* set = nullptr;
      /// The position
      unsigned pos = ~0u;

      constexpr iterator(const BitSetVar* s, unsigned p) noexcept : set(s), pos(p) {}
      friend class BitSetVar;

      public:
      constexpr iterator() noexcept = default;
      unsigned operator*() const noexcept { return pos; }
      iterator& operator++() noexcept;

      using difference_type = ptrdiff_t;
      using value_type = unsigned;

      constexpr bool operator==(const iterator& o) const noexcept { return pos == o.pos; }
      constexpr bool operator!=(const iterator& o) const noexcept { return pos != o.pos; }
   };

   [[nodiscard]] iterator begin() const noexcept;
   [[nodiscard]] iterator end() const noexcept { return {this, ~0u}; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_INFRA_BITSET_HPP
