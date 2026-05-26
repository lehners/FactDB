#ifndef H_factdb_queryc_writer_container
#define H_factdb_queryc_writer_container
// ---------------------------------------------------------------------------------------------------
// #include "factDB/queryc/writer/FWContainer.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
class NewFileWriter;
// ---------------------------------------------------------------------------------------------------
enum ContainerType {
   CT_String,
   CT_Reference,
   CT_Int,
   CT_SizeT,
   CT_Unsigned,
   CT_IU,
   CT_SchemaType,
   CT_ListContainer,
   CT_Newline,
   CT_Breakpoint,
   CT_Get,
   CT_NOP,
   CT_Separator,
   CT_SeparatorPush,
   CT_SeparatorPop,
   CT_Format,
   CT_StringEscape,
   CT_Conditional,
   CT_Star,
   CT_Functor,
   CT_StringView,
   CT_VERBOSE,
   CT_QuoteCheck
};
// ---------------------------------------------------------------------------------------------------
struct Container {
   ContainerType type;
   Container() : type(CT_NOP) {}
   explicit Container(ContainerType t) : type(t){};
   virtual ~Container() = default;
   [[nodiscard]] bool isNOP() const { return type == CT_NOP; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_queryc_writer_container