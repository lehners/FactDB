#pragma once
// ---------------------------------------------------------------------------------------------------
#include <cassert>
#include <cstdlib>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <typename T>
OwningLinkedList<T>::~OwningLinkedList() {
   while (firstEntry) {
      Node* curNext = firstEntry->next;
      firstEntry->~Node();
      std::free(firstEntry);
      firstEntry = curNext;
   }
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <typename... Args>
OwningLinkedList<T>::Iterator OwningLinkedList<T>::emplace_back(Args... args) {
   void* newBack = std::malloc(sizeof(Node));
   new (newBack) Node({std::forward<Args>(args)...});
   ++std::atomic_ref(sizeVal);

   // swap first entry with node.next
   Node* node = static_cast<Node*>(newBack);
   node->next = std::atomic_ref(firstEntry).exchange(node);

   return {node};
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
bool OwningLinkedList<T>::containsObject(ElementType& element) const {
   void* elementPtr = &element;
   for (auto& cur : *this)
      if (&cur == elementPtr)
         return true;
   return false;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
OwningLinkedList<T>::ElementType& OwningLinkedList<T>::back() const {
   assert(firstEntry);
   return firstEntry->value;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
OwningLinkedList<T>::ElementType& OwningLinkedList<T>::operator[](size_t idx) const {
   assert(idx < sizeVal);
   idx = sizeVal - idx - 1;

   auto* curElem = firstEntry;
   while (idx && curElem) {
      curElem = curElem->next;
      --idx;
   }
   assert(idx == 0 && "index larger than chain length");
   return curElem->value;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <bool isConst>
OwningLinkedList<T>::reverseIteratorImpl<isConst>& OwningLinkedList<T>::reverseIteratorImpl<isConst>::operator++() {
   curElement = curElement->next;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <bool isConst>
OwningLinkedList<T>::reverseIteratorImpl<isConst>& OwningLinkedList<T>::reverseIteratorImpl<isConst>::operator+=(size_t diff) {
   for (size_t i = 0; i < diff; i++) {
      ++(*this);
   }
   return *this;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <bool isConst>
OwningLinkedList<T>::iteratorImplSlow<isConst>& OwningLinkedList<T>::iteratorImplSlow<isConst>::operator++() {
   // forward = get previous element, therefore, we have to traverse the list, since we do not have backward pointers
   if (curElement == nullptr || list->firstEntry == curElement) {
      curElement = nullptr;
      return *this;
   }

   Node* curPtr = list->firstEntry;
   while (curPtr->next != curElement) {
      assert(curPtr->next != nullptr);
      curPtr = curPtr->next;
   }
   assert(curPtr->next == curElement);
   curElement = curPtr;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <bool isConst>
OwningLinkedList<T>::iteratorImplSlow<isConst>& OwningLinkedList<T>::iteratorImplSlow<isConst>::operator--() {
   // backward = next element
   if (curElement == nullptr && list != nullptr) {
      curElement = list->firstEntry;
      return *this;
   }
   curElement = curElement->next;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------