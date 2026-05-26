#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/list/ChunkedList.hpp"
#include "factDB/infra/list/LinkedList.hpp"
#include "factDB/infra/list/ListTypeEnum.hpp"
#include "factDB/infra/list/STLDeque.hpp"
#include "factDB/infra/list/StlVector.hpp"
#include "factDB/infra/list/TLFastDeque.hpp"
#include "factDB/infra/list/TLSDeque.hpp"
#include "factDB/infra/list/TbbConcurrentVector.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <ListTypeEnum lt, typename T>
struct ListTypeSelector;
// ---------------------------------------------------------------------------------------------------
template <typename T>
struct ListTypeSelector<ChunkedList, T> {
   using OwningType = OwningChunkedList<T>;
   using NonOwningType = NonOwningChunkedList<T>;
};
// ---------------------------------------------------------------------------------------------------
template <typename T>
struct ListTypeSelector<LinkedList, T> {
   using OwningType = OwningLinkedList<T>;
   using NonOwningType = NonOwningLinkedList<T>;
};
// ---------------------------------------------------------------------------------------------------
template <typename T>
struct ListTypeSelector<TbbConcurrentVector, T> {
   using OwningType = OwningTbbConcurrentVector<T>;
   using NonOwningType = NonOwningTbbConcurrentVector<T>;
};
// ---------------------------------------------------------------------------------------------------
template <typename T>
struct ListTypeSelector<InlineConcurrentVector, T> {
   using OwningType = OwningTbbConcurrentVector<T>;
   using NonOwningType = NonOwningTbbConcurrentVector<T>;
};
// ---------------------------------------------------------------------------------------------------
template <typename T>
struct ListTypeSelector<STLVector, T> {
   using OwningType = OwningStlVector<T>;
   using NonOwningType = NonOwningStlVector<T>;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------