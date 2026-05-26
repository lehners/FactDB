#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include <atomic>
#include <cstdint>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class NonOwningLinkedList;
// ---------------------------------------------------------------------------------------------------
template <typename T>
T* assertNotNullPtr(T* ptr) {
   assert(ptr != nullptr);
   return ptr;
}
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class OwningLinkedList {
   public:
   template <typename T>
   friend class NonOwningLinkedList;
   using ElementType = ElementTypeParam;
   using NonOwningVariant = NonOwningLinkedList<ElementType>;

   struct Node {
      ElementType value;
      Node* next = nullptr;
   };

   private:
   /// the first entry
   Node* firstEntry = nullptr;
   size_t sizeVal = 0;

   template <bool isConst>
   struct reverseIteratorImpl { // fast but reverse order
      using value_type = std::conditional_t<isConst, const ElementType, ElementType>;

      Node* curElement = nullptr;

      public:
      // Pre-increment
      reverseIteratorImpl& operator++();
      reverseIteratorImpl& operator+=(size_t diff);
      /// decrement the iterator
      [[noreturn]] [[deprecated]] reverseIteratorImpl& operator--() { not_implemented(); }

      /// compare two iterators for equality
      [[nodiscard]] bool operator==(const reverseIteratorImpl& other) const { return curElement == other.curElement; }
      /// compare two iterators for inequality
      [[nodiscard]] bool operator!=(const reverseIteratorImpl& other) const { return curElement != other.curElement; }

      /// Dereference
      [[nodiscard]] value_type& operator*() const { return assertNotNullPtr(curElement)->value; }
      /// Member access
      [[nodiscard]] value_type* operator->() const { return assertNotNullPtr(curElement)->value; }

      /// sets the iterator to the end
      void setEnd() { curElement = nullptr; }
      /// points the iterator to the last element of the list?
      [[nodiscard]] bool isLast() const { return curElement == nullptr; }
   };

   template <bool isConst>
   struct iteratorImplSlow { // slow but in insert order
      using value_type = std::conditional_t<isConst, const ElementType, ElementType>;

      OwningLinkedList* list = nullptr;
      Node* curElement = nullptr;

      public:
      iteratorImplSlow() = default;
      iteratorImplSlow(OwningLinkedList* list_) : list(list_), curElement(list != nullptr ? list->firstEntry : nullptr) {
         while (curElement != nullptr && curElement->next != nullptr)
            curElement = curElement->next;
      }
      iteratorImplSlow(OwningLinkedList* list_, Node* nodePtr) : list(list_), curElement(nodePtr) {}

      // Pre-increment
      iteratorImplSlow& operator++();
      /// decrement the iterator
      iteratorImplSlow& operator--();

      /// compare two iterators for equality
      [[nodiscard]] bool operator==(const iteratorImplSlow& other) const { return curElement == other.curElement; }
      /// compare two iterators for inequality
      [[nodiscard]] bool operator!=(const iteratorImplSlow& other) const { return curElement != other.curElement; }

      /// Dereference
      [[nodiscard]] value_type& operator*() const { return assertNotNullPtr(curElement)->value; }
      /// Member access
      [[nodiscard]] value_type* operator->() const { return assertNotNullPtr(curElement)->value; }

      /// sets the iterator to the end
      void setEnd() { curElement = nullptr; }
      /// points the iterator to the last element of the list?
      [[nodiscard]] bool isLast() const { return curElement == nullptr; }
   };

   public:
   OwningLinkedList() noexcept = default;
   ~OwningLinkedList();

   using ConstIterator = reverseIteratorImpl<true>;
   using Iterator = reverseIteratorImpl<false>;

   using ConstIteratorSlow = iteratorImplSlow<true>;
   using IteratorSlow = iteratorImplSlow<false>;

   template <typename... Args>
   Iterator emplace_back(Args... args);

   void compactLastChunk() {}
   void reserve(size_t) {}
   [[deprecated]] void resize(size_t) {}
   [[nodiscard]] bool empty() const { return firstEntry == nullptr; }
   [[nodiscard]] bool containsObject(ElementType& element) const;
   [[nodiscard]] size_t size() const { return sizeVal; }
   [[nodiscard]] ElementType& back() const;

   [[nodiscard]] bool operator==(OwningLinkedList& other) const { return firstEntry == other.firstEntry; }
   [[nodiscard]] [[deprecated]] ElementType& operator[](size_t idx) const;

   [[nodiscard]] ConstIterator begin() const { return {firstEntry}; }
   [[nodiscard]] ConstIterator end() const { return {nullptr}; }
   [[nodiscard]] Iterator begin() { return {firstEntry}; }
   [[nodiscard]] Iterator end() { return {nullptr}; }

   [[nodiscard]] ConstIteratorSlow slowBegin() const { return {this}; }
   [[nodiscard]] ConstIteratorSlow slowEnd() const { return {this, nullptr}; }
   [[nodiscard]] IteratorSlow slowBegin() { return {this}; }
   [[nodiscard]] IteratorSlow slowEnd() { return {this, nullptr}; }
};
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class NonOwningLinkedList {
   using ElementType = ElementTypeParam;
   using OwningVariant = OwningLinkedList<ElementType>;
   using ListType = OwningLinkedList<typename OwningVariant::Node*>;

   std::size_t sizeVal = 0;
   ListType list;

   template <bool isConst>
   struct reverseIteratorImpl {
      private:
      using IteratorElementType = std::conditional_t<isConst, const ElementType, ElementType>;
      using iteratorTypeOuter = std::conditional_t<isConst, typename ListType::ConstIterator, typename ListType::Iterator>;
      using iteratorTypeInner = std::conditional_t<isConst, typename OwningVariant::ConstIterator, typename OwningVariant::Iterator>;

      iteratorTypeOuter curList = {};
      iteratorTypeInner curListElem = {};

      public:
      /// the reference type
      using reference = IteratorElementType&;
      /// the pointer type
      using pointer = IteratorElementType*;

      /// constructor
      reverseIteratorImpl() = default;
      /// constructor
      explicit reverseIteratorImpl(ListType::Node* cur) : curList(cur), curListElem{cur ? cur->value : nullptr} {}
      /// increment the iterator
      reverseIteratorImpl& operator++();
      /// decrement the iterator
      reverseIteratorImpl& operator--() { not_implemented(); }

      /// compare two iterators for equality
      bool operator==(const reverseIteratorImpl& other) const { return curList == other.curList && curListElem == other.curListElem; }
      /// compare two iterators for inequality
      bool operator!=(const reverseIteratorImpl& other) const { return curList != other.curList || curListElem != other.curListElem; }

      /// get reference
      reference operator*() const;
      /// get pointer
      pointer operator->() const;

      /// points the iterator to the last element of the list?
      [[nodiscard]] bool isLast() const { return curList.isLast() && curListElem.isLast(); }
      void setEnd() { curList = nullptr, curListElem = nullptr; }
   };

   template <bool isConst>
   struct iteratorImplSlow {
      private:
      using IteratorElementType = std::conditional_t<isConst, const ElementType, ElementType>;
      using iteratorTypeOuter = std::conditional_t<isConst, typename ListType::ConstIteratorSlow, typename ListType::IteratorSlow>;
      using iteratorTypeInner = std::conditional_t<isConst, typename OwningVariant::ConstIteratorSlow, typename OwningVariant::IteratorSlow>;

      NonOwningLinkedList* list = nullptr;
      iteratorTypeOuter curList = {};
      iteratorTypeInner curListElem = {};

      public:
      /// the reference type
      using reference = IteratorElementType&;
      /// the pointer type
      using pointer = IteratorElementType*;

      /// constructor
      iteratorImplSlow() = default;
      /// constructor
      iteratorImplSlow(NonOwningLinkedList* list_) : list(list_), curList(list), curListElem(*curList) {}
      iteratorImplSlow(NonOwningLinkedList* list_, ListType::Node* cur) : list(list_), curList(cur), curListElem{cur ? cur->value : nullptr} {}
      /// increment the iterator
      iteratorImplSlow& operator++();
      /// decrement the iterator
      iteratorImplSlow& operator--();

      /// compare two iterators for equality
      bool operator==(const iteratorImplSlow& other) const { return curList == other.curList && curListElem == other.curListElem; }
      /// compare two iterators for inequality
      bool operator!=(const iteratorImplSlow& other) const { return curList != other.curList || curListElem != other.curListElem; }

      /// get reference
      reference operator*() const;
      /// get pointer
      pointer operator->() const;

      /// points the iterator to the last element of the list?
      [[nodiscard]] bool isLast() const { return curList.isLast() && curListElem.isLast(); }
      void setEnd() { curList = nullptr, curListElem = nullptr; }
   };

   public:
   void addElementReference(ElementType& element);

   void merge(OwningVariant& otherList);
   void merge(NonOwningLinkedList& otherList);

   [[nodiscard]] size_t size() const { return sizeVal; }
   [[nodiscard]] bool empty() const { return list.empty(); }
   [[nodiscard]] ElementType& back() const { return list.back()->value; }

   bool operator==(const OwningVariant& other) const { return sizeVal == 1 && static_cast<const ListType&>(list).firstEntry->value == other.firstEntry; }

   using Iterator = reverseIteratorImpl<false>;
   using ConstIterator = reverseIteratorImpl<true>;

   using IteratorSlow = iteratorImplSlow<false>;
   using ConstIteratorSlow = iteratorImplSlow<true>;

   [[nodiscard]] Iterator begin() { return Iterator(sizeVal != 0 ? static_cast<const ListType&>(list).firstEntry : nullptr); }
   [[nodiscard]] Iterator end() { return Iterator(nullptr); }
   [[nodiscard]] ConstIterator begin() const { return ConstIterator(sizeVal != 0 ? static_cast<const ListType&>(list).firstEntry : nullptr); }
   [[nodiscard]] ConstIterator end() const { return ConstIterator(nullptr); }

   [[nodiscard]] IteratorSlow slowBegin() { return Iterator(sizeVal != 0 ? static_cast<const ListType&>(list).firstEntry : nullptr); }
   [[nodiscard]] IteratorSlow slowEnd() { return Iterator(this, nullptr); }
   [[nodiscard]] ConstIteratorSlow slowBegin() const { return ConstIterator(sizeVal != 0 ? static_cast<const ListType&>(list).firstEntry : nullptr); }
   [[nodiscard]] ConstIteratorSlow slowEnd() const { return ConstIterator(this, nullptr); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------
// add implementations:
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/list/LinkedListNonOwningImpl.tpp"
#include "factDB/infra/list/LinkedListOwningImpl.tpp"
// ---------------------------------------------------------------------------------------------------