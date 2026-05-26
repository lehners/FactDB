#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/list/ListTypeEnum.hpp"
#include "factDB/newftree/generator/util/FGenUtil.hpp"
#include "factDB/newftree/generator/util/FNodeGenerator.hpp"
#include <ranges>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct FNodeListTypes : public FNodeGenerator {
   explicit FNodeListTypes(FileWriter& o) : FNodeGenerator(o) {}

   void visit(const FNodeReferencing& node) override;
   void visit(const FNodeOwning& node) override;
   void visitRoot(const FNodeOwning& node) override;

   static std::pair<infra::list::ListTypeEnum, bool> getListType(bool isRoot, bool needsLock);

   private:
   void genTypeString(const FNode& node, bool owning, bool isRoot);
   std::string getListClass(bool isRoot, bool needsLock, bool owning);
};
// ---------------------------------------------------------------------------------------------------
inline std::pair<infra::list::ListTypeEnum, bool> FNodeListTypes::getListType(bool isRoot, bool needsLock) {
   auto tlsBase = SettingBase::getSetting<bool>("codegen.factorized.tlsBase")->get();
   auto usePool = SettingBase::getSetting<bool>("codegen.threadPool")->get();
   auto listType = SettingBase::getSetting<infra::list::ListTypeEnum>("codegen.factorized.listType")->get();
   if (isRoot && tlsBase) {
      if (usePool) {
         listType = infra::list::ListTypeEnum::TLFastDequeList;
      } else {
         listType = infra::list::ListTypeEnum::TbbTLFastDequeList;
      }
   }
   switch (listType) {
      case infra::list::ListTypeEnum::STLVector:
      case infra::list::ListTypeEnum::STLDeque:
         assert(infra::list::hasLock(listType));
         return std::make_pair(listType, needsLock);
      case infra::list::ListTypeEnum::FastDequeEnum:
         assert(infra::list::hasLock(listType));
         if (SettingBase::getSetting<bool>("codegen.factorized.inline")->get()) {
            return std::make_pair(infra::list::ListTypeEnum::FastDequeEnumInlining, needsLock);
         } else {
            return std::make_pair(infra::list::ListTypeEnum::FastDequeEnum, needsLock);
         }
      case infra::list::ListTypeEnum::Auto:
         if (needsLock) {
            return std::make_pair(infra::list::ListTypeEnum::TbbConcurrentVector, false);
         } else if (SettingBase::getSetting<bool>("codegen.factorized.inline")->get()) {
            return std::make_pair(infra::list::ListTypeEnum::FastDequeEnumInlining, false);
         } else {
            return std::make_pair(infra::list::ListTypeEnum::FastDequeEnum, false);
         }
      default:
         assert(!infra::list::hasLock(listType));
         return std::make_pair(listType, false);
   }
}
// ---------------------------------------------------------------------------------------------------
inline std::string FNodeListTypes::getListClass(bool isRoot, bool needsLock, bool owning) {
   auto [listType, needLockReally] = getListType(isRoot, needsLock);
   return infra::list::toClassName(listType, owning) + (needLockReally && owning ? "Lockable" : "");
}
// ---------------------------------------------------------------------------------------------------
inline void FNodeListTypes::genTypeString(const FNode& node, bool owning, bool isRoot) {
   bool sizePerList = factDB::SettingBase::getSetting<bool>("codegen.factorized.sizePerList")->get();

   assert(!node.getIUs().empty());

   auto sizeContainer = fw::condition(sizePerList, "size_t /*iterated subtree size*/, ");
   auto iuTypes = fw::lc(FTreeUtil(node).genTypes(), fw::separator(!node.getChildren().empty()));
   auto childContainer = fw::iter(node.getChildren() | std::views::transform([](const auto& child) { return FTreeUtil(*child).listType(); }));

   out << fw::fmt("using {} = /*{}*/ {}<std::tuple<{}{}{}>>;", FTreeUtil(node).listType(), node.getIUs().front(), getListClass(isRoot, node.needsLock(), owning), sizeContainer, iuTypes, childContainer) << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
inline void FNodeListTypes::visitRoot(const FNodeOwning& node) {
   recurseChildren(node);
   genTypeString(node, true, true);
}
// ---------------------------------------------------------------------------------------------------
inline void FNodeListTypes::visit(const FNodeOwning& node) {
   recurseChildren(node);
   genTypeString(node, true, false);
}
// ---------------------------------------------------------------------------------------------------
inline void FNodeListTypes::visit(const FNodeReferencing& node) {
   recurseChildren(node);
   genTypeString(node, false, false);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
