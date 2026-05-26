#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/list/ListTypeEnum.hpp"
#include "factDB/newftree/generator/util/FGenUtil.hpp"
#include "factDB/newftree/generator/util/FNodeGenerator.hpp"
#include "factDB/util/TemporaySetter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct FTreePushBack : public FNodeGenerator {
   private:
   FWContainer currentList = fw::lc("cl");
   FWContainer parentHandleType = fw::nop();
   bool genPushBacks = true;

   public:
   FTreePushBack(FileWriter& o, bool genPushBacks_ = true) : FNodeGenerator(o), genPushBacks(genPushBacks_){};
   void visit(const FNodeOwning& node) override;
   void visit(const FNodeReferencing& node) override;

   static FWContainer getPushbackEnum(const FNode& node) { return fw::fmt("PBEnum{}", node.getNodeId()); }
   static FWContainer getPushbackEnumValue(const FNode& node, const IU& iu) { return fw::fmt("PBEnum{}::{}", node.getNodeId(), iu); }
};
// ---------------------------------------------------------------------------------------------------
inline void FTreePushBack::visit(const FNodeReferencing& node) {
   FWContainer pushbackEnum = getPushbackEnum(node);
   FWContainer handleParameter = fw::condition(!parentHandleType.isNOP(), fw::fmt("{}& parentHandle, ", parentHandleType));

   out << fw::pushSeparator(", ")
       << fw::fmt("enum {} {{ {} }};", pushbackEnum, fw::iter(node.getIUs())) << fw::endl()
       << fw::fmt("// pushback not generated for reference node ({})", pushbackEnum) << fw::endl()
       << fw::endl() << fw::popSeparator();

   // generate for all children
   for (const auto& child : node.getChildren()) {
      TemporarySetter tsCurrentList(currentList, fw::get(FTreeUtil(node).getChildIdx(*child), "parentHandle"));
      TemporarySetter tsParentHandleType(parentHandleType, FTreeUtil(node).elementType());
      TemporarySetter tsGenPushBack(genPushBacks, false);
      apply(*child);
   }
}
// ---------------------------------------------------------------------------------------------------
inline void FTreePushBack::visit(const FNodeOwning& node) {
   FWContainer pushbackEnum = getPushbackEnum(node);
   FWContainer handleParameter = fw::condition(!parentHandleType.isNOP(), fw::fmt("{}& parentHandle, ", parentHandleType));
   auto doTlsLists = factDB::SettingBase::getSetting<bool>("codegen.factorized.tlsBase")->get();
   auto listType = SettingBase::getSetting<factDB::infra::list::ListTypeEnum>("codegen.factorized.listType")->get();

   out << fw::pushSeparator(", ")
       << fw::fmt("enum {} {{ {} }};", pushbackEnum, fw::iter(node.getIUs())) << fw::endl();
   if (genPushBacks) {
      if (doTlsLists && parentHandleType.isNOP()) { // root + thread_local list push_back
         out << fw::fmt("{}::reference local() {{ return cl.local(); }}", FTreeUtil(node).listType()) << fw::endl()
             << fw::fmt("{0}::Iterator push_back({1}, {2}, {0}::reference local) {{", FTreeUtil(node).listType(), pushbackEnum, FTreeUtil(node).genParamList()) << fw::endl();
         if (listType == infra::list::TLFastDequeList || listType == infra::list::TbbTLFastDequeList || listType == infra::list::FastDequeEnum || listType == infra::list::FastDequeEnumInlining) {
            out << fw::fmt("{}::Iterator handle = cl.emplace_back(local);", FTreeUtil(node).listType()) << fw::endl();
         } else {
            out << fw::fmt("local.emplace_back();") << fw::endl()
                << fw::fmt("{}::Iterator handle(cl.list.begin(), local.end() - 1);", FTreeUtil(node).listType(), currentList) << fw::endl();
         }
      } else {
         out << fw::fmt("{}::Iterator push_back({}{}, {}) {{", FTreeUtil(node).listType(), handleParameter, pushbackEnum, FTreeUtil(node).genParamList()) << fw::endl()
             << fw::popSeparator() // body:
             << fw::condition(infra::list::hasLock(listType) && node.needsLock(), fw::fmt("auto locked = std::lock_guard({}.lock);", currentList)) << fw::endl_non_empty()
             << fw::fmt("auto handle = {}.emplace_back();", currentList) << fw::endl();
      }

      for (const IU* iu : node.getIUs()) {
         out << fw::fmt("{} = {};", fw::get(FTreeUtil(node).getIUIdx(*iu), "*handle"), iu) << fw::endl();
      }
      out << "  return handle;" << fw::endl()
          << "}" << fw::endl()
          << fw::endl();
   } else {
      out << fw::fmt("// pushback not generated for children of reference node ({})", pushbackEnum) << fw::endl()
          << fw::endl();
   }

   // generate for all children
   for (const auto& child : node.getChildren()) {
      TemporarySetter tsCurrentList(currentList, fw::get(FTreeUtil(node).getChildIdx(*child), "parentHandle"));
      TemporarySetter tsParentHandleType(parentHandleType, FTreeUtil(node).elementType());
      apply(*child);
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
