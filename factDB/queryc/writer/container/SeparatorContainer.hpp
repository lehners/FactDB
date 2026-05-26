#ifndef H_factdb_queryc_writer_container_separatorcontainer
#define H_factdb_queryc_writer_container_separatorcontainer
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/container/Container.hpp"
#include <source_location>
#include <string>
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
struct SeparatorInfo : public Container {
   bool firstSeparator = true;
   bool newlineAfter = false;
   std::string separatorVal = ", ";

   SeparatorInfo(const std::string& val, bool newline = false, bool skipFirst = true)
      : Container(CT_SeparatorPush), firstSeparator(skipFirst), newlineAfter(newline), separatorVal(std::move(val)){};
};
// ---------------------------------------------------------------------------------------------------
struct SeparatorContainer : public Container {
   bool doSeparator;
   bool restartSeparator;
   std::source_location location;
   SeparatorContainer(bool doSeparatorP, bool restart, std::source_location loc) : Container(CT_Separator), doSeparator(doSeparatorP), restartSeparator(restart), location(loc) {}
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_queryc_writer_container_separatorcontainer