#ifndef H_factdb_queryc_writer_container_listcontainer
#define H_factdb_queryc_writer_container_listcontainer
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/container/Container.hpp"
#include <cstddef>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
struct FWContainer;
// ---------------------------------------------------------------------------------------------------
struct ListContainer : public Container {
   std::vector<FWContainer> elements;
   ListContainer();
   ~ListContainer();
   explicit ListContainer(FWContainer&& cont);
   ListContainer& operator<<(FWContainer&& item);
   ListContainer& operator<<(FWContainer& item);
   const FWContainer& operator[](size_t i) const;
   [[nodiscard]] size_t size() const;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_queryc_writer_container_listcontainer