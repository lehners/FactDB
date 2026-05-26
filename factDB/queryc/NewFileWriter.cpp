#include "factDB/queryc/NewFileWriter.hpp"
// #include "factDB/infra/Expression.hpp"
// #include "factDB/infra/iu.hpp"
// #include "factDB/util/TemporaySetter.hpp"
// #include "fmt/format.h"
// #include "infra/Config.hpp"
// #include <filesystem>
// ---------------------------------------------------------------------------------------------------
#if 0
namespace factDB {
// ---------------------------------------------------------------------------------------------------
NewFileWriter NewFileWriter::nullStream = NewFileWriter(std::cout);
// ---------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const factDB::schemac::Type& type)
/// Printer for a Type
{
   return os << type.toString();
}
// ---------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const factDB::IU& iu)
/// Printer for an IU
{
   return os << iu.table << "_" << iu.column;
}
// ---------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const factDB::Expression& expression)
/// Printer for a predicate
{
   auto* exprPtr = &expression;
   switch (expression.get_type()) {
      case Expression::Compare: {
         auto cmp = static_cast<const Compare*>(exprPtr);
         return os << cmp->get_left() << " == " << cmp->get_right();
      }
      case Expression::IURef:
         return os << static_cast<const IURef*>(exprPtr)->get_iu();
      case Expression::Const:
         return os << static_cast<const Const*>(exprPtr)->get_value();
      case Expression::AndExpression: {
         auto andExpr = static_cast<const AndExpression*>(exprPtr);
         return os << andExpr->get_left() << " && " << andExpr->get_right();
      }
      case Expression::OrExpression: {
         auto orExpr = static_cast<const OrExpression*>(exprPtr);
         return os << orExpr->get_left() << " || " << orExpr->get_right();
      }
      case Expression::JoinConditionList:
         for (auto& a : *static_cast<const JoinConditionList*>(exprPtr))
            os << a.get_left() << " == " << a.get_right() << "\n";
         return os;
      default: return os;
   }
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter::NewFileWriter(std::ostream& streamParam, bool verboseParam) : stream(streamParam), verbose(verboseParam) {
   separators.push(SeparatorInfo(", "));
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter::~NewFileWriter() {
   if (finalNewline)
      stream << std::endl;
   assert(indent == 0 && "indentation mismatch");
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const std::string& val) {
   return *this << val.c_str();
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const std::string_view& val) {
   return *this << val.data();
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const unsigned int& val) {
   printIndent(indent) << val;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::addChar(const char val, bool doIndent) {
   auto requiredIndent = indent;
   switch (val) {
      case '\"':
         inQuote = !inQuote;
         break;
      case '}':
      case ']':
      case ')':
         if (!inQuote) {
            --indent;
            --requiredIndent;
         }
         break;
      case '{':
      case '[':
      case '(':
         if (!inQuote) ++indent;
         break;
      case '\n':
         assert(false && "String must not contain a new line (\\n)");
         break;
      case ' ':
      case '\f':
      case '\r':
      case '\t':
      case '\v':
         if (requiresIndent)
            return *this;
   }
   if (doIndent)
      printIndent(requiredIndent);

   stream << val;
   stream.flush();

   return *this;
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const char* val) {
   while (requiresIndent && std::isspace(*val))
      val++;

   assert(!checkQuotes || !inQuote); // must either ignore current quote state or be outside of quotes

   for (const char* cur = val; *cur; ++cur) {
      addChar(*cur, cur==val); // only indent for first letter, since we do not allow linebreaks here
   }
   assert((!checkQuotes || !inQuote) && "Contains unterminated quote");
   return *this;
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const IU& iu) {
   printIndent(indent) << iu;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const IU* iu) {
   return *this << *iu;
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const Expression& iu) {
   printIndent(indent) << iu;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const factDB::schemac::Type& type) {
   printIndent(indent) << type;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::addInclude(const FWContainer& header, std::source_location location) {
   return *this << NewFileWriter::fmt("#include \"{}\"", header) << endl(location);
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::addFunctionHeader(const std::string& signature, std::source_location location) {
   return *this << endl(location)
                << ("extern \"C\" ") << signature << "  {" << endl(location);
}
// ---------------------------------------------------------------------------------------------------
std::ostream& NewFileWriter::printIndent(int curIndent) {
   assert(indent >= 0);
   for (auto i = 0; requiresIndent && i < curIndent; i++)
      stream << "   ";
   requiresIndent = false;
   return stream;
}
// ---------------------------------------------------------------------------------------------------
void NewFileWriter::linebreak(const std::source_location& loc) {
   requiresIndent = true;
   std::filesystem::path path = loc.file_name();
   stream << (verbose ? fmt::format(" // {}({}:{})\n", path.filename().string(), loc.line(), loc.column()) : "\n");
   stream.flush();
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const NewlineContainer& s) {
   if (s.semicolon) *this << ";";
   if (requiresIndent) { // empty line
      if (!s.nonEmptyLineOnly)
         stream << "\n";
   } else {
      linebreak(s.location);
   }
   return *this;
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const BreakpointContainer& container) {
#ifdef NDEBUG
   printIndent(indent) << "// no breakpoint in release mode";
#else
   printIndent(indent) << "asm(\"int $3\"); // debug breakpoint";
#endif
   linebreak(container.location);
   return *this;
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const GetContainer& container) {
   return *this << "std::get<" << container.position << ">(" << container.variable << ")";
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const SeparatorContainer& container) {
   auto& top_sep = separators.top();

   if (!top_sep.firstSeparator && container.doSeparator) {
      *this << top_sep.separatorVal;
      if (top_sep.newlineAfter)
         linebreak(container.location);
   }
   top_sep.firstSeparator = false;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const FormatContainer& container) {
   bool parenthesesOpen = false;
   bool indexed = false;
   size_t paramIdx = 0, fmtStrSize = container.formatString.size();
   auto peek = [fmtStrSize](size_t cur_idx, const char* cPtr, char cmpTo) {
      return cur_idx + 1 < fmtStrSize && *(cPtr + 1) == cmpTo;
   };

   for (size_t i = 0; i < fmtStrSize; i++) {
      const char& c = container.formatString[i];
      const char* c_ptr = &container.formatString[i];
      switch (c) {
         case '{':
            assert(!parenthesesOpen);
            if (peek(i, c_ptr, '{')) { // two braces, i.e. escaped braces
               addChar('{');
               ++i; // two chars were read
            } else if (!indexed){
               parenthesesOpen = true;
            } else {
               paramIdx = 0;
               parenthesesOpen = true;
            }
            break;
         case '}':
            if (peek(i, c_ptr, '}')) { // two braces, i.e. escaped braces
               addChar('}');
               ++i; // two chars were read
            } else {
               assert(parenthesesOpen);
               assert(paramIdx < container.args.size() && "format string contains index out of range");
               TemporarySetter tsCheckQuotes(checkQuotes, false);
               *this << container.args[paramIdx];
               parenthesesOpen = false;
               ++paramIdx;
            }
            break;
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
         case '0':
            if (parenthesesOpen) {
               assert(paramIdx==0 || indexed);
               indexed = true;
               paramIdx = 10 * paramIdx + (c - '0');
            } else {
               addChar(c);
            }
            break;
         default:
            addChar(c);
      }
   }


   assert(!parenthesesOpen); // check that no opened parentheses are left
   return *this;
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const ListContainer& type) {
   for (const FWContainer& elem : type.elements)
      *this << elem;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const std::unique_ptr<Container>& container) {
   return *this << *container;
}
// ---------------------------------------------------------------------------------------------------
NewFileWriter& NewFileWriter::operator<<(const Container& container) {

   switch (container.type) {
      case CT_String: return *this << static_cast<const StringContainer&>(container);
      case CT_StringView: return *this << static_cast<const StringViewContainer&>(container);
      case CT_StringEscape: return *this << static_cast<const StringEscapeContainer&>(container);
      case CT_Int: return *this << static_cast<const IntContainer&>(container);
      case CT_SizeT: return *this << static_cast<const SizeTContainer&>(container);
      case CT_Unsigned: return *this << static_cast<const UnsignedContainer&>(container);
      case CT_IU: return *this << static_cast<const IuContainer&>(container);
      case CT_SchemaType: return *this << static_cast<const SchemaTypeContainer&>(container);
      case CT_ListContainer: return *this << static_cast<const ListContainer&>(container);
      case CT_Conditional: return *this << static_cast<const ConditionalContainer&>(container);

      case CT_Newline: return *this << static_cast<const NewlineContainer&>(container);
      case CT_Reference: return *this << static_cast<const ReferenceContainer&>(container);
      case CT_Breakpoint: return *this << static_cast<const BreakpointContainer&>(container);
      case CT_Get: return *this << static_cast<const GetContainer&>(container);
      case CT_NOP: return *this << static_cast<const NopContainer&>(container);
      case CT_Separator: return *this << static_cast<const SeparatorContainer&>(container);
      case CT_SeparatorPush: return *this << static_cast<const SeparatorInfo&>(container);
      case CT_SeparatorPop: separators.pop(); assert(!separators.empty()); return *this;
      case CT_Format: return *this << static_cast<const FormatContainer&>(container);
      case CT_Star: return *this << static_cast<const PointerDereferenceContainer&>(container);
      case CT_Functor: return *this << static_cast<const FunctorContainer&>(container);
      case CT_VERBOSE: verbose = static_cast<const VerboseContainer&>(container).verbose; return *this;
   }
   unreachable();
   // todo unreachable

   //*this << *container.get();
   //return *this;
}
// ---------------------------------------------------------------------------------------------------
FWContainer Container::dereference() const {
   return std::make_unique<PointerDereferenceContainer>(*this);
}
// ---------------------------------------------------------------------------------------------------
FWContainer FWContainer::dereference() const {
   return cast(std::make_unique<PointerDereferenceContainer>(ReferenceContainer(container)));
}
// ---------------------------------------------------------------------------------------------------
/*NewFileWriter& NewFileWriter::operator<<(const Container& container) {
   switch (container.type) {
      case CT_String: return *this << static_cast<const StringContainer&>(container);
      case CT_Int: return *this << static_cast<const IntContainer&>(container);
      case CT_SizeT: return *this << static_cast<const SizeTContainer&>(container);
      case CT_Unsigned: return *this << static_cast<const UnsignedContainer&>(container);
      case CT_IU: return *this << static_cast<const IuContainer&>(container);
      case CT_SchemaType: return *this << static_cast<const SchemaTypeContainer&>(container);
      case CT_ListContainer: return *this << static_cast<const ListContainer&>(container);

      case CT_Newline: return *this << dynamic_cast<const NewlineContainer&>(container);
      case CT_Breakpoint: return *this << static_cast<const BreakpointContainer&>(container);
      case CT_Get: return *this << static_cast<const GetContainer&>(container);
      case CT_NOP: return *this << static_cast<const NopContainer&>(container);
      case CT_Separator: return *this << static_cast<const SeparatorContainer&>(container);
      case CT_Format: return *this << static_cast<const FormatContainer&>(container);
   }
   assert(false);
}*/
// ---------------------------------------------------------------------------------------------------
}
#endif
// ---------------------------------------------------------------------------------------------------
