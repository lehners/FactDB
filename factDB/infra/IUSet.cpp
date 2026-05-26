#include "factDB/infra/IUSet.hpp"
#include "factDB/infra/iu.hpp"
#include <iterator>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
std::size_t IUHash::operator()(const std::reference_wrapper<const IU>& iuRef) const {
   return std::hash<std::string_view>{}(iuRef.get().table) ^ std::hash<std::string>{}(iuRef.get().column);
}
// ---------------------------------------------------------------------------------------------------
bool IUEqual::operator()(const std::reference_wrapper<const IU>& lhs, const std::reference_wrapper<const IU>& rhs) const {
   return lhs.get() == rhs.get();
}
// ---------------------------------------------------------------------------------------------------
std::size_t IUPointerHash::operator()(const factDB::IU* iu) const {
   return std::hash<std::string_view>{}(iu->table) ^ std::hash<std::string>{}(iu->column);
}
// ---------------------------------------------------------------------------------------------------
bool IUPointerEqual::operator()(const factDB::IU* iu1, const factDB::IU* iu2) const {
   return (*iu1) == (*iu2);
}
// ---------------------------------------------------------------------------------------------------
IUSet::IUSet(const OrderedIUSet& orderedSet) : IUSet(orderedSet.begin(), orderedSet.end()) {
}
// ---------------------------------------------------------------------------------------------------
[[nodiscard]] IUSet IUSet::intersect(const IUSet& other) const {
   IUSet intersection;
   for (auto& elem : *this) {
      if (other.contains(elem))
         intersection.insert(elem);
   }
   return intersection;
}
// ---------------------------------------------------------------------------------------------------
IUSet IUSet::intersect(const OrderedIUSet& other) const {
   return other.intersect(*this);
}
// ---------------------------------------------------------------------------------------------------
IUSet IUSet::difference(const OrderedIUSet& other) const {
   IUSet difference;
   for (auto& elem : *this) {
      if (!other.contains(elem))
         difference.insert(elem);
   }
   return difference;
}
// ---------------------------------------------------------------------------------------------------
[[nodiscard]] IUSet IUSet::merge(const IUSet& other) const noexcept {
   IUSet merged = *this;
   for (auto& iu : other) {
      if (!contains(iu))
         merged.insert(iu);
   }
   return merged;
}
// ---------------------------------------------------------------------------------------------------
const IU* OrderedIUSet::find(const factDB::IU& iu) const {
   auto res = std::find_if(begin(), end(), [&iu](const IU* cur) { return iu == *cur; });
   return res == end() ? nullptr : *res;
}
// ---------------------------------------------------------------------------------------------------
bool OrderedIUSet::contains(const IU& iu) const {
   return std::find_if(begin(), end(), [&iu](const IU* cur) { return iu == *cur; }) != end();
}
// ---------------------------------------------------------------------------------------------------
OrderedIUSet& OrderedIUSet::removeDuplicates() {
   std::sort(begin(), end());
   erase(std::unique(begin(), end()), end());
   return *this;
}
// ---------------------------------------------------------------------------------------------------
OrderedIUSet& OrderedIUSet::merge(const factDB::OrderedIUSet& other) {
   reserve(size() + other.size());
   insert(end(), other.begin(), other.end());
   return *this;
}
// ---------------------------------------------------------------------------------------------------
IUSet OrderedIUSet::intersect(const factDB::OrderedIUSet& other) const {
   IUSet intersection;
   std::set_intersection(begin(), end(), other.begin(), other.end(), std::inserter(intersection, intersection.begin()));
   return intersection;
}
// ---------------------------------------------------------------------------------------------------
IUSet OrderedIUSet::intersect(const factDB::IUSet& other) const {
   IUSet intersection;
   for (auto& elem : *this) {
      if (other.contains(elem))
         intersection.insert(elem);
   }
   return intersection;
}
// ---------------------------------------------------------------------------------------------------
IUSet OrderedIUSet::difference(const factDB::IUSet& other) const {
   IUSet difference;
   for (auto& elem : *this) {
      if (!other.contains(elem))
         difference.insert(elem);
   }
   return difference;
}
// ---------------------------------------------------------------------------------------------------
bool OrderedIUSet::iuOrder(const factDB::IU* first, const factDB::IU* second) {
   if (first->table < second->table)
      return true;
   if (first->table > second->table)
      return false;
   return first->column < second->column;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------