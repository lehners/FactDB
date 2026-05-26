#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/IUSet.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/newftree/FIterator.hpp"
#include "factDB/newftree/FNode.hpp"
#include "factDB/newftree/FTree.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
#include <ranges>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct FHandle {
   std::string val;
   size_t id;
   explicit FHandle(std::string v = {}) : val(std::move(v)) {}
   explicit FHandle(std::string v, size_t idx) : val(std::move(v)), id(idx) {}
   [[nodiscard]] bool isEmpty() const { return val.empty(); }
   [[nodiscard]] fw::FWContainer gen() const { return fw::fmt("{}{}", val, id); }
   void setUniqueID();

   static void resetCounters();
};
// ---------------------------------------------------------------------------------------------------
struct FTreeUtil {
   const FNode& node;

   explicit FTreeUtil(const FNode& nodeP) : node(nodeP) {}

   inline FWContainer listType() const { return "ListType" + std::to_string(node.getNodeId()); }
   inline FWContainer iteratorType() const { return "ListType" + std::to_string(node.getNodeId()) + "::Iterator"; }

   inline FWContainer elementType() const { return fw::fmt("{}::ElementType", listType()); }
   inline FWContainer iteratorVar() { return fw::fmt("iter{}", node.getNodeId()); }

   static inline FWContainer getIteratorName(const FIterator& iterator) { return fw::fmt("FactorizedIterator{}", iterator.getIteratorID()); }
   static FWContainer getQualifiedIteratorName(const FIterator& iterator);

   FWContainer genParamList();
   FWContainer genTypes(bool references = false);

   static size_t getSizeIdx();
   size_t getIUIdx(const IU& iu) const;
   size_t getChildIdx(const FNode& child) const;
   size_t getChildIdx(size_t childIdx) const;
};
// ---------------------------------------------------------------------------------------------------
struct FIteratorTranslator;
// ---------------------------------------------------------------------------------------------------
struct FTreeTranslator {
   const FTree& tree_;
   FWContainer name_;
   explicit FTreeTranslator(const FTree& treeP, FWContainer nameP = fw::nop()) : tree_(treeP), name_(std::move(nameP)) {}

   const FNode* getIUOwner(const IU& pivot);
   const FTree& getFTree() const { return tree_; }

   inline FWContainer classname() const { return fw::fmt("FactorizedTable{}", tree_.getRootNode().getNodeId()); }
   inline FWContainer reference() const { return fw::fmt("FactorizedTable{}&", tree_.getRootNode().getNodeId()); }
   FWContainer iuEnum(const IU& iu) const { return fw::fmt("{}::{}", classname(), iu); }

   FWContainer instance() const { return fw::fmt("{} {}", classname(), name_); }
   FWContainer genBegin(const FIterator& iter) const { return fw::fmt("{}.begin{}()", name_, iter.getIteratorID()); }
   FWContainer name() const { return fw::lc(name_); }
   FWContainer size() const { return fw::fmt("{}.size()", name_); }
   FWContainer reserve(const FHandle& handle, const IU& pivot, const FWContainer& size);

   FWContainer push_back(const FHandle& parentHandle, FHandle& childHandle, const IU& pivot, std::initializer_list<FWContainer>&& values, const FWContainer& tlsLocal = {});
   FWContainer push_back(const FHandle& parentHandle, FHandle& childHandle, const OrderedIUSet& iuSet, const FWContainer& tlsLocal = {});
   FWContainer push_back(FHandle& childHandle, const IU& pivot, std::initializer_list<FWContainer>&& values, const FWContainer& tlsLocal = {}) { return push_back(FHandle{}, childHandle, pivot, std::forward<std::initializer_list<FWContainer>>(values), tlsLocal); }
   FWContainer push_back(FHandle& childHandle, const OrderedIUSet& values, const FWContainer& tlsLocal = {}) { return push_back(FHandle{}, childHandle, values, tlsLocal); }

   FWContainer getInsertHandleType() const;
   FWContainer handleStorage() const { return fw::fmt("hs{}", tree_.getRootNode().getNodeId()); }
   FWContainer genHandleStorage() const { return fw::fmt("{}::HandleStorage {};", classname(), handleStorage()); }
   FWContainer insertHandleStorage() const { return fw::fmt("ihs{}", tree_.getRootNode().getNodeId()); }
   FWContainer genInsertHandleStorage() const { return fw::fmt("{}::InsertHandleStorage {};", classname(), insertHandleStorage()); }
   FWContainer getIteratorRef(const FNode* node) const { return fw::fmt("{}.get{}Ref()", name(), FTreeUtil(*node).iteratorVar()); }
   FWContainer constructMergingIterator() const;
   FWContainer insertAsChild(const FHandle& handle, bool useInsertHandleStorage = true);
   FWContainer appendEmptyTree(const FHandle& handle, const FWContainer& tlsLocal, bool useHandleStorage = true);

   FIteratorTranslator getIteratorTranslator(const FIterator& iterator, FWContainer instanceName = fw::nop()) const;
   FIteratorTranslator getFrontIteratorTranslator(FWContainer instanceName = fw::nop()) const;
   FIteratorTranslator getMergingIteratorTranslator(FWContainer instanceName = fw::nop()) const;
   FIteratorTranslator getTopIteratorTranslator(FWContainer instanceName = fw::nop()) const;
   FIteratorTranslator getLowerIteratorTranslator(FWContainer instanceName = fw::nop()) const;
   auto& getIterators() const { return tree_.getIterators(); }
   auto isComposedTree() const { return tree_.isComposedTree(); }

   bool hasComposedSubiterator() const;
};
// ---------------------------------------------------------------------------------------------------
struct FIteratorTranslator {
   const FIterator& iterator;
   /// the name of this instance
   FWContainer instanceName;
   FWContainer ftreeName;

   FWContainer name() { return instanceName; }
   FIteratorTranslator(const FTreeTranslator& ownerTranslatorP, const FIterator& iteratorP, FWContainer instanceNameP) : iterator(iteratorP), instanceName(std::move(instanceNameP)), ftreeName(ownerTranslatorP.name()) {}
   FIteratorTranslator(const FIterator& iteratorP, FWContainer instanceNameP) : iterator(iteratorP), instanceName(std::move(instanceNameP)), ftreeName(fw::nop()) {}
   FIteratorTranslator(const FIterator& iteratorP) : iterator(iteratorP), instanceName(getDefaultName(iterator)), ftreeName(fw::nop()) {}

   FTreeTranslator getOwnerTranslator() const { return FTreeTranslator(iterator.getOwner(), fw::lc(ftreeName)); }

   FWContainer getClassname() const { return fw::fmt("{}::{}", getOwnerTranslator().classname(), FTreeUtil::getIteratorName(iterator)); }
   FWContainer getQualifiedIteratorName() const { return FTreeUtil::getQualifiedIteratorName(iterator); }

   FWContainer iterableSize() {
      return fw::fmt("{}::iterableSize({})", getClassname(), ftreeName);
   }
   FWContainer iteratedTupleSize(bool useCache = true) { return fw::fmt("{}.iteratedTupleSize({})", instanceName, useCache); }
   FWContainer isLast() { return fw::fmt("{}.isLast()", name()); }
   FWContainer beginAt(const FWContainer& idx, const FWContainer& bounds) {
      assert(!iterator.hasReferencedIterator());
      return fw::fmt("{}::beginAt({}, {}, {})", getClassname(), ftreeName, idx, bounds);
   }

   FWContainer get(const IU& iu);

   FWContainer begin() { return fw::fmt("{}.begin{}()", ftreeName, iterator.getIteratorID()); }
   FWContainer dereferenceIUs() { return fw::fmt("[[maybe_unused]] const auto& [{}] = *{};", fw::iter(iterator.getIteratedIUs()), name()); }

   FWContainer fromReferencedIterator(const FIteratorTranslator& other) const {
      assert(iterator.hasReferencedIterator());
      return fw::fmt("{}::fromReferencedIterator({})", getClassname(), other.instanceName);
   }
   FWContainer fromReferencedIterator(const FWContainer& cont) const {
      assert(iterator.hasReferencedIterator());
      return fw::fmt("{}::fromReferencedIterator({})", getClassname(), cont);
   }

   bool hasIteratedIUs() const { return !iterator.getIteratedIUs().empty(); }
   bool hasReferencedIterator() const { return iterator.hasReferencedIterator(); }

   const OrderedIUSet& getIteratedIUs() const { return iterator.getIteratedIUs(); }
   FIteratorTranslator getReferencedIteratorTranslator(FWContainer instanceName = fw::nop()) const;

   static FWContainer getDefaultName(const FIterator& iter) {
      return fw::fmt("fti{}Iterator{}", iter.getOwner().getRootNode().getNodeId(), iter.getIteratorID());
   }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
