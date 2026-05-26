#ifndef H_factdb_queryc_writer_container_trivialcontainer
#define H_factdb_queryc_writer_container_trivialcontainer
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/container/Container.hpp"
#include <memory>
#include <source_location>
#include <string>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
class IU;
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
namespace factDB::schemac {
struct Type;
} // namespace factDB::schemac
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
struct ListContainer;
struct FWContainer;
// ---------------------------------------------------------------------------------------------------
struct IntContainer : public Container {
   size_t val;
   explicit IntContainer(size_t v) : Container(CT_Int), val(v){};
};
// ---------------------------------------------------------------------------------------------------
struct SizeTContainer : public Container {
   size_t val;
   explicit SizeTContainer(size_t v) : Container(CT_SizeT), val(v){};
};
// ---------------------------------------------------------------------------------------------------
struct UnsignedContainer : public Container {
   size_t val;
   explicit UnsignedContainer(unsigned v) : Container(CT_Unsigned), val(v){};
};
// ---------------------------------------------------------------------------------------------------
struct StringContainer : public Container {
   std::string val;
   explicit StringContainer(std::string v) : Container(CT_String), val(std::move(v)){};
   explicit StringContainer(const char* v) : Container(CT_String), val(std::string(v)){};

   protected:
   explicit StringContainer(ContainerType typeP, std::string v) : Container(typeP), val(std::move(v)){};
};
// ---------------------------------------------------------------------------------------------------
struct StringViewContainer : public Container {
   std::string_view val;
   explicit StringViewContainer(const std::string_view& v) : Container(CT_StringView), val(v){};
};
// ---------------------------------------------------------------------------------------------------
struct IuContainer : public Container {
   const IU* val;
   explicit IuContainer(const IU& iu) : Container(CT_IU), val(&iu) {}
};
// ---------------------------------------------------------------------------------------------------
struct BreakpointContainer : public Container {
   std::source_location location;
   BreakpointContainer(std::source_location loc) : Container(CT_Breakpoint), location(loc) {}
};
// ---------------------------------------------------------------------------------------------------
struct NopContainer : public Container {
   NopContainer() : Container(CT_NOP) {}
};
// ---------------------------------------------------------------------------------------------------
struct VerboseContainer : public Container {
   bool verbose;
   VerboseContainer(bool v) : Container(CT_VERBOSE), verbose(v) {}
};
// ---------------------------------------------------------------------------------------------------
struct CheckQuoteContainer : public Container {
   bool checkQuotes;
   CheckQuoteContainer(bool v) : Container(CT_QuoteCheck), checkQuotes(v) {}
};
// ---------------------------------------------------------------------------------------------------
struct SchemaTypeContainer : public Container {
   const schemac::Type& val;
   explicit SchemaTypeContainer(const schemac::Type& v);
};
// ---------------------------------------------------------------------------------------------------
struct GetContainer : public Container {
   size_t position;
   ListContainer& variable;
   GetContainer(size_t idx, ListContainer& var);
};
// ---------------------------------------------------------------------------------------------------
struct StringEscapeContainer : public Container {
   const Container& reference;
   explicit StringEscapeContainer(const Container& ref) : Container(CT_StringEscape), reference(ref) {}
   explicit StringEscapeContainer(const std::unique_ptr<Container>& ref) : Container(CT_StringEscape), reference(*ref.get()) {}
   explicit StringEscapeContainer(const FWContainer& ref); // : Container(CT_StringEscape), reference(*ref.container.get()) {}
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_queryc_writer_container_trivialcontainer