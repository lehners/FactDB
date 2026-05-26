#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include "factDB/infra/list/tbb/ConcurrentVector.hpp"
#include "factDB/infra/list/tbb/InlineWrapper.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class NonOwningInlineConcurrentVector;
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class OwningInlineConcurrentVector {
   public:
   friend class NonOwningInlineConcurrentVector<ElementTypeParam>;
   using ElementType = ElementTypeParam;
   using NonOwningVariant = NonOwningInlineConcurrentVector<ElementType>;
   using WrappedListType = InlineWrapper<ElementType, factDB::infra::list::tbb::concurrent_vector<ElementType>>;

   private:
   // the list
   WrappedListType list;

   public:
   OwningInlineConcurrentVector() noexcept = default;
   ~OwningInlineConcurrentVector() = default;

   using ConstIterator = WrappedListType::const_iterator;
   using Iterator = WrappedListType::iterator;

   template <typename... Args>
   Iterator emplace_back(Args... args) { return list.emplace_back(std::forward<Args>(args)...); }

   void compactLastChunk() {}
   void reserve(size_t size) { list.reserve(size); }
   void resize(size_t size) { list.resize(size); }
   [[nodiscard]] bool empty() const { return list.empty(); }
   [[nodiscard]] bool containsObject(ElementType&) const { not_implemented(); }
   [[nodiscard]] size_t size() const { return list.size(); }
   [[nodiscard]] const ElementType& back() const { return list.back(); }

   [[nodiscard]] bool operator==(OwningInlineConcurrentVector& other) const { return list == other.list; }
   [[nodiscard]] const ElementType& operator[](size_t idx) const { return list[idx]; }
   [[nodiscard]] ElementType& operator[](size_t idx) { return list[idx]; }

   [[nodiscard]] Iterator begin() { return list.begin(); }
   [[nodiscard]] Iterator end() { return list.end(); }
   [[nodiscard]] ConstIterator begin() const { return list.begin(); }
   [[nodiscard]] ConstIterator end() const { return list.end(); }
};
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class NonOwningInlineConcurrentVector {
   using ElementType = ElementTypeParam;
   using OwningVariant = OwningInlineConcurrentVector<ElementType>;
   using OwningWrappedListType = typename OwningVariant::WrappedListType;
   using ListType = InlineWrapper<OwningWrappedListType*, factDB::infra::list::tbb::concurrent_vector<OwningWrappedListType*>>;

   ListType list;
   std::atomic<size_t> sizeVal;

   template <bool isConst>
   struct iteratorImpl {
      using IteratorElementType = std::conditional_t<isConst, const ElementType, ElementType>;
      using iteratorTypeInner = std::conditional_t<isConst, typename OwningWrappedListType::iterator, typename OwningWrappedListType::iterator>;
      using iteratorTypeOuter = std::conditional_t<isConst, typename ListType::const_iterator, typename ListType::iterator>;

      // iterator over the currently pointed to list
      iteratorTypeInner iteratorInner = {};
      /// iterator over the list "list"
      iteratorTypeOuter iteratorOuter = {};

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
         if (iteratorInner == iteratorTypeInner{})
            return *this;
         assert(!iteratorOuter.isLast());
         if ((++iteratorInner).isLast()) {
            ++iteratorOuter;
            if (!iteratorOuter.isLast()) {
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
         // auto* iteratorInnerAccessible = reinterpret_cast<const TbbVectorIteratorHelper<ListType>*>(&iteratorOuter);
         size_t remainingSize = (*iteratorOuter)->size() - iteratorInner.index();
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
      [[nodiscard]] bool isLast() const { return iteratorOuter.isLast(); }

      void setEnd() {
         iteratorOuter.setEnd();
         iteratorInner = (*iteratorOuter)->end();
      }
   };

   public:
   NonOwningInlineConcurrentVector() = default;
   NonOwningInlineConcurrentVector(const NonOwningInlineConcurrentVector& other) : list(other.list), sizeVal(other.sizeVal.load()) {}

   NonOwningInlineConcurrentVector& operator=(const NonOwningInlineConcurrentVector& other) {
      list = other.list;
      sizeVal = other.sizeVal.load();
      return *this;
   }

   void merge(OwningVariant& otherList) {
      assert(!otherList.empty());
      sizeVal += otherList.size();
      list.emplace_back(&otherList.list);
   }
   void merge(NonOwningInlineConcurrentVector& otherList) {
      // unfortunately we need a hard copy here
      sizeVal += otherList.sizeVal;
      list.reserve(list.size() + otherList.size());
      for (auto& elem : otherList.list)
         list.emplace_back(elem);
   }

   [[nodiscard]] size_t size() const { return sizeVal.load(); }
   [[nodiscard]] bool empty() const { return list.empty(); }
   [[nodiscard]] const ElementType& back() const { return list.back()->back(); }

   bool operator==(const OwningVariant& other) const { return list == other.list; }

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