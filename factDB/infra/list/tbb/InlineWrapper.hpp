#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include <algorithm>
#include <atomic>
#include <cstddef>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <typename ElementType, size_t elementsCount>
struct InlineArray {
   std::atomic<size_t> size_ = 0;
   ElementType elements[elementsCount] = {};

   InlineArray() = default;
   InlineArray(const InlineArray& other) : size_(other.size_.load()) {
      assert(&other != this);
      std::memcpy(elements, other.elements, elementsCount * sizeof(ElementType));
   }
   InlineArray& operator=(InlineArray const& other) {
      size_ = other.size_.load();
      for (size_t i = 0; i != elementsCount; ++i)
         elements[i] = other.elements[i];
      return *this;
   }

   size_t size() const { return size_.load(); }
};
// ---------------------------------------------------------------------------------------------------
template <typename ElementType, typename WrappedList>
class InlineWrapper {
   static constexpr size_t inlineElements = std::max(0ul, (sizeof(WrappedList) - sizeof(size_t)) / sizeof(ElementType));
   // static constexpr size_t inlineElements = (sizeof(WrappedList) - sizeof(size_t)) / sizeof(ElementType);
   // static_assert(sizeof(InlineArray<ElementType, inlineElements>) <= sizeof(WrappedList));

   std::atomic<size_t> locked = 0;
   union {
      InlineArray<ElementType, inlineElements> inlineArray;
      WrappedList list;
   };

   bool isInlined() const { return inlineArray.size() <= inlineElements; }

   public:
   InlineWrapper() : inlineArray({}) {
   }

   InlineWrapper(InlineWrapper const& other) {
      assert(!locked.load());
      if (isInlined()) {
         inlineArray = other.inlineArray;
      } else {
         list = other.list;
      }
   }

   InlineWrapper& operator=(InlineWrapper const& other) {
      assert(!locked.load());
      if (isInlined()) {
         inlineArray = other.inlineArray;
      } else {
         list = other.list;
      }
      return *this;
   }

   InlineWrapper(InlineWrapper&& other) = default;
   InlineWrapper& operator=(InlineWrapper&& other) = default;

   ~InlineWrapper() {
      if (!isInlined()) {
         list.~WrappedList();
      }
   }

   enum IteratorMode {
      ElementIterator,
      ListIterator
   };

   template <bool isConst>
   class iteratorImpl {
      public:
      using ElementTypeIterator = std::conditional_t<isConst, const ElementType, ElementType>;
      using WrappedListIteratorType = std::conditional_t<isConst, typename WrappedList::const_iterator, typename WrappedList::iterator>;

      using reference = std::conditional_t<isConst, const ElementTypeIterator, ElementTypeIterator>&;
      using pointer = ElementTypeIterator*;

      private:
      IteratorMode mode = IteratorMode::ElementIterator;
      union {
         struct {
            ElementTypeIterator* elementIterator = nullptr;
            size_t elementsSeen = 0;
         };
         WrappedListIteratorType inlineIterator;
      };

      public:
      iteratorImpl() : mode(IteratorMode::ElementIterator), elementIterator(nullptr), elementsSeen(0) {}
      explicit iteratorImpl(ElementTypeIterator* elementPtr, size_t seen) : mode(ElementIterator), elementIterator(elementPtr), elementsSeen(seen) {}
      explicit iteratorImpl(WrappedListIteratorType elementPtr) : mode(ListIterator), inlineIterator(std::move(elementPtr)) {}

      iteratorImpl(const iteratorImpl& other) : mode(other.mode) {
         if (other.mode == ElementIterator) {
            elementIterator = other.elementIterator;
            elementsSeen = other.elementsSeen;
         } else {
            inlineIterator = other.inlineIterator;
         }
      }

      iteratorImpl& operator=(const iteratorImpl& other) {
         mode = other.mode;
         if (other.mode == ElementIterator) {
            elementIterator = other.elementIterator;
            elementsSeen = other.elementsSeen;
         } else {
            inlineIterator = other.inlineIterator;
         }
         return *this;
      }

      reference operator*() const {
         if (mode == ElementIterator) {
            return *elementIterator;
         } else {
            return *inlineIterator;
         }
      }

      pointer operator->() const {
         if (mode == ElementIterator) {
            return elementIterator;
         } else {
            return inlineIterator.operator->();
         }
      }

      iteratorImpl& operator++() {
         if (mode == ElementIterator) {
            ++elementIterator;
            ++elementsSeen;
         } else {
            ++inlineIterator;
         }
         return *this;
      }

      iteratorImpl& operator--() {
         if (mode == ElementIterator) {
            --elementIterator;
            --elementsSeen;
         } else {
            --inlineIterator;
         }
         return *this;
      }

      bool operator==(const iteratorImpl& other) const {
         if (mode != other.mode)
            return false;
         else if (mode == ElementIterator)
            return elementIterator == other.elementIterator;
         else
            return inlineIterator == other.inlineIterator;
      }

      bool operator!=(const iteratorImpl& other) const {
         if (mode != other.mode)
            return true;
         else if (mode == ElementIterator)
            return elementIterator != other.elementIterator;
         else
            return inlineIterator != other.inlineIterator;
      }

      iteratorImpl operator+(const size_t diff) {
         if (mode == ElementIterator) {
            return iteratorImpl(elementIterator + diff, elementsSeen + diff);
         } else {
            return iteratorImpl(inlineIterator + diff);
         }
      }

      iteratorImpl& operator+=(const size_t diff) {
         if (mode == ElementIterator) {
            elementsSeen += diff;
            elementIterator += diff;
         } else {
            inlineIterator += diff;
         }
         return *this;
      }

      void setEnd() {
         if (mode == ElementIterator) {
            elementIterator += inlineElements - elementsSeen;
            elementsSeen = inlineElements;
         } else {
            inlineIterator.setEnd();
         }
      }

      [[nodiscard]] bool isLast() const {
         if (mode == ElementIterator) {
            return elementsSeen == inlineElements;
         } else {
            return inlineIterator.isLast();
         }
      }

      [[nodiscard]] size_t index() const { return mode == ElementIterator ? elementsSeen : inlineIterator.index(); }
   };

   using const_iterator = iteratorImpl<true>;
   using iterator = iteratorImpl<false>;

   // todo fix iterator: if the layout of the inline Wrapper changes, the first iterators will be invalidated.
   // However, these iterators are already stored somewhere, hence, random writes in memory may happen
   // do we need an updatedable iterator? check in each iterator access, if its actually the quick access and if it has changed, use the other acecss variant?

   template <typename... Args>
   iterator emplace_back(Args... args) {
      auto oldSize = inlineArray.size_++;
      auto lockedAtBegin = locked.load();
      if (oldSize < inlineElements && lockedAtBegin <= inlineElements) {
         // SHARED LOCK, MULTIPLE THREADS MAY ENTER THIS REGION
         new (&inlineArray.elements[oldSize]) ElementType(args...);
         [[maybe_unused]] auto comitted = ++locked;
         assert(comitted <= inlineElements);
         return iterator(&inlineArray.elements[oldSize], oldSize);
      } else if (oldSize == inlineElements && lockedAtBegin <= inlineElements) {
         // ONE THREAD ONLY, OTHER BRANCHES MUST NOT BE EDITED, MUST WAIT UNTIL ALL PREVIOUS INLINE PLACES ARE WRITTEN
         oneapi::tbb::detail::d0::spin_wait_until_eq(locked, inlineElements);
         // create parallel vector
         WrappedList helper;
         helper.resize(inlineElements + 1);
         for (size_t i = 0; i < inlineElements; ++i)
            helper[i] = std::move(inlineArray.elements[i]);
         new (&helper[inlineElements]) ElementType(args...);

         //  we need a special init function which does not touch the counter since this may be incremented atomically in the meantime
         list.initFrom(std::move(helper));
         [[maybe_unused]] auto comitted = ++locked;
         assert(comitted == inlineElements + 1);
         return iterator(list.begin() + inlineElements);
      } else {
         oneapi::tbb::detail::d0::spin_wait_until_eq(locked, inlineElements + 1);
         return iterator(list.emplace_back_noSizeAdjust(oldSize, std::forward<Args>(args)...));
      }
      unreachable();
   }

   size_t size() const { return inlineArray.size(); }
   bool empty() const { return size() == 0; }

   void reserve(size_t targetSize) {
      assert(empty());
      if (targetSize <= inlineElements) {
         // everything is already allocated
      } else {
         new (&list) WrappedList();
         list.reserve(targetSize);
         locked = inlineElements + 1;
      }
   }

   void resize(size_t targetSize) {
      assert(empty());
      if (targetSize <= inlineElements) {
         inlineArray.size_ = targetSize;
         locked = targetSize;
      } else {
         new (&list) WrappedList();
         list.resize(targetSize);
         locked = inlineElements + 1;
      }
   }

   [[nodiscard]] ElementType& operator[](size_t idx) {
      if (isInlined()) {
         assert(idx < inlineElements);
         return inlineArray.elements[idx];
      } else {
         return list[idx];
      }
   }

   const_iterator begin() const {
      if (isInlined()) {
         return const_iterator(&inlineArray.elements[0], inlineElements - inlineArray.size());
      } else {
         return const_iterator(list.begin());
      }
   }

   const_iterator end() const {
      if (isInlined()) {
         return const_iterator(&inlineArray.elements[0] + inlineArray.size(), inlineElements);
      } else {
         return const_iterator(list.end());
      }
   }

   iterator begin() {
      if (isInlined()) {
         return iterator(&inlineArray.elements[0], inlineElements - inlineArray.size());
      } else {
         return iterator(list.begin());
      }
   }

   iterator end() {
      if (isInlined()) {
         return iterator(&inlineArray.elements[0] + inlineArray.size(), inlineElements);
      } else {
         return iterator(list.end());
      }
   }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------