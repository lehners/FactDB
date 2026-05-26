// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/container/ReferenceContainer.hpp"
#include "factDB/queryc/writer/FWContainer.hpp"
#include "factDB/queryc/writer/container/PointerDereferenceContainer.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
ReferenceContainer::ReferenceContainer(const Container& ref) : Container(CT_Reference), reference(ref) {}
ReferenceContainer::ReferenceContainer(const std::unique_ptr<Container>& ref) : Container(CT_Reference), reference(*ref.get()) {}
ReferenceContainer::ReferenceContainer(const FWContainer& ref) : Container(CT_Reference), reference(*ref.container.get()) {}
// ---------------------------------------------------------------------------------------------------
PointerDereferenceContainer::PointerDereferenceContainer(FWContainer&& ref) : Container(CT_Star), reference(std::move(ref)) {}
PointerDereferenceContainer::PointerDereferenceContainer(const FWContainer& ref) : Container(CT_Star), reference(std::make_unique<ReferenceContainer>(ref)) {}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------