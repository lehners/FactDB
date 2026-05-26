#include "factDB/queryc/writer/container/ListContainer.hpp"
#include "Container.hpp"
#include "factDB/queryc/writer/FWContainer.hpp"
#include <cassert>
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
ListContainer::ListContainer() : Container(CT_ListContainer) {
}
// ---------------------------------------------------------------------------------------------------
ListContainer::ListContainer(FWContainer&& cont) : Container(CT_ListContainer) {
   *this << cont;
};
// ---------------------------------------------------------------------------------------------------
ListContainer::~ListContainer() = default;
// ---------------------------------------------------------------------------------------------------
ListContainer& ListContainer::operator<<(FWContainer&& item) {
   elements.push_back(std::move(item));
   return *this;
}
// ---------------------------------------------------------------------------------------------------
ListContainer& ListContainer::operator<<(FWContainer& item) {
   elements.push_back(std::move(item));
   return *this;
}
// ---------------------------------------------------------------------------------------------------
const FWContainer& ListContainer::operator[](size_t i) const {
   assert(i < elements.size());
   return elements[i];
}
// ---------------------------------------------------------------------------------------------------
size_t ListContainer::size() const {
   return elements.size();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------