// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/FWUtil.hpp"
#include "factDB/queryc/writer/container/Container.hpp"
#include "factDB/queryc/writer/container/FunctorContainer.hpp"
#include "factDB/queryc/writer/container/SeparatorContainer.hpp"
#include <source_location>
#include <vector>

#include "factDB/queryc/writer/FWContainer.hpp"
#include "factDB/queryc/writer/container/ConditionalContainer.hpp"
#include "factDB/queryc/writer/container/Container.hpp"
#include "factDB/queryc/writer/container/PointerDereferenceContainer.hpp"
#include "factDB/queryc/writer/container/ReferenceContainer.hpp"

#include "factDB/queryc/writer/container/FunctorContainer.hpp"
#include "factDB/queryc/writer/container/NewlineContainer.hpp"
#include "factDB/queryc/writer/container/SeparatorContainer.hpp"
#include "factDB/queryc/writer/container/TrivialContainer.hpp"

// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
template <std::derived_from<Container> T>
FWContainer cast(std::unique_ptr<T> t) {
   std::unique_ptr<Container> ptr = std::move(t);
   return ptr;
}
// ---------------------------------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------------------------------
FWContainer separator(bool doSeparator, std::source_location loc) {
   return std::make_unique<SeparatorContainer>(doSeparator, false, loc);
}
// ---------------------------------------------------------------------------------------------------
FWContainer separator(SeparatorRestartSequenceEnum, bool doSeparator, std::source_location loc) {
   return std::make_unique<SeparatorContainer>(doSeparator, true, loc);
}
// ---------------------------------------------------------------------------------------------------
FWContainer pushSeparator(const std::string& val, bool newline, bool skipFirst) {
   return cast(std::make_unique<SeparatorInfo>(val, newline, skipFirst));
}
// ---------------------------------------------------------------------------------------------------
FWContainer popSeparator(bool doPop) {
   return doPop ? cast(std::make_unique<Container>(CT_SeparatorPop)) : cast(std::make_unique<NopContainer>());
}
// ---------------------------------------------------------------------------------------------------
FWContainer endl(std::source_location loc) {
   return std::make_unique<NewlineContainer>(false, false, loc);
}
// ---------------------------------------------------------------------------------------------------
FWContainer sendl(std::source_location loc) {
   return std::make_unique<NewlineContainer>(true, false, loc);
}
// ---------------------------------------------------------------------------------------------------
FWContainer endl_non_empty(std::source_location loc) {
   return std::make_unique<NewlineContainer>(false, true, loc);
}
// ---------------------------------------------------------------------------------------------------
FWContainer breakpoint(std::source_location loc) {
   return std::make_unique<BreakpointContainer>(loc);
}
// ---------------------------------------------------------------------------------------------------
FWContainer condition(bool condition, FWContainer&& thenCase) {
   return std::make_unique<ConditionalContainer>(condition, std::forward<FWContainer>(thenCase), std::forward<FWContainer>(std::make_unique<NopContainer>()));
}
// ---------------------------------------------------------------------------------------------------
FWContainer condition(bool condition, FWContainer&& thenCase, FWContainer&& elseCase) {
   return std::make_unique<ConditionalContainer>(condition, std::forward<FWContainer>(thenCase), std::forward<FWContainer>(elseCase));
}
// ---------------------------------------------------------------------------------------------------
FWContainer str(const FWContainer&& fwc) {
   return std::make_unique<StringEscapeContainer>(fwc);
}
// ---------------------------------------------------------------------------------------------------
FWContainer str(const FWContainer& fwc) {
   return std::make_unique<StringEscapeContainer>(fwc);
}
// ---------------------------------------------------------------------------------------------------
FWContainer nop() {
   return std::make_unique<NopContainer>();
}
// ---------------------------------------------------------------------------------------------------
FWContainer get(size_t idx, FWContainer&& value) {
   return fmt("std::get<{}>({})", idx, std::forward<FWContainer>(value));
}
// ---------------------------------------------------------------------------------------------------
FWContainer get(size_t idx, const FWContainer& value) {
   return fmt("std::get<{}>({})", idx, value);
}
// ---------------------------------------------------------------------------------------------------
FWContainer context_switch() {
   return std::make_unique<StringContainer>("/*---------------------------------------------------------------------------*/");
}
// ---------------------------------------------------------------------------------------------------
FWContainer func(std::function<void(FileWriter&)>&& functor) {
   return cast(std::make_unique<FunctorContainer>(std::forward<std::function<void(FileWriter&)>>(functor)));
}
// ---------------------------------------------------------------------------------------------------
FWContainer verbosity(bool targetVerbose) {
   return cast(std::make_unique<VerboseContainer>(targetVerbose));
}
// ---------------------------------------------------------------------------------------------------
FWContainer dereference(FWContainer&& cont) {
   return cast(std::make_unique<PointerDereferenceContainer>(std::forward<FWContainer>(cont)));
}
// ---------------------------------------------------------------------------------------------------
FWContainer checkQuotes(bool doCheck) {
   return cast(std::make_unique<CheckQuoteContainer>(doCheck));
}
// ---------------------------------------------------------------------------------------------------
FWContainer lc_helper(const std::string& firstV) { return cast(std::make_unique<StringContainer>(firstV)); }
FWContainer lc_helper(const char* firstV) { return cast(std::make_unique<StringContainer>(firstV)); }
FWContainer lc_helper(const std::string_view& firstV) { return cast(std::make_unique<StringViewContainer>(firstV)); }
FWContainer lc_helper(const schemac::Type& firstV) { return cast(std::make_unique<SchemaTypeContainer>(firstV)); }
FWContainer lc_helper(int firstV) { return cast(std::make_unique<IntContainer>(firstV)); }
FWContainer lc_helper(size_t firstV) { return cast(std::make_unique<SizeTContainer>(firstV)); }
FWContainer lc_helper(long firstV) { return cast(std::make_unique<SizeTContainer>(firstV)); }
FWContainer lc_helper(const IU& firstV) { return cast(std::make_unique<IuContainer>(firstV)); }
FWContainer lc_helper(const IU* firstV) { return cast(std::make_unique<IuContainer>(*firstV)); }
FWContainer lc_helper(const FWContainer& firstV) { return lcContainerImpl(firstV, std::false_type{}); }
FWContainer lc_helper(FWContainer&& firstV) { return lcContainerImpl(std::forward<FWContainer>(firstV), std::true_type{}); }
FWContainer lc_helper(const Container& firstV) { return cast(std::make_unique<ReferenceContainer>(firstV)); }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------
