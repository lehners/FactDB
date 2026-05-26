#ifndef H_factdb_queryc_writer_fwcontainer
#define H_factdb_queryc_writer_fwcontainer
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/container/Container.hpp"
#include <memory>
#include <string_view>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
class IU;
} // namespace factDB
namespace factDB::schemac {
struct Type;
} // namespace factDB::schemac
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw {
// ---------------------------------------------------------------------------------------------------
struct BreakpointContainer;
struct ConditionalContainer;
struct FormatContainer;
struct GetContainer;
struct IntContainer;
struct IuContainer;
struct ListContainer;
struct NewlineContainer;
struct NopContainer;
struct ReferenceContainer;
struct SchemaTypeContainer;
struct SeparatorContainer;
struct SizeTContainer;
struct StringContainer;
struct StringEscapeContainer;
struct UnsignedContainer;
struct FWContainer;
// ---------------------------------------------------------------------------------------------------
FWContainer lcContainerImpl(const FWContainer& value, std::false_type);
FWContainer lcContainerImpl(FWContainer&& value, std::true_type);
// ---------------------------------------------------------------------------------------------------
struct FWContainer {
   std::unique_ptr<Container> container;

   FWContainer();
   FWContainer(FWContainer& val);
   FWContainer(FWContainer&&) noexcept;
   FWContainer& operator=(FWContainer&&) = default;
   // FWContainer(const FWContainer&, std::true_type);
   // FWContainer(FWContainer&&, std::false_type);

   FWContainer(std::unique_ptr<Container>&& cont);
   FWContainer(std::unique_ptr<BreakpointContainer>&& cont);
   FWContainer(std::unique_ptr<ConditionalContainer>&& cont);
   FWContainer(std::unique_ptr<FormatContainer>&& cont);
   FWContainer(std::unique_ptr<GetContainer>&& cont);
   FWContainer(std::unique_ptr<IntContainer>&& cont);
   FWContainer(std::unique_ptr<IuContainer>&& cont);
   FWContainer(std::unique_ptr<ListContainer>&& cont);
   FWContainer(std::unique_ptr<NewlineContainer>&& cont);
   FWContainer(std::unique_ptr<NopContainer>&& cont);
   FWContainer(std::unique_ptr<ReferenceContainer>&& cont);
   FWContainer(std::unique_ptr<SchemaTypeContainer>&& cont);
   FWContainer(std::unique_ptr<SeparatorContainer>&& cont);
   FWContainer(std::unique_ptr<SizeTContainer>&& cont);
   FWContainer(std::unique_ptr<StringContainer>&& cont);
   FWContainer(std::unique_ptr<StringEscapeContainer>&& cont);
   FWContainer(std::unique_ptr<UnsignedContainer>&& cont);

   FWContainer(const std::string& ptr);
   FWContainer(const char* ptr);
   FWContainer(const char* ptr, size_t val);
   FWContainer(const std::string_view& ptr);
   FWContainer(const int val);
   FWContainer(const unsigned val);
   FWContainer(const uint64_t val);
#ifdef __APPLE__
   FWContainer(const size_t val);
#endif
   FWContainer(const IU& val);
   FWContainer(const IU* val);
   FWContainer(const schemac::Type& ptr);

   template <typename T>
   static FWContainer gen(T&& val);

   Container* get() { return container.get(); }
   [[nodiscard]] bool isNOP() const;
   [[nodiscard]] FWContainer dereference() const;
   [[nodiscard]] FWContainer dereference();
};
// ---------------------------------------------------------------------------------------------------
template <typename T>
FWContainer FWContainer::gen(T&& val) {
   if constexpr (std::is_same_v<std::remove_cvref_t<T>, FWContainer>) {
      return lcContainerImpl(std::forward<T>(val), std::is_rvalue_reference<T&&>());
   } else {
      return FWContainer(std::forward<T>(val));
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::fw
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
using FWContainer = fw::FWContainer;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_queryc_writer_container