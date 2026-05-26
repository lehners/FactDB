#ifndef H_factdb_queryc_writer_container_formatcontainer
#define H_factdb_queryc_writer_container_formatcontainer
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/container/Container.hpp"
#include "factDB/queryc/writer/container/ListContainer.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
struct FormatContainer : public Container {
   std::string formatString;
   ListContainer args;
   template <typename... T>
   FormatContainer(std::string fmtString, T&&... arguments);
   FormatContainer() : Container(CT_Format){};
};
// ---------------------------------------------------------------------------------------------------
template <typename... T>
FormatContainer::FormatContainer(std::string fmtString, T&&... arguments) : Container(CT_Format), formatString(std::move(fmtString)) {
   if constexpr (sizeof...(T) != 0) {
      (args << ... << FWContainer::gen(std::forward<T>(arguments)));
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_queryc_writer_container_formatcontainer