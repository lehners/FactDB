#ifndef H_factdb_queryc_writer_filewriter
#define H_factdb_queryc_writer_filewriter
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/FWContainer.hpp"
#include "factDB/queryc/writer/container/SeparatorContainer.hpp"
#include <ostream>
#include <source_location>
#include <stack>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class IU;
class Expression;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra { class BitSet64; }
namespace factDB::schemac { struct Type; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
struct ListContainer;
struct StringContainer;
struct StringViewContainer;
struct StringEscapeContainer;
struct IntContainer;
struct SizeTContainer;
struct UnsignedContainer;
struct IuContainer;
struct SchemaTypeContainer;
struct ConditionalContainer;
struct PointerDereferenceContainer;
struct FunctorContainer;
struct IuContainer;
struct NewlineContainer;
struct BreakpointContainer;
struct GetContainer;
struct NopContainer;
struct FormatContainer;
struct ReferenceContainer;
struct FWContainer;
struct SeparatorInfo;
// ---------------------------------------------------------------------------------------------------
FWContainer separator(bool doSeparator, std::source_location loc);
// ---------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const factDB::schemac::Type& type);
std::ostream& operator<<(std::ostream& os, const factDB::IU& iu);
std::ostream& operator<<(std::ostream& os, const factDB::Expression& expression);
// ---------------------------------------------------------------------------------------------------
struct FileWriter {
   private:
   std::ostream& stream;
   int indent = 0;
   bool requiresIndent = true;
   bool verbose = true;
   std::stack<SeparatorInfo> separators;
   bool checkQuotes = true;
   bool inQuote = false;
   size_t escaped = 0;
   bool escapeNextChar = true;
   bool finalNewline = true;

   std::ostream& printIndent(int curIndent);
   void linebreak(const std::source_location& loc = std::source_location::current());
   FileWriter& addChar(char c, bool doIndent = true);

   public:
   enum FinalNewlineEnum { FinalNewline,
                           NoFinalNewline };
   enum VerboseEnum { Verbose,
                      Silent };

   private:
   FileWriter(std::ostream& stream, bool verbose, bool useNewline);

   public:
   FileWriter(std::ostream& stream_, VerboseEnum verbosity, FinalNewlineEnum finalNewline_ = FinalNewline) : FileWriter(stream_, verbosity == Verbose, finalNewline_ == FinalNewline) {}
   FileWriter(std::ostream& stream_, FinalNewlineEnum finalNewline_ = FinalNewline) : FileWriter(stream_, true, finalNewline_ == FinalNewline) {}

   FileWriter(const FileWriter&) = delete;
   FileWriter& operator=(const FileWriter&) = delete;
   ~FileWriter();

   void setSeparator(const std::string& val, bool newline = false, bool skipFirst = true);
   void useFinalNewline(bool useNewline) { finalNewline = useNewline; };
   void restartSeparatorSequence();

   template <std::ranges::input_range Iterable>
   FileWriter& operator<<(const Iterable& iterable);
   FileWriter& operator<<(const std::string& val);
   FileWriter& operator<<(const std::string_view& val);
   FileWriter& operator<<(const unsigned int& val);
   FileWriter& operator<<(const char* val);
   FileWriter& operator<<(const IU* iu);
   FileWriter& operator<<(const IU& iu);
   FileWriter& operator<<(const infra::BitSet64& bitSet64);
   FileWriter& operator<<(const Expression& iu);
   FileWriter& operator<<(const schemac::Type& type);
   // ---------------------------------------------------------------------------------------------------
   FileWriter& operator<<(const Container& container);
   FileWriter& operator<<(const std::unique_ptr<Container>& container);
   FileWriter& operator<<(const ListContainer& container);
   // ---------------------------------------------------------------------------------------------------
   FileWriter& operator<<(const StringContainer& container);
   FileWriter& operator<<(const StringViewContainer& container);
   FileWriter& operator<<(const StringEscapeContainer& container);
   FileWriter& operator<<(const IntContainer& container);
   FileWriter& operator<<(const SizeTContainer& container);
   FileWriter& operator<<(const UnsignedContainer& container);
   FileWriter& operator<<(const IuContainer& container);
   FileWriter& operator<<(const SchemaTypeContainer& container);
   FileWriter& operator<<(const ConditionalContainer& container);
   FileWriter& operator<<(const FWContainer& container);
   FileWriter& operator<<(const PointerDereferenceContainer& container);
   FileWriter& operator<<(const SeparatorInfo& container);
   FileWriter& operator<<(const FunctorContainer& container);
   // ---------------------------------------------------------------------------------------------------
   FileWriter& operator<<(const NewlineContainer& container);
   FileWriter& operator<<(const BreakpointContainer& container);
   FileWriter& operator<<(const GetContainer& container);
   FileWriter& operator<<(const NopContainer& container);
   FileWriter& operator<<(const SeparatorContainer& container);
   FileWriter& operator<<(const FormatContainer& container);
   FileWriter& operator<<(const ReferenceContainer& container);

   FileWriter& addInclude(const FWContainer& header, std::source_location location = std::source_location::current());
   FileWriter& addFunctionHeader(const std::string& signature, bool externFun = true, std::source_location location = std::source_location::current());
};
// ---------------------------------------------------------------------------------------------------
template <std::ranges::input_range Iterable>
FileWriter& FileWriter::operator<<(const Iterable& iterable) {
   restartSeparatorSequence();
   for (const auto& i : iterable)
      *this << separator(true, std::source_location::current()) << i;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
using FileWriter = fw::FileWriter;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_queryc_writer_filewriter