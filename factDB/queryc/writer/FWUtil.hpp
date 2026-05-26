#ifndef H_factdb_queryc_writer_fwutil
#define H_factdb_queryc_writer_fwutil
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/FWContainer.hpp"
#include "factDB/queryc/writer/FileWriter.hpp"
#include "factDB/queryc/writer/container/FormatContainer.hpp"
#include <functional>
#include <iostream>
#include <memory>
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
template <typename>
struct is_bounded_char_array : std::false_type {};
template <size_t N>
struct is_bounded_char_array<const char (&)[N]> : std::true_type {};
// ---------------------------------------------------------------------------------------------------
template <typename T>
struct is_unique_ptr_of_container : std::false_type {};
template <typename T, typename Deleter>
struct is_unique_ptr_of_container<std::unique_ptr<T, Deleter>> : std::is_base_of<Container, T> {};
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
} // anonymous namespace
// ---------------------------------------------------------------------------------------------------
enum SeparatorRestartSequenceEnum { RestartSequence };
// ---------------------------------------------------------------------------------------------------
template <typename... LCs>
[[maybe_unused]] FWContainer fmt(std::string fmtString, LCs&&... container) { return std::make_unique<FormatContainer>(std::move(fmtString), std::forward<LCs>(container)...); }
[[maybe_unused]] FWContainer separator(bool doSeparator = true, std::source_location loc = std::source_location::current());
[[maybe_unused]] FWContainer separator(SeparatorRestartSequenceEnum, bool doSeparator = true, std::source_location loc = std::source_location::current());
[[maybe_unused]] FWContainer pushSeparator(const std::string& val, bool newline = false, bool skipFirst = true);
[[maybe_unused]] FWContainer popSeparator(bool doPop = true);
[[maybe_unused]] FWContainer endl(std::source_location loc = std::source_location::current());
[[maybe_unused]] FWContainer sendl(std::source_location loc = std::source_location::current());
[[maybe_unused]] FWContainer endl_non_empty(std::source_location loc = std::source_location::current());
[[maybe_unused]] FWContainer breakpoint(std::source_location loc = std::source_location::current());
[[maybe_unused]] FWContainer condition(bool condition, FWContainer&& thenCase);
[[maybe_unused]] FWContainer condition(bool condition, FWContainer&& thenCase, FWContainer&& elseCase);
[[maybe_unused]] FWContainer str(const FWContainer&& fwc);
[[maybe_unused]] FWContainer str(const FWContainer& fwc);
[[maybe_unused]] FWContainer nop();
[[maybe_unused]] FWContainer get(size_t idx, FWContainer&& value);
[[maybe_unused]] FWContainer get(size_t idx, const FWContainer& value);
[[maybe_unused]] FWContainer context_switch();
[[maybe_unused]] FWContainer func(std::function<void(FileWriter&)>&& functor);
[[maybe_unused]] FWContainer dereference(FWContainer&& cont);
[[maybe_unused]] FWContainer verbosity(bool targetVerbose = true);
[[maybe_unused]] FWContainer checkQuotes(bool doCheck);
// ---------------------------------------------------------------------------------------------------
template <std::ranges::input_range Iterable>
static FWContainer iter(Iterable&& iterable, const std::string& separator = "", bool newline = false) {
   // skip first cannot be implemented easily here, since the iterator sequence is restarted at the beginning.
   if (separator.empty())
      return func([iterable](FileWriter& out) { out << iterable; });
   else
      return func([iterable, separator, newline](FileWriter& out) { out << pushSeparator(separator, newline) << iterable << popSeparator(); });
}
// ---------------------------------------------------------------------------------------------------
/// Default writer, writes to std::cout
static FileWriter nullStream = FileWriter(std::cout, FileWriter::NoFinalNewline);
static FileWriter silentCout = FileWriter(std::cout, FileWriter::Silent, FileWriter::NoFinalNewline);
// ---------------------------------------------------------------------------------------------------
FWContainer lc_helper(const std::string& firstV); // { return cast(std::make_unique<StringContainer>(firstV)); }
FWContainer lc_helper(const char* firstV); // { return cast(std::make_unique<StringContainer>(firstV)); }
FWContainer lc_helper(const std::string_view& firstV); // { return cast(std::make_unique<StringViewContainer>(firstV)); }
FWContainer lc_helper(const schemac::Type& firstV); // { return cast(std::make_unique<SchemaTypeContainer>(firstV)); }
//
FWContainer lc_helper(int firstV); // {  return cast(std::make_unique<IntContainer>(firstV)); }
FWContainer lc_helper(size_t firstV); // { return cast(std::make_unique<SizeTContainer>(firstV)); }
FWContainer lc_helper(long firstV); // { return cast(std::make_unique<SizeTContainer>(firstV)); }
FWContainer lc_helper(const IU& firstV); // {  return cast(std::make_unique<IuContainer>(firstV)); }
FWContainer lc_helper(const IU* firstV); // {  return cast(std::make_unique<IuContainer>(*firstV)); }
FWContainer lc_helper(const FWContainer& firstV); //  {return lcContainerImpl(firstV, std::false_type {}); }
FWContainer lc_helper(FWContainer&& firstV); // {return lcContainerImpl(std::forward<FWContainer>(firstV), std::true_type{}); }
FWContainer lc_helper(const Container& firstV); //  {return cast(std::make_unique<ReferenceContainer>(firstV)); }
// ---------------------------------------------------------------------------------------------------
template <typename FirstT, typename... Ts>
FWContainer lc(FirstT&& firstV, Ts&&... values) {
   if constexpr (sizeof...(Ts) == 0) {
      return lc_helper(std::forward<FirstT>(firstV));
   } else {
      std::unique_ptr<ListContainer> container = std::make_unique<ListContainer>();
      *container << lc_helper(std::forward<FirstT>(firstV));
      (*container << ... << lc_helper(std::forward<Ts>(values)));
      return {std::move(container)};
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_queryc_writer_fwutil