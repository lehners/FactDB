// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/BitSet.hpp"
#include <bitset>
#include <iostream>
#include <sstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra {
// ---------------------------------------------------------------------------------------------------
BitSet64::BitSet64(std::initializer_list<unsigned int> elements) noexcept {
   for (unsigned i : elements) {
      insert(i);
   }
}
// ---------------------------------------------------------------------------------------------------
BitSet64::BitSet64(unsigned begin, unsigned end, RangeEnum) noexcept {
   assert(begin < capacity && end <= capacity);
   assert(begin <= end);
   set = (1ull << end) - (1ull << begin);
}
// ---------------------------------------------------------------------------------------------------
void BitSet64::insert(unsigned int val) noexcept {
   assert(val < capacity);
   set |= 1ull << val;
}
// ---------------------------------------------------------------------------------------------------
std::string BitSet64::toBitString() const noexcept {
   std::stringstream ss;
   ss << std::bitset<8>(set >> 48) << " "
      << std::bitset<8>(set >> 40) << " "
      << std::bitset<8>(set >> 32) << " "
      << std::bitset<8>(set >> 24) << " "
      << std::bitset<8>(set >> 16) << " "
      << std::bitset<8>(set >> 8) << " "
      << std::bitset<8>(set >> 0);
   return ss.str();
}
// ---------------------------------------------------------------------------------------------------
BitSet64 BitSet64::nextPermutation(size_t relations) {
   if (set < BitSet64(0, relations, RangeEnum::Range).set)
      return {set + 1, FromSet::FromSet};
   else
      return {};
}
// ---------------------------------------------------------------------------------------------------
void BitSet64::print() noexcept {
   std::cout << std::bitset<64>(set) << std::endl;
}
// ---------------------------------------------------------------------------------------------------
BitSet64& BitSet64::operator+=(const BitSetVar& other) noexcept {
   assert(other.sets.size() == 1);
   return *this += other.sets.front();
}
// ---------------------------------------------------------------------------------------------------
BitSetVar::BitSetVar(size_t size, std::initializer_list<unsigned> element) noexcept : sets((size >> 6) + 1) {
   for (auto& i : element)
      insert(i);
}
// ---------------------------------------------------------------------------------------------------
std::pair<size_t, size_t> BitSetVar::calcSlot(unsigned val) const {
   assert(val < (sets.size() * BitSet64::capacity));
   static_assert(BitSet64::capacity == 1ull << 6);
   size_t slot = val >> 6;
   size_t valInSlot = val - (slot << 6);
   return {slot, valInSlot};
}
// ---------------------------------------------------------------------------------------------------
void BitSetVar::insert(unsigned val) noexcept {
   auto [slot, valInSlot] = calcSlot(val);
   sets[slot].insert(valInSlot);
}
// ---------------------------------------------------------------------------------------------------
void BitSetVar::clear() noexcept {
   for (auto& s : sets)
      s.clear();
}
// ---------------------------------------------------------------------------------------------------
bool BitSetVar::count(unsigned int val) const noexcept {
   auto [slot, valInSlot] = calcSlot(val);
   return sets[slot].count(valInSlot);
}
// ---------------------------------------------------------------------------------------------------
bool BitSetVar::empty() const noexcept {
   for (auto& s : sets)
      if (!s.empty())
         return false;
   return true;
}
// ---------------------------------------------------------------------------------------------------
size_t BitSetVar::size() const noexcept {
   size_t res = 0;
   for (auto& s : sets)
      res += s.size();
   return res;
}
// ---------------------------------------------------------------------------------------------------
bool BitSetVar::isSubsetOf(const factDB::infra::BitSetVar& superSet) const noexcept {
   for (size_t idx = 0; idx != sets.size(); ++idx) {
      if (!sets[idx].isSubsetOf(superSet.sets[idx]))
         return false;
   }
   return true;
}
// ---------------------------------------------------------------------------------------------------
bool BitSetVar::doesIntersectWith(const factDB::infra::BitSetVar& other) const noexcept {
   for (size_t idx = 0; idx != sets.size(); ++idx)
      if (!sets[idx].doesIntersectWith(other.sets[idx]))
         return false;
   return true;
}
// ---------------------------------------------------------------------------------------------------
BitSetVar BitSetVar::operator+(const factDB::infra::BitSetVar& other) const noexcept {
   BitSetVar ret = *this;
   for (size_t idx = 0; idx != sets.size(); ++idx)
      ret.sets[idx] = sets[idx] + other.sets[idx];
   return ret;
}
// ---------------------------------------------------------------------------------------------------
BitSetVar BitSetVar::operator&(const factDB::infra::BitSetVar& other) const noexcept {
   BitSetVar ret = *this;
   for (size_t idx = 0; idx != sets.size(); ++idx)
      ret.sets[idx] = sets[idx] & other.sets[idx];
   return ret;
}
// ---------------------------------------------------------------------------------------------------
BitSetVar BitSetVar::operator-(const factDB::infra::BitSetVar& other) const noexcept {
   BitSetVar ret = *this;
   for (size_t idx = 0; idx != sets.size(); ++idx)
      ret.sets[idx] = sets[idx] - other.sets[idx];
   return ret;
}
// ---------------------------------------------------------------------------------------------------
BitSetVar& BitSetVar::operator+=(const factDB::infra::BitSetVar& other) noexcept {
   for (size_t idx = 0; idx != sets.size(); ++idx)
      sets[idx] += other.sets[idx];
   return *this;
}
// ---------------------------------------------------------------------------------------------------
BitSetVar& BitSetVar::operator-=(const factDB::infra::BitSetVar& other) noexcept {
   for (size_t idx = 0; idx != sets.size(); ++idx)
      sets[idx] -= other.sets[idx];
   return *this;
}
// ---------------------------------------------------------------------------------------------------
BitSetVar& BitSetVar::operator&=(const factDB::infra::BitSetVar& other) noexcept {
   for (size_t idx = 0; idx != sets.size(); ++idx)
      sets[idx] &= other.sets[idx];
   return *this;
}
// ---------------------------------------------------------------------------------------------------
bool BitSet64::compareNumeric(const factDB::infra::BitSet64& other) const noexcept {
   // returns true if this set is smaller than the set other.
   // https://stackoverflow.com/a/2603254
   // clang-format off
   static unsigned char lookup[16] = { 0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe, 0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };
   // clang-format on

   auto reverseByte = [](uint8_t n) {
      // Reverse the top and bottom nibble then swap them.
      return (lookup[n & 0b1111] << 4) | lookup[n >> 4];
   };

   auto reverseUInt64 = [&](uint64_t n) {
      uint64_t reversed = 0;
      for (size_t i = 0; i != sizeof(uint64_t); ++i)
         reversed = (reversed << 8) + reverseByte(n >> (8 * i));
      return reversed;
   };
   return reverseUInt64(set) > reverseUInt64(other.set);
}
// ---------------------------------------------------------------------------------------------------
bool BitSetVar::compareNumeric(const BitSetVar& other) const noexcept {
   // returns true if this set is smaller than the set other.
   assert(sets.size() == other.sets.size());
   for (size_t idx = 0; idx != sets.size(); ++idx) {
      const auto &thisSet = sets[idx], otherSet = other.sets[idx];
      if (thisSet.compareNumeric(otherSet))
         return true;
      else if (otherSet.compareNumeric(thisSet))
         return false;
   }
   return false;
}
// ---------------------------------------------------------------------------------------------------
std::string BitSetVar::toBitString() const noexcept {
   std::stringstream ss;
   for (size_t idx = 0; idx < sets.size(); ++idx)
      ss << sets[idx].toBitString() << (idx != sets.size() ? "|" : "");
   return ss.str();
}
// ---------------------------------------------------------------------------------------------------
BitSetVar::iterator& BitSetVar::iterator::operator++() noexcept {
   if (~pos) {
      unsigned l = set->sets.size();
      ++pos;
      while (true) {
         unsigned word = pos / 64, ofs = pos % 64;
         if (word >= l) {
            pos = ~0u;
            break;
         }
         uint64_t remaining = set->sets[word].set >> ofs;
         if (!remaining) {
            pos += 64 - ofs;
            continue;
         }
         pos += __builtin_ctzll(remaining);
         break;
      }
   }
   return *this;
}
// ---------------------------------------------------------------------------------------------------
BitSetVar::iterator BitSetVar::begin() const noexcept {
   if (empty()) {
      return end();
   } else {
      iterator iter(this, 0);
      if (!(sets.front().set & 1))
         ++iter;
      return iter;
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra
// ---------------------------------------------------------------------------------------------------