#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include "tbb/scalable_allocator.h"
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class NonOwningStlVector;
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class OwningStlVector {
   public:
   friend class NonOwningStlVector<ElementTypeParam>;
   using ElementType = ElementTypeParam;
   using NonOwningVariant = NonOwningStlVector<ElementType>;
#if SCALABLE_ALLOC
   using ListType = std::vector<ElementType, ::tbb::scalable_allocator<ElementType>>;
#else
   using ListType = std::vector<ElementType>;
#endif

   protected:
   // the list
   ListType list;

   template <bool isConst>
   struct iteratorImpl {
      using value_type = std::conditional_t<isConst, const ElementType, ElementType>;
      using iteratorType = std::conditional_t<isConst, typename ListType::const_iterator, typename ListType::iterator>;

      iteratorType iterator;
      iteratorType iteratorEnd;

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
      void setEnd() { iterator = {}; }
      /// points the iterator to the last element of the list?
      [[nodiscard]] bool isLast() const { return iterator == iteratorType{} || iterator == iteratorEnd; }
   };

   public:
   OwningStlVector() noexcept = default;
   ~OwningStlVector() = default;

   OwningStlVector(OwningStlVector&& other) : list(std::move(other.list)) {}
   OwningStlVector& operator=(OwningStlVector&& other) {
      this->list = other.list;
      return *this;
   }

   using ConstIterator = iteratorImpl<true>;
   using Iterator = iteratorImpl<false>;

   // using ConstIteratorSlow = iteratorImplSlow<true>;
   // using IteratorSlow = iteratorImplSlow<false>;

   template <typename... Args>
   Iterator emplace_back(Args... args) {
      list.emplace_back(std::forward<Args>(args)...);
      return {list.end() - 1, list.end()};
   }

   void compactLastChunk() {}
   void reserve(size_t targetSize) { list.reserve(targetSize); }
   void* data() { return list.data(); }
   void resize(size_t size) { list.resize(size); }
   [[nodiscard]] bool empty() const { return list.empty(); }
   [[nodiscard]] bool containsObject(ElementType&) const { not_implemented(); }
   [[nodiscard]] size_t size() const { return list.size(); }
   [[nodiscard]] size_t capacity() const { return list.capacity(); }
   [[nodiscard]] const ElementType& back() const { return list.back(); }

   [[nodiscard]] bool operator==(const OwningStlVector& other) const { return list == other.list; }
   [[nodiscard]] const ElementType& operator[](size_t idx) const { return list[idx]; }
   [[nodiscard]] ElementType& operator[](size_t idx) { return list[idx]; }

   [[nodiscard]] Iterator begin() { return {list.begin(), list.end()}; }
   [[nodiscard]] Iterator end() { return {list.end(), list.end()}; }
   [[nodiscard]] ConstIterator begin() const { return {list.begin(), list.end()}; }
   [[nodiscard]] ConstIterator end() const { return {list.end(), list.end()}; }
};
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class OwningStlVectorLockable : public OwningStlVector<ElementTypeParam> {
   public:
   std::mutex lock = {};

   OwningStlVectorLockable() = default;

   OwningStlVectorLockable(const OwningStlVectorLockable& other) = delete;
   OwningStlVectorLockable(OwningStlVectorLockable&& other)
      : OwningStlVector<ElementTypeParam>(std::move(other)) {}

   OwningStlVectorLockable& operator=(const OwningStlVectorLockable& other) = delete;
   OwningStlVectorLockable& operator=(OwningStlVectorLockable&& other) {
      this->list = other.list;
      return *this;
   }
};
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class NonOwningStlVector {
   using ElementType = ElementTypeParam;
   using OwningVariant = OwningStlVector<ElementType>;

   using InnerListTypeRaw = OwningVariant::ListType;
   using InnerListType = InnerListTypeRaw*;
#if SCALABLE_ALLOC
   using ListType = std::vector<InnerListType, ::tbb::scalable_allocator<InnerListType>>;
#else
   using ListType = std::vector<InnerListType>;
#endif

   ListType list;
   size_t sizeVal;

   template <bool isConst>
   struct iteratorImpl {
      using IteratorElementType = std::conditional_t<isConst, const ElementType, ElementType>;
      using iteratorTypeInner = std::conditional_t<isConst, typename InnerListTypeRaw::const_iterator, typename InnerListTypeRaw::iterator>;
      using iteratorTypeOuter = std::conditional_t<isConst, typename ListType::const_iterator, typename ListType::iterator>;

      // iterator over the currently pointed to list
      iteratorTypeInner iteratorInner = {};
      /// iterator over the list "list"
      iteratorTypeOuter iteratorOuter = {};
      iteratorTypeOuter iteratorOuterEnd = {};

      /// the reference type
      using reference = IteratorElementType&;
      /// the pointer type
      using pointer = IteratorElementType*;

      enum EndTag { End };

      iteratorImpl() = default;

      iteratorImpl(iteratorTypeOuter iterType, iteratorTypeOuter outerEnd)
         : iteratorOuter(std::move(iterType)), iteratorOuterEnd(std::move(outerEnd)) {
         iteratorInner = (*iteratorOuter)->begin();
      }

      iteratorImpl(iteratorTypeOuter iterType, EndTag)
         : iteratorOuter(iterType), iteratorOuterEnd(iterType) {
      }

      iteratorImpl(std::conditional_t<isConst, const ListType, ListType>& list, size_t idx)
         : iteratorOuter(list.begin()), iteratorOuterEnd(list.end()) {
         iteratorOuter += idx;
         iteratorInner = (*iteratorOuter)->end();
      }

      /// increment the iterator
      iteratorImpl& operator++() {
         if (iteratorInner == iteratorTypeInner{})
            return *this;
         assert(iteratorOuter < iteratorOuterEnd);
         if (++iteratorInner == (*iteratorOuter)->end()) {
            ++iteratorOuter;
            if (iteratorOuter != iteratorOuterEnd) {
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
         size_t remainingSize = (*iteratorOuter)->end() - iteratorInner;
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
      [[nodiscard]] bool isLast() const { return iteratorOuter == iteratorOuterEnd; }

      void setEnd() {
         iteratorOuter = iteratorOuterEnd;
         iteratorInner = {};
      }
   };

   public:
   void merge(OwningVariant& otherList) {
      auto otherSize = otherList.size();
      if (otherSize == 0) return;
      assert(otherSize != 0);
      sizeVal += otherSize;
      list.emplace_back(&otherList.list);
   }
   void merge(NonOwningStlVector& otherList) {
      // unfortunately we need a hard copy here
      sizeVal += otherList.sizeVal;
      list.reserve(otherList.size());
      list.insert(list.end(), otherList.list.begin(), otherList.list.end());
   }

   [[nodiscard]] size_t size() const { return sizeVal; } // todo
   [[nodiscard]] bool empty() const { return list.empty(); }
   [[nodiscard]] const ElementType& back() const { return list.back()->back(); }

   bool operator==(const OwningVariant& other) const { return list == other.list; }
   bool operator==(const NonOwningStlVector& other) const { return list == other.list; }

   using Iterator = iteratorImpl<false>;
   using ConstIterator = iteratorImpl<true>;

   [[nodiscard]] Iterator begin() { return list.empty() ? end() : Iterator{list.begin(), list.end()}; }
   [[nodiscard]] Iterator end() { return Iterator(list.end(), Iterator::End); }
   [[nodiscard]] ConstIterator begin() const { return list.empty() ? end() : ConstIterator{list.begin(), list.end()}; }
   [[nodiscard]] ConstIterator end() const { return ConstIterator(list.end(), ConstIterator::End); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------
