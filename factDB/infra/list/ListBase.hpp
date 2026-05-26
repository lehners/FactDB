#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/list/ListTypeEnum.hpp"
#include "factDB/infra/list/ListTypeSelector.hpp"
#include <utility>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam, ListTypeEnum listType>
class NonOwningListBase;
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam, ListTypeEnum listType>
class OwningListBase {
   friend class NonOwningListBase<ElementTypeParam, listType>;

   public:
   using ElementType = ElementTypeParam;
   using ConcreteList = list::ListTypeSelector<listType, ElementType>::OwningType;
   using NonOwningVariant = NonOwningListBase<ElementTypeParam, listType>;

   using Iterator = ConcreteList::Iterator;
   using ConstIterator = typename ConcreteList::ConstIterator;

   private:
   ConcreteList list;

   public:
   OwningListBase() = default;
   OwningListBase(OwningListBase&) = delete;
   OwningListBase operator=(const OwningListBase&) = delete;
   OwningListBase(OwningListBase&&) noexcept = default;
   OwningListBase& operator=(OwningListBase&&) noexcept = default;
   ~OwningListBase() = default;

   /// removes unnecessary allocated space from the last chunk (lazy implementation)
   void compactLastChunk() { list.compactLastChunk(); }

   /// Compare two Lists
   [[nodiscard]] auto operator<=>(const OwningListBase& other) const = default;
   [[nodiscard]] bool operator==(const NonOwningVariant& other) const;

   /// Is the list empty?
   [[nodiscard]] bool empty() const { return list.empty(); }
   /// the size of the list
   [[nodiscard]] std::size_t size() const { return list.size(); }

   /// emplace an element at the back of the list
   template <typename... Args>
   auto emplace_back(Args&&... args) { return list.emplace_back(std::forward<Args>(args)...); }
   /// push an element at the back of the list
   auto push_back(ElementType value) {
      auto ret = emplace_back();
      *ret = std::move(value);
      return ret;
   }

   /// checks if an element (the element itself, not a copy) is contained in this list or not.
   [[nodiscard]] bool containsObject(ElementType& element) const { return list.containsObject(element); }
   /// retrieve the element at specified index, may be implemented very slow and only for test purpose
   [[nodiscard]] ElementType& operator[](std::size_t idx) { return list[idx]; }
   /// retrieve the last element of the list
   [[nodiscard]] ElementType& back() { return list.back(); }

   /// begin iterator of the list
   [[nodiscard]] Iterator begin() { return list.begin(); }
   /// end iterator of the list
   [[nodiscard]] Iterator end() { return list.end(); }
   /// begin iterator of the list
   [[nodiscard]] ConstIterator begin() const { return list.begin(); }
   /// end iterator of the list
   [[nodiscard]] ConstIterator end() const { return list.end(); }
};
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam, ListTypeEnum listType>
class NonOwningListBase {
   friend class OwningListBase<ElementTypeParam, listType>;

   public:
   using ElementType = ElementTypeParam;
   using ConcreteList = ListTypeSelector<listType, ElementType>::NonOwningType;
   using OwningVariant = OwningListBase<ElementType, listType>;

   using Iterator = ConcreteList::Iterator;
   using ConstIterator = ConcreteList::ConstIterator;

   private:
   ConcreteList list;

   public:
   NonOwningListBase() = default;
   NonOwningListBase(NonOwningListBase&) = delete;
   NonOwningListBase operator=(const NonOwningListBase&) = delete;
   NonOwningListBase(NonOwningListBase&&) noexcept = default;
   NonOwningListBase& operator=(NonOwningListBase&&) noexcept = default;
   ~NonOwningListBase() = default;

   /// Compare two Lists
   auto operator<=>(const NonOwningListBase& other) const = default;
   bool operator==(const OwningVariant& other) const { return list == other.list; }

   /// Lazy copy of the list, this list overtakes the ownership if other list had it before
   void operator<<(OwningVariant& other) { list.merge(other.list); }
   /// Lazy copy of the list, this list overtakes the ownership if other list had it before
   void operator<<(NonOwningListBase& other) { list.merge(other.list); }
   /// Lazy merge a node of other list
   // void addElementReference(ElementType& element) { list.addElementReference(element); }
   /// Lazy merge of two lists
   void merge(OwningVariant& other) { list.merge(other.list); }

   /// Is the list empty?
   [[nodiscard]] bool empty() const { return list.empty(); }
   /// the size of the list
   [[nodiscard]] std::size_t size() const { return list.size(); }

   /// retrieve the element at specified index, may be implemented very slow and only for test purpose
   // [[nodiscard]] ElementType& operator[](std::size_t idx) { return list[idx]; }
   /// retrieve the last element of the list
   [[nodiscard]] ElementType& back() const { return list.back(); }

   /// begin iterator of the list
   [[nodiscard]] Iterator begin() { return list.begin(); }
   /// end iterator of the list
   [[nodiscard]] Iterator end() { return list.end(); }

   /// begin iterator of the list
   [[nodiscard]] ConstIterator begin() const { return list.begin(); }
   /// end iterator of the list
   [[nodiscard]] ConstIterator end() const { return list.end(); }
};
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam, ListTypeEnum listType>
inline bool OwningListBase<ElementTypeParam, listType>::operator==(const NonOwningVariant& other) const { return list == other.list; }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------