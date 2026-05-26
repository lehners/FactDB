#pragma once
// ---------------------------------------------------------------------------------------------------
#include <cassert>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <typename T>
void NonOwningLinkedList<T>::addElementReference(ElementType& element) {
   auto* nodePtr = reinterpret_cast<OwningVariant::Node*>(&element);
   list.emplace_back(nodePtr);
   ++sizeVal;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
void NonOwningLinkedList<T>::merge(OwningVariant& otherList) {
   list.emplace_back(otherList.firstEntry);
   sizeVal += otherList.size();
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
void NonOwningLinkedList<T>::merge(NonOwningLinkedList& otherList) {
   for (typename OwningLinkedList<ElementType>::Node* firstEntries : otherList.list)
      list.emplace_back(firstEntries);
   sizeVal += otherList.size();
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <bool isConst>
NonOwningLinkedList<T>::reverseIteratorImpl<isConst>& NonOwningLinkedList<T>::reverseIteratorImpl<isConst>::operator++() {
   if ((++curListElem).isLast())
      curListElem = iteratorTypeInner{(++curList).isLast() ? nullptr : *curList};
   return *this;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <bool isConst>
NonOwningLinkedList<T>::reverseIteratorImpl<isConst>::reference NonOwningLinkedList<T>::reverseIteratorImpl<isConst>::operator*() const {
   assert(!curListElem.isLast());
   return *curListElem;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <bool isConst>
NonOwningLinkedList<T>::reverseIteratorImpl<isConst>::pointer NonOwningLinkedList<T>::reverseIteratorImpl<isConst>::operator->() const {
   assert(!curListElem.isLast());
   return curListElem.operator->();
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <bool isConst>
NonOwningLinkedList<T>::iteratorImplSlow<isConst>& NonOwningLinkedList<T>::iteratorImplSlow<isConst>::operator++() {
   if (++curListElem.isLast()) {
      curListElem = iteratorTypeInner{(++curList).isLast() ? nullptr : *curList}; // go to previous list
   }
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <bool isConst>
NonOwningLinkedList<T>::iteratorImplSlow<isConst>& NonOwningLinkedList<T>::iteratorImplSlow<isConst>::operator--() {
   if ((--curListElem).isLast())
      curListElem = iteratorTypeInner{(--curList).isLast() ? nullptr : *curList};
   return *this;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <bool isConst>
NonOwningLinkedList<T>::iteratorImplSlow<isConst>::reference NonOwningLinkedList<T>::iteratorImplSlow<isConst>::operator*() const {
   assert(!curListElem.isLast());
   return *curListElem;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <bool isConst>
NonOwningLinkedList<T>::iteratorImplSlow<isConst>::pointer NonOwningLinkedList<T>::iteratorImplSlow<isConst>::operator->() const {
   assert(!curListElem.isLast());
   return curListElem.operator->();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------