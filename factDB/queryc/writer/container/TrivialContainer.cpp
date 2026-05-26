// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/container/TrivialContainer.hpp"
#include "factDB/queryc/writer/container/ConditionalContainer.hpp"
#include "factDB/queryc/writer/container/ListContainer.hpp"

#include "factDB/schemac/Type.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
SchemaTypeContainer::SchemaTypeContainer(const schemac::Type& v) : Container(CT_SchemaType), val(v) {
}
// ---------------------------------------------------------------------------------------------------
GetContainer::GetContainer(size_t idx, ListContainer& var) : Container(CT_Get), position(idx), variable(var) {
}
// ---------------------------------------------------------------------------------------------------
StringEscapeContainer::StringEscapeContainer(const FWContainer& ref) : Container(CT_StringEscape), reference(*ref.container.get()) {
}
// ---------------------------------------------------------------------------------------------------
ConditionalContainer::ConditionalContainer(bool cond, factDB::fw::FWContainer&& tCase)
   : Container(CT_Conditional), condition(cond), trueCase(std::forward<FWContainer>(tCase)), falseCase(std::make_unique<NopContainer>()) {
}

} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------