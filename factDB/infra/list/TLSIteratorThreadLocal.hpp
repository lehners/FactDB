#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include "factDB/infra/threading/ThreadLocal.hpp"
#include "tbb/concurrent_vector.h"
#include "tbb/enumerable_thread_specific.h"
#include <type_traits>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <typename T>
concept IsOwningThreadLocal = requires {
   typename T::value_type;
   requires std::is_same_v<T, factDB::OwningThreadLocal<typename T::value_type>>;
};
// ---------------------------------------------------------------------------------------------------
template <typename ListType, typename ElementType, bool isConst>
   requires IsOwningThreadLocal<ListType>
class TLSIteratorThreadLocal {
   using value_type = std::conditional_t<isConst, const ElementType, ElementType>;
   using IteratorTLSType = std::conditional_t<isConst, typename ListType::const_iterator, typename ListType::iterator>;
   using ListIteratorType = std::conditional_t<isConst, typename ListType::value_type::const_iterator, typename ListType::value_type::iterator>;

   IteratorTLSType iteratorTLS;
   ListIteratorType iteratorList;

   public:
   TLSIteratorThreadLocal(const IteratorTLSType& t, const ListIteratorType& l) : iteratorTLS(t), iteratorList(l) {
   }
   TLSIteratorThreadLocal() = default;

   // Pre-increment
   TLSIteratorThreadLocal& operator++() {
      if (!iteratorTLS.isLast()) {
         if (++iteratorList == iteratorTLS->end()) {
            do { // skip empty TLS
               ++iteratorTLS;
            } while (!iteratorTLS.isLast() && iteratorTLS->empty());

            if (!iteratorTLS.isLast()) {
               iteratorList = (iteratorTLS)->begin();
            } else {
               iteratorList = {};
               return *this;
            }
         }
      }
      return *this;
   }

   /// decrement the iterator
   TLSIteratorThreadLocal& operator--() {
      not_implemented();
   }

   TLSIteratorThreadLocal& operator+=(size_t diff) {
      ListIteratorType lastIter = (iteratorTLS)->end();
      size_t remainingSize = lastIter - iteratorList;
      if (diff < remainingSize) {
         iteratorList += diff;
      } else {
         while (remainingSize <= diff) {
            ++iteratorTLS;
            assert(!isLast());
            diff -= remainingSize;
            remainingSize = (iteratorTLS)->size();
         }
         iteratorList = (iteratorTLS)->begin();
         iteratorList += diff;
      }
      return *this;
   }

   TLSIteratorThreadLocal operator+(const size_t diff) {
      auto other = *this;
      other += diff;
      return other;
   }

   /// compare two iterators for equality
   [[nodiscard]] bool operator==(const TLSIteratorThreadLocal& other) const { return iteratorTLS == other.iteratorTLS && iteratorList == other.iteratorList; }
   /// compare two iterators for inequality
   [[nodiscard]] bool operator!=(const TLSIteratorThreadLocal& other) const { return iteratorTLS != other.iteratorTLS || iteratorList != other.iteratorList; }

   /// Dereference
   [[nodiscard]] value_type& operator*() const {
      assert(!isLast());
      return *iteratorList;
   }
   /// Member access
   [[nodiscard]] value_type* operator->() const { return iteratorList.operator->(); }

   /// sets the iterator to the end
   void setEnd() { iteratorTLS.setEnd(); }
   /// points the iterator to the last element of the list?
   [[nodiscard]] bool isLast() const { return iteratorTLS.isLast(); }

   static TLSIteratorThreadLocal get_begin(std::conditional_t<isConst, const ListType, ListType>& list) {
      auto iterTL = list.begin();
      while (!iterTL.isLast() && iterTL->empty()) ++iterTL;
      if (iterTL.isLast())
         return {iterTL, {}};
      else
         return {iterTL, iterTL->begin()};
   }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------