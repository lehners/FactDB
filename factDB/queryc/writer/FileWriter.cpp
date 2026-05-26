#include "factDB/queryc/writer/FileWriter.hpp"
#include "factDB/infra/BitSet.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/queryc/writer/container/ConditionalContainer.hpp"
#include "factDB/queryc/writer/container/Container.hpp"
#include "factDB/queryc/writer/container/FormatContainer.hpp"
#include "factDB/queryc/writer/container/FunctorContainer.hpp"
#include "factDB/queryc/writer/container/ListContainer.hpp"
#include "factDB/queryc/writer/container/NewlineContainer.hpp"
#include "factDB/queryc/writer/container/PointerDereferenceContainer.hpp"
#include "factDB/queryc/writer/container/ReferenceContainer.hpp"
#include "factDB/queryc/writer/container/SeparatorContainer.hpp"
#include "factDB/queryc/writer/container/TrivialContainer.hpp"
#include "factDB/schemac/Type.hpp"
#include "factDB/util/TemporaySetter.hpp"
#include <cassert>
#include <filesystem>
#include <fmt/format.h>
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
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
std::ostream& printConstExpr(std::ostream& os, const Const& constExpr) {
   switch (constExpr.getConstType().tclass) {
      case schemac::Type::KInteger:
         return os << "Integer(" << constExpr.get_value() << ")";
      case schemac::Type::KUInt64:
         break;
         return os << "UInt64(" << constExpr.get_value() << ")";
      case schemac::Type::KTimestamp:
         return os << "Timestamp(" << constExpr.get_value() << ")";
      case schemac::Type::KDate:
         return os << "Date(" << constExpr.get_value() << ")";
      case schemac::Type::KNumeric:
      case schemac::Type::KBool:
         return os << constExpr.get_value();
      case schemac::Type::KChar:
      case schemac::Type::KVarchar:
      case schemac::Type::KUndefined:
         return os << "\"" << constExpr.get_value() << "\"";
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const factDB::Expression& expression)
/// Printer for a predicate
{
   auto* exprPtr = &expression;
   switch (expression.get_type()) {
      case Expression::CompareL:
      case Expression::CompareLE:
      case Expression::CompareEq:
      case Expression::CompareGE:
      case Expression::CompareG:
      case Expression::AndExpression:
      case Expression::OrExpression: {
         auto cmp = static_cast<const BinaryExpression*>(exprPtr);
         return os << cmp->get_left() << " " << cmp->getComparator() << " " << cmp->get_right();
      }
      case Expression::IURef:
         return os << static_cast<const IURef*>(exprPtr)->get_iu();
      case Expression::Const:
         return printConstExpr(os, *static_cast<const Const*>(exprPtr));
      case Expression::JoinConditionList:
         for (auto& a : *static_cast<const JoinConditionList*>(exprPtr))
            os << a.get_left() << " == " << a.get_right() << "\n";
         return os;
      case Expression::JoinCondition: {
         auto cond = static_cast<const JoinCondition*>(exprPtr);
         os << cond->get_left() << " == " << cond->get_right() << "\n";
         return os;
      }
      case Expression::Reference: {
         auto ref = static_cast<const ReferenceExpression*>(exprPtr);
         return os << ref->getReferencedExpr();
      }
      default:
         unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
FileWriter::FileWriter(std::ostream& streamParam, bool verboseParam, bool finalNewline_) : stream(streamParam), verbose(verboseParam), finalNewline(finalNewline_) {
   separators.emplace(", ");
}
// ---------------------------------------------------------------------------------------------------
FileWriter::~FileWriter() {
   if (finalNewline)
      stream << std::endl;
   assert(indent == 0 && "indentation mismatch");
}
// ---------------------------------------------------------------------------------------------------
std::ostream& FileWriter::printIndent(int curIndent) {
   assert(indent >= 0);
   for (auto i = 0; requiresIndent && i < curIndent; i++)
      stream << "   ";
   requiresIndent = false;
   return stream;
}
// ---------------------------------------------------------------------------------------------------
void FileWriter::linebreak(const std::source_location& loc) {
   requiresIndent = true;
   std::filesystem::path path = loc.file_name();
   stream << (verbose ? fmt::format(" // {}({}:{})\n", path.filename().string(), loc.line(), loc.column()) : "\n");
   stream.flush();
}
// ---------------------------------------------------------------------------------------------------
void FileWriter::setSeparator(const std::string& val, bool newline, bool skipFirst) {
   auto& top_sep = separators.top();

   top_sep.separatorVal = val;
   top_sep.newlineAfter = newline;
   restartSeparatorSequence();
   top_sep.firstSeparator = skipFirst;
}
// ---------------------------------------------------------------------------------------------------
void FileWriter::restartSeparatorSequence() {
   separators.top().firstSeparator = true;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::addChar(const char val, bool doIndent) {
   auto requiredIndent = indent;
   switch (val) {
      case '\"':
         if (!escapeNextChar) inQuote = !inQuote;
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
         assert(escaped != 0 && "String must not contain a new line or be properly escaped (\\n)");
         break;
      case ' ':
      case '\f':
      case '\r':
      case '\t':
      case '\v':
         if (escaped == 0 && requiresIndent)
            return *this;
   }
   if (doIndent)
      printIndent(requiredIndent);

   if (escaped) {
      switch (val) {
         case '\\': stream << "\\\\"; break;
         case '\"': stream << "\\\""; break;
         case '\n': stream << "\\n"; break;
         case '\r': stream << "\\r"; break;
         case '\t': stream << "\\t"; break;
         case '\b': stream << "\\b"; break;
         case '\f': stream << "\\f"; break;
         case '\v': stream << "\\v"; break;
         default:
            stream << val;
      }
   } else {
      stream << val;
   }
   stream.flush();
   escapeNextChar = (val == '\\');

   return *this;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const std::string& val) {
   return *this << val.c_str();
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const std::string_view& val) {
   return *this << val.data();
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const unsigned int& val) {
   printIndent(indent) << val;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const char* val) {
   while (requiresIndent && std::isspace(*val))
      val++;

   assert(!checkQuotes || !inQuote); // must either ignore current quote state or be outside of quotes

   for (const char* cur = val; *cur; ++cur) {
      addChar(*cur, cur == val); // only indent for first letter, since we do not allow linebreaks here
   }
   assert((!checkQuotes || !inQuote) && "Contains unterminated quote");
   return *this;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const IU& iu) {
   printIndent(indent) << iu;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const IU* iu) {
   return *this << *iu;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const infra::BitSet64& bitSet) {
   *this << "(" << SeparatorInfo(", ");
   for (auto val : bitSet)
      *this << SeparatorContainer{true, false, std::source_location::current()} << val;
   *this << ")" << Container(CT_SeparatorPop);
   return *this;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const Expression& expression) {
   printIndent(indent) << expression;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const factDB::schemac::Type& type) {
   printIndent(indent) << type;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const StringContainer& container) {
   return *this << container.val;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const StringViewContainer& container) {
   return *this << container.val;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const StringEscapeContainer& container) {
   *this << "\"";
   ++escaped;
   *this << container.reference;
   --escaped;
   return *this << "\"";
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const IntContainer& container) {
   return *this << container.val;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const SizeTContainer& container) {
   printIndent(indent) << container.val;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const UnsignedContainer& container) {
   printIndent(indent) << container.val;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const IuContainer& container) {
   return *this << container.val;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const SchemaTypeContainer& container) {
   return *this << container.val;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const ConditionalContainer& container) {
   if (container.condition)
      return *this << container.trueCase;
   else
      return *this << container.falseCase;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const FWContainer& container) {
   return *this << *container.container;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const PointerDereferenceContainer& container) {
   return *this << "*" << container.reference;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const SeparatorInfo& container) {
   separators.push(container);
   return *this;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const FunctorContainer& container) {
   container.functor(*this);
   return *this;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const NewlineContainer& s) {
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
FileWriter& FileWriter::operator<<(const BreakpointContainer& container) {
   if constexpr (debugMode)
      printIndent(indent) << "asm(\"int $3\"); // debug breakpoint";
   else
      printIndent(indent) << "// no breakpoint in release mode";

   linebreak(container.location);
   return *this;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const GetContainer& container) {
   return *this << "std::get<" << container.position << ">(" << container.variable << ")";
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const SeparatorContainer& container) {
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
FileWriter& FileWriter::operator<<(const NopContainer&) {
   return *this;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const ReferenceContainer& container) {
   return *this << container.reference;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const std::unique_ptr<Container>& container) {
   return *this << *container;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const FormatContainer& container) {
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
            } else if (!indexed) {
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
               assert(paramIdx == 0 || indexed);
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
FileWriter& FileWriter::operator<<(const ListContainer& type) {
   for (const FWContainer& elem : type.elements)
      *this << elem;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::operator<<(const Container& container) {
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
      case CT_SeparatorPop:
         separators.pop();
         assert(!separators.empty());
         return *this;
      case CT_Format: return *this << static_cast<const FormatContainer&>(container);
      case CT_Star: return *this << static_cast<const PointerDereferenceContainer&>(container);
      case CT_Functor: return *this << static_cast<const FunctorContainer&>(container);
      case CT_VERBOSE: verbose = static_cast<const VerboseContainer&>(container).verbose; return *this;
      case CT_QuoteCheck:
         checkQuotes = static_cast<const CheckQuoteContainer&>(container).checkQuotes;
         assert((!checkQuotes || !inQuote) && "Contains unterminated quote");
         return *this;
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::addInclude(const FWContainer& header, std::source_location location) {
   return *this << FormatContainer("#include \"{}\"", header) << NewlineContainer(location);
}
// ---------------------------------------------------------------------------------------------------
FileWriter& FileWriter::addFunctionHeader(const std::string& signature, bool externFun, std::source_location location) {
   return *this << NewlineContainer(location)
                << (externFun ? "extern \"C\" " : "") << signature << "  {" << NewlineContainer(location);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------