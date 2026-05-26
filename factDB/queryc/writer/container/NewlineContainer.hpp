#ifndef H_factdb_queryc_writer_container_newlinecontainer
#define H_factdb_queryc_writer_container_newlinecontainer
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/container/Container.hpp"
#include <source_location>
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
struct NewlineContainer : public Container {
   bool semicolon;
   bool nonEmptyLineOnly;
   std::source_location location;
   ~NewlineContainer() override = default;
   explicit NewlineContainer(bool semicolonP, bool nonEmptyLineOnlyP, std::source_location loc) : Container(CT_Newline), semicolon(semicolonP), nonEmptyLineOnly(nonEmptyLineOnlyP), location(loc) {}
   explicit NewlineContainer(std::source_location loc) : Container(CT_Newline), semicolon(false), nonEmptyLineOnly(false), location(loc) {}
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_queryc_writer_container_newlinecontainer