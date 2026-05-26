#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include "factDB/infra/list/TLSIterator.hpp"
#include "tbb/enumerable_thread_specific.h"
#include "tbb/scalable_allocator.h"
#include <deque>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
struct TLSDeque {
   using ElementType = ElementTypeParam;
   using ListType = tbb::enumerable_thread_specific<std::deque<ElementType, tbb::scalable_allocator<ElementType>>>;
   using reference = ListType::reference;

   using Iterator = TLSIterator<ListType, ElementType, false>;
   using ConstIterator = TLSIterator<ListType, ElementType, true>;

   // the list
   ListType list;

   TLSDeque() noexcept = default;
   ~TLSDeque() = default;

   reference local() { return list.local(); }

   template <typename... Args>
   Iterator emplace_back(Args... args) {
      list.local().emplace_back(std::forward<Args>(args)...);
      return {list.begin(), list.local().begin() + list.local().size() - 1};
   } // todo fixme

   void compactLastChunk() {}
   void reserve(size_t /*size*/) { not_implemented(); }
   void resize(size_t size) {
      if (list.empty())
         list.local();
      assert(!list.empty());
      list.begin()->resize(size);
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

   [[nodiscard]] const ElementType& back() const { return list.local().back(); }
   [[nodiscard]] bool operator==(TLSDeque& other) const { return list == other.list; }
   [[nodiscard]] const ElementType& operator[](size_t /*idx*/) const { not_implemented(); }
   [[nodiscard]] ElementType& operator[](size_t idx) { return *(begin() + idx); }

   [[nodiscard]] Iterator begin() { return Iterator::get_begin(list); }
   [[nodiscard]] Iterator end() { return Iterator(list.end(), {}); }
   [[nodiscard]] ConstIterator begin() const { return ConstIterator::get_begin(list); }
   [[nodiscard]] ConstIterator end() const { return ConstIterator(list.end(), {}); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------
