// ---------------------------------------------------------------------------------------------------
#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/list/FastDeque.hpp"
#include "factDB/infra/list/TLSIterator.hpp"
#include "factDB/infra/list/TLSIteratorThreadLocal.hpp"
#include "factDB/infra/threading/ThreadLocal.hpp"
#include "tbb/enumerable_thread_specific.h"
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <bool usePool, typename ListType, typename ElementType>
struct IteratorSelector {
   using Const = TLSIteratorThreadLocal<ListType, ElementType, true>;
   using NonConst = TLSIteratorThreadLocal<ListType, ElementType, false>;
};
// ---------------------------------------------------------------------------------------------------
template <typename ListType, typename ElementType>
struct IteratorSelector<false, ListType, ElementType> {
   using Const = TLSIterator<ListType, ElementType, true>;
   using NonConst = TLSIterator<ListType, ElementType, false>;
};
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam, bool usePool = true>
class TLFastDeque {
   public:
   using ListType = std::conditional_t<usePool, OwningThreadLocal<FastDeque<ElementTypeParam>>, tbb::enumerable_thread_specific<FastDeque<ElementTypeParam>>>;
   using ElementType = ListType::value_type::ElementType;
   using reference = ListType::reference;

   using ConstIterator = typename IteratorSelector<usePool, ListType, ElementType>::Const;
   using Iterator = typename IteratorSelector<usePool, ListType, ElementType>::NonConst;

   ListType list;

   TLFastDeque() noexcept = default;
   ~TLFastDeque() = default;

   reference local() { return list.local(); }

   template <typename... Args>
   Iterator emplace_back(reference& local, Args... args) {
      auto iterator = local.emplace_back(args...);
      return {list.begin(), iterator};
   }

   [[nodiscard]] bool empty() const {
      for (auto& e : list)
         if (!e.empty()) return false;
      return true;
   }
   [[nodiscard]] bool containsObject(ElementType&) const { not_implemented(); }
   [[nodiscard]] size_t size() const {
      size_t sizeVal = 0;
      for (auto& e : list)
         sizeVal += e.size();
      return sizeVal;
   }
   [[nodiscard]] const ElementType& back() const { not_implemented(); }

   void compactLastChunk() {}
   void reserve(size_t /*size*/) { not_implemented(); }

   [[nodiscard]] bool operator==(TLFastDeque&) const { not_implemented(); }
   [[nodiscard]] const ElementType& operator[](size_t /*idx*/) const { not_implemented(); }
   [[nodiscard]] ElementType& operator[](size_t idx) { return *(begin() + idx); }

   [[nodiscard]] Iterator begin() { return Iterator::get_begin(list); }
   [[nodiscard]] Iterator end() { return {list.end(), {}}; }
   [[nodiscard]] ConstIterator begin() const { return ConstIterator::get_begin(list); }
   [[nodiscard]] ConstIterator end() const { return {list.end(), {}}; }
};
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
using TbbTLFastDeque = TLFastDeque<ElementTypeParam, false>;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------