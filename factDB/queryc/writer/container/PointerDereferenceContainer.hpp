#ifndef H_factdb_queryc_writer_container_pointerdereferencecontainer
#define H_factdb_queryc_writer_container_pointerdereferencecontainer
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/FWContainer.hpp"
#include "factDB/queryc/writer/container/Container.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
struct PointerDereferenceContainer : public Container {
   FWContainer reference;
   explicit PointerDereferenceContainer(FWContainer&& ref);
   explicit PointerDereferenceContainer(const FWContainer& ref);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_queryc_writer_container_pointerdereferencecontainer