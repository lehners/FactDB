#ifndef H_factdb_queryc_writer_container_referenceContainer
#define H_factdb_queryc_writer_container_referenceContainer
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/container/Container.hpp"
#include <memory>
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
struct FWContainer;
// ---------------------------------------------------------------------------------------------------
struct ReferenceContainer : public Container {
   const Container& reference;
   ReferenceContainer(const Container& ref);
   ReferenceContainer(const std::unique_ptr<Container>& ref);
   ReferenceContainer(const FWContainer& ref);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_queryc_writer_container_referenceContainer