#include "factDB/queryc/writer/FWContainer.hpp"
#include "factDB/queryc/writer/container/ConditionalContainer.hpp"
#include "factDB/queryc/writer/container/Container.hpp"
#include "factDB/queryc/writer/container/FormatContainer.hpp"
#include "factDB/queryc/writer/container/NewlineContainer.hpp"
#include "factDB/queryc/writer/container/PointerDereferenceContainer.hpp"
#include "factDB/queryc/writer/container/ReferenceContainer.hpp"
#include "factDB/queryc/writer/container/SeparatorContainer.hpp"
#include "factDB/queryc/writer/container/TrivialContainer.hpp"
#include <memory>
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
FWContainer lcContainerImpl(const FWContainer& value, std::false_type) {
   return std::make_unique<ReferenceContainer>(value);
}
// ---------------------------------------------------------------------------------------------------
FWContainer lcContainerImpl(FWContainer&& value, std::true_type) {
   return std::move(value);
}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer()
   : container(std::make_unique<Container>()) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(FWContainer& val)
   : container(std::make_unique<ReferenceContainer>(val)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(FWContainer&&) noexcept = default;
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<Container>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<BreakpointContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<ConditionalContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<FormatContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<GetContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<IntContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<IuContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<ListContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<NewlineContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<NopContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<ReferenceContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<SchemaTypeContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<SeparatorContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<SizeTContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<StringContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<StringEscapeContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(std::unique_ptr<UnsignedContainer>&& cont)
   : container(std::move(cont)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(const std::string& ptr)
   : container(std::make_unique<StringContainer>(ptr)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(const char* ptr)
   : container(std::make_unique<StringContainer>(ptr)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(const char* ptr, size_t val)
   : container(std::make_unique<StringContainer>(std::string(ptr) + std::to_string(val))) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(const std::string_view& val)
   : container(std::make_unique<StringViewContainer>(val)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(int val)
   : container(std::make_unique<IntContainer>(val)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(uint64_t val)
   : container(std::make_unique<SizeTContainer>(val)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(unsigned val)
   : container(std::make_unique<SizeTContainer>(val)) {}
// ---------------------------------------------------------------------------------------------------
#ifdef __APPLE__
FWContainer::FWContainer(size_t val)
   : container(std::make_unique<SizeTContainer>(val)) {}
#endif
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(const IU& val)
   : container(std::make_unique<IuContainer>(val)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(const IU* val)
   : container(std::make_unique<IuContainer>(*val)) {}
// ---------------------------------------------------------------------------------------------------
FWContainer::FWContainer(const schemac::Type& val)
   : container(std::make_unique<SchemaTypeContainer>(val)) {}
// ---------------------------------------------------------------------------------------------------
bool FWContainer::isNOP() const {
   return container->isNOP();
}
// ---------------------------------------------------------------------------------------------------
FWContainer FWContainer::dereference() const {
   return FWContainer(std::make_unique<PointerDereferenceContainer>(*this));
}
// ---------------------------------------------------------------------------------------------------
FWContainer FWContainer::dereference() {
   return FWContainer(std::make_unique<PointerDereferenceContainer>(std::move(*this)));
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------