#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include "tbb/concurrent_vector.h"
#include "tbb/scalable_allocator.h"
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class NonOwningTbbConcurrentVector;
// ---------------------------------------------------------------------------------------------------
template <typename VectorType>
struct TbbVectorIteratorHelper {
   VectorType* my_vector;
   typename VectorType::size_type my_index;
   void* my_item;
};
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class OwningTbbConcurrentVector {
   public:
   friend class NonOwningTbbConcurrentVector<ElementTypeParam>;
   using ElementType = ElementTypeParam;
   using NonOwningVariant = NonOwningTbbConcurrentVector<ElementType>;
#if SCALABLE_ALLOC
   using TbbListType = tbb::concurrent_vector<ElementType, oneapi::tbb::scalable_allocator<ElementType>>;
#else
   using TbbListType = tbb::concurrent_vector<ElementType>;
#endif

   private:
   // the list
   TbbListType list;

   template <bool isConst>
   struct iteratorImpl { // fast but reverse order
      using value_type = std::conditional_t<isConst, const ElementType, ElementType>;
      using iteratorType = std::conditional_t<isConst, typename TbbListType::const_iterator, typename TbbListType::iterator>;

      iteratorType iterator;

      static_assert(sizeof(iteratorType) == sizeof(TbbVectorIteratorHelper<TbbListType>));

      private:
      TbbVectorIteratorHelper<TbbListType>* reinterpretIterator() { return reinterpret_cast<TbbVectorIteratorHelper<TbbListType>*>(&iterator); }
      const TbbVectorIteratorHelper<TbbListType>* reinterpretIterator() const { return reinterpret_cast<const TbbVectorIteratorHelper<TbbListType>*>(&iterator); }

      public:
      // Pre-increment
      iteratorImpl& operator++() {
         ++iterator;
         return *this;
      }
      /// decrement the iterator
      iteratorImpl& operator--() {
         --iterator;
         return *this;
      }

      iteratorImpl& operator+=(const size_t diff) {
         iterator += diff;
         return *this;
      }

      iteratorImpl operator+(const size_t diff) { return {iterator + diff}; }

      /// compare two iterators for equality
      [[nodiscard]] bool operator==(const iteratorImpl& other) const { return iterator == other.iterator; }
      /// compare two iterators for inequality
      [[nodiscard]] bool operator!=(const iteratorImpl& other) const { return iterator != other.iterator; }

      /// Dereference
      [[nodiscard]] value_type& operator*() const { return *iterator; }
      /// Member access
      [[nodiscard]] value_type* operator->() const { return iterator.operator->(); }

      /// sets the iterator to the end
      void setEnd() { reinterpretIterator()->my_vector = nullptr; }
      /// points the iterator to the last element of the list?
      [[nodiscard]] bool isLast() const { return reinterpretIterator()->my_vector == nullptr || iterator == reinterpretIterator()->my_vector->end(); }
   };

   public:
   OwningTbbConcurrentVector() noexcept = default;
   ~OwningTbbConcurrentVector() = default;

   using ConstIterator = iteratorImpl<true>;
   using Iterator = iteratorImpl<false>;

   // using ConstIteratorSlow = iteratorImplSlow<true>;
   // using IteratorSlow = iteratorImplSlow<false>;

   template <typename... Args>
   Iterator emplace_back(Args... args) { return {list.emplace_back(std::forward<Args>(args)...)}; }

   void compactLastChunk() {}
   void reserve(size_t size) { list.reserve(size); }
   void resize(size_t size) { list.resize(size); }
   [[nodiscard]] bool empty() const { return list.empty(); }
   [[nodiscard]] bool containsObject(ElementType&) const { not_implemented(); }
   [[nodiscard]] size_t size() const { return list.size(); }
   [[nodiscard]] const ElementType& back() const { return list.back(); }

   [[nodiscard]] bool operator==(const OwningTbbConcurrentVector& other) const { return list == other.list; }
   [[nodiscard]] const ElementType& operator[](size_t idx) const { return list[idx]; }
   [[nodiscard]] ElementType& operator[](size_t idx) { return list[idx]; }

   [[nodiscard]] Iterator begin() { return {list.begin()}; }
   [[nodiscard]] Iterator end() { return {list.end()}; }
   [[nodiscard]] ConstIterator begin() const { return {list.begin()}; }
   [[nodiscard]] ConstIterator end() const { return {list.end()}; }
};
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class NonOwningTbbConcurrentVector {
   using ElementType = ElementTypeParam;
   using OwningVariant = OwningTbbConcurrentVector<ElementType>;
   using ListType = tbb::concurrent_vector<typename OwningVariant::TbbListType*>;

   ListType list;
   size_t sizeVal;

   template <bool isConst>
   struct iteratorImpl {
      using IteratorElementType = std::conditional_t<isConst, const ElementType, ElementType>;
      using InnerListType = typename OwningVariant::TbbListType;
      using iteratorTypeInner = std::conditional_t<isConst, typename InnerListType::const_iterator, typename InnerListType::iterator>;
      using iteratorTypeOuter = std::conditional_t<isConst, typename ListType::const_iterator, typename ListType::iterator>;

      // iterator over the currently pointed to list
      iteratorTypeInner iteratorInner = {};
      /// iterator over the list "list"
      iteratorTypeOuter iteratorOuter = {};

      static_assert(sizeof(iteratorTypeInner) == sizeof(TbbVectorIteratorHelper<ListType>));
      static_assert(sizeof(iteratorTypeOuter) == sizeof(TbbVectorIteratorHelper<ListType>));

      private:
      TbbVectorIteratorHelper<ListType>* reinterpretIterator() { return reinterpret_cast<TbbVectorIteratorHelper<ListType>*>(&iteratorOuter); }
      const TbbVectorIteratorHelper<ListType>* reinterpretIterator() const { return reinterpret_cast<const TbbVectorIteratorHelper<ListType>*>(&iteratorOuter); }

      public:
      /// the reference type
      using reference = IteratorElementType&;
      /// the pointer type
      using pointer = IteratorElementType*;

      enum EndTag { End };

      iteratorImpl() = default;

      iteratorImpl(iteratorTypeOuter iterType) {
         iteratorOuter = std::move(iterType);
         iteratorInner = (*iteratorOuter)->begin();
      }

      iteratorImpl(iteratorTypeOuter iterType, EndTag)
         : iteratorOuter(std::move(iterType)) {
      }

      iteratorImpl(std::conditional_t<isConst, const ListType, ListType>& list, size_t idx) {
         iteratorOuter = list.begin();
         iteratorOuter += idx;
         iteratorInner = (*iteratorOuter)->end();
      }

      /// increment the iterator
      iteratorImpl& operator++() {
         size_t outerSize = reinterpretIterator()->my_vector->size();
         if (iteratorInner == iteratorTypeInner{})
            return *this;
         assert(reinterpretIterator()->my_index < outerSize);
         if (++iteratorInner == (*iteratorOuter)->end()) {
            ++iteratorOuter;
            if (reinterpretIterator()->my_index != outerSize) {
               iteratorInner = (*iteratorOuter)->begin();
            } else {
               iteratorInner = iteratorTypeInner{};
            }
         }
         return *this;
      }
      /// decrement the iterator
      iteratorImpl& operator--() {
         if (iteratorInner == (*iteratorOuter)->begin()) {
            --iteratorOuter;
            iteratorInner = --(*iteratorOuter)->end();
         } else {
            --iteratorInner;
         }
         return *this;
      }

      iteratorImpl& operator+=(size_t diff) {
         auto* iteratorInnerAccessible = reinterpret_cast<const TbbVectorIteratorHelper<ListType>*>(&iteratorOuter);
         size_t remainingSize = (*iteratorOuter)->size() - iteratorInnerAccessible->my_index;
         if (diff < remainingSize) {
            iteratorInner += diff;
            return *this;
         } else {
            while (remainingSize < diff) {
               ++iteratorOuter;
               diff -= remainingSize;
               remainingSize = (*iteratorOuter)->size();
            }
            iteratorInner = (*iteratorOuter)->begin();
            iteratorInner += diff;
         }

         return *this;
      }

      /// compare two iterators for equality
      bool operator==(const iteratorImpl& other) const { return iteratorInner == other.iteratorInner && iteratorOuter == other.iteratorOuter; }
      /// compare two iterators for inequality
      bool operator!=(const iteratorImpl& other) const { return iteratorInner != other.iteratorInner || iteratorOuter != other.iteratorOuter; }

      /// get reference
      reference operator*() const { return *iteratorInner; }
      /// get pointer
      pointer operator->() const { return iteratorInner.operator->(); }

      /// points the iterator to the last element of the list?
      [[nodiscard]] bool isLast() const { return iteratorOuter == reinterpretIterator()->my_vector->end(); }

      void setEnd() {
         iteratorOuter = reinterpretIterator()->my_vector->end();
         iteratorInner = {};
      }
   };

   public:
   void merge(OwningVariant& otherList) {
      if (otherList.empty()) return;
      assert(!otherList.empty());
      std::atomic_ref(sizeVal) += otherList.size();
      list.emplace_back(&otherList.list);
   }
   void merge(NonOwningTbbConcurrentVector& otherList) {
      // unfortunately we need a hard copy here
      std::atomic_ref(sizeVal) += std::atomic_ref(otherList.sizeVal).load();
      list.reserve(list.size() + otherList.size());
      for (auto* elem : otherList.list)
         list.emplace_back(elem);
   }

   [[nodiscard]] size_t size() const { return sizeVal; } // todo
   [[nodiscard]] bool empty() const { return list.empty(); }
   [[nodiscard]] const ElementType& back() const { return list.back()->back(); }

   bool operator==(const OwningVariant& other) const { return list == other.list; }
   bool operator==(const NonOwningTbbConcurrentVector& other) const { return list == other.list; }

   using Iterator = iteratorImpl<false>;
   using ConstIterator = iteratorImpl<true>;

   [[nodiscard]] Iterator begin() { return list.empty() ? end() : Iterator{list.begin()}; }
   [[nodiscard]] Iterator end() { return Iterator(list.end(), Iterator::End); }
   [[nodiscard]] ConstIterator begin() const { return list.empty() ? end() : ConstIterator{list.begin()}; }
   [[nodiscard]] ConstIterator end() const { return ConstIterator(list.end(), ConstIterator::End); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------
