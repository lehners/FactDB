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
template <typename VectorType>
struct TLSIteratorHelper {
   VectorType* my_vector;
   typename VectorType::size_type my_index;
   void* my_value;
};
// ---------------------------------------------------------------------------------------------------
template <typename T>
concept IsTBBThreadSpecific = requires {
   typename T::value_type;
   requires std::is_same_v<T, tbb::enumerable_thread_specific<typename T::value_type>>;
};
// ---------------------------------------------------------------------------------------------------
template <typename ListType, typename ElementType, bool isConst>
   requires IsTBBThreadSpecific<ListType>
struct TLSIterator {
   using value_type = std::conditional_t<isConst, const ElementType, ElementType>;
   using IteratorTLSType = std::conditional_t<isConst, typename ListType::const_iterator, typename ListType::iterator>;
   using ListIteratorType = std::conditional_t<isConst, typename ListType::value_type::const_iterator, typename ListType::value_type::iterator>;
   using TLSHelperType = TLSIteratorHelper<::tbb::concurrent_vector<typename ListType::value_type>>;

   static_assert(sizeof(IteratorTLSType) == sizeof(TLSIteratorHelper<ListType>));

   IteratorTLSType iteratorTLS;
   ListIteratorType iteratorList;

   private:
   TLSHelperType* reinterpretIterator() { return reinterpret_cast<TLSHelperType*>(&iteratorTLS); }
   const TLSHelperType* reinterpretIterator() const { return reinterpret_cast<const TLSHelperType*>(&iteratorTLS); }

   public:
   // Pre-increment
   TLSIterator& operator++() {
      if (reinterpretIterator()->my_index != reinterpretIterator()->my_vector->size()) {
         if (++iteratorList == (iteratorTLS)->end()) {
            do { // skip empty TLS
               ++iteratorTLS;
            } while (reinterpretIterator()->my_index != reinterpretIterator()->my_vector->size() && iteratorTLS->empty());

            if (reinterpretIterator()->my_index != reinterpretIterator()->my_vector->size()) {
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
   TLSIterator& operator--() {
      not_implemented();
   }

   TLSIterator& operator+=(size_t diff) {
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

   TLSIterator operator+(const size_t diff) {
      auto other = *this;
      other += diff;
      return other;
   }

   /// compare two iterators for equality
   [[nodiscard]] bool operator==(const TLSIterator& other) const { return iteratorTLS == other.iteratorTLS && iteratorList == other.iteratorList; }
   /// compare two iterators for inequality
   [[nodiscard]] bool operator!=(const TLSIterator& other) const { return iteratorTLS != other.iteratorTLS || iteratorList != other.iteratorList; }

   /// Dereference
   [[nodiscard]] value_type& operator*() const {
      assert(!isLast());
      return *iteratorList;
   }
   /// Member access
   [[nodiscard]] value_type* operator->() const { return iteratorList.operator->(); }

   /// sets the iterator to the end
   void setEnd() { reinterpretIterator()->my_vector = nullptr; }
   /// points the iterator to the last element of the list?
   [[nodiscard]] bool isLast() const { return reinterpretIterator()->my_vector == nullptr || reinterpretIterator()->my_index == reinterpretIterator()->my_vector->size(); }

   static TLSIterator get_begin(std::conditional_t<isConst, const ListType, ListType>& list) {
      auto iterTLS = list.begin();
      while (iterTLS != list.end() && iterTLS->empty()) ++iterTLS;
      if (iterTLS == list.end())
         return {iterTLS, {}};
      else
         return {iterTLS, iterTLS->begin()};
   }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------