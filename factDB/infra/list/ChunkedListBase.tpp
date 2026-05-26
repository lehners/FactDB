#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/util/DoOnDestruction.hpp"
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <source_location>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <bool isConst>
ChunkedListBase<T>::iteratorImpl<isConst>& ChunkedListBase<T>::iteratorImpl<isConst>::operator++() {
   // do not go to next chunk if already in last chunk, this would lead to UB
   if (++cur_element == curChunk->end && curChunk != lastChunk) {
      curChunk++;
      cur_element = curChunk->begin;
      assert(curChunk->begin != curChunk->end);
   }
   return *this;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <bool isConst>
ChunkedListBase<T>::iteratorImpl<isConst>& ChunkedListBase<T>::iteratorImpl<isConst>::operator--() {
   if (cur_element == curChunk->begin) {
      curChunk--;
      cur_element = curChunk->end;
      // cur_element decrement required since curChunk->end points is past-the-end pointer
   }
   --cur_element;
   return *this;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
OwningChunkedList<T>::~OwningChunkedList() {
   for (auto& elem : *this)
      elem.~ElementType();
   for (auto c : this->chunks)
      std::free(c.begin);
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
T& OwningChunkedList<T>::operator[](size_t idx) {
   // naive and slow
   for (const auto& ci : this->chunks) {
      size_t size = ci.end - ci.begin;
      if (idx < size) {
         return *(ci.begin + idx);
      } else {
         idx -= size;
      }
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
typename ChunkedListBase<T>::ChunkInfo OwningChunkedList<T>::allocate(size_t allocSize) {
   assert(emptySlots == 0);
   emptySlots += allocSize;
   auto* chunkStart = static_cast<T*>(std::malloc(sizeof(T) * allocSize)); // todo
   return {chunkStart, chunkStart};
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
void OwningChunkedList<T>::reserve(size_t size) {
   assert(emptySlots == 0 && sizeVal == 0);
   std::lock_guard lock(growLock);
   this->chunks.push_back(allocate(size));
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
template <typename... Args>
OwningChunkedList<T>::ParentClass::Iterator OwningChunkedList<T>::emplace(Args&&... args) {
   growLock.lock_shared();
   // Locker locked(growLock);
   std::atomic_ref emptySlotsAtomic(emptySlots);
   {
      auto curSize = emptySlotsAtomic.load();
      do {
         while (curSize == 0) {
            growLock.unlock_shared();
            {
               std::lock_guard lockedExclusive(growLock);
               curSize = emptySlotsAtomic.load();
               if (curSize == 0)
                  this->chunks.push_back(allocate(std::max(100, (int) (std::atomic_ref(sizeVal).load()*2))));
            }
            growLock.lock_shared();
            curSize = emptySlotsAtomic.load();
         }
      } while (!emptySlotsAtomic.compare_exchange_weak(curSize, curSize - 1));
   }
   ++std::atomic_ref(sizeVal);
   auto curElem = std::atomic_ref(this->chunks.back().end)++;
   new (curElem) T(std::forward<Args>(args)...);

   auto ret = typename OwningChunkedList<T>::Iterator(&this->chunks.back(), curElem, &this->chunks.back());

   growLock.unlock_shared();
   return ret;
   // todo fixme: will fail if list of chunks grows (pointer instability)
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
OwningChunkedList<T>::ParentClass::Iterator OwningChunkedList<T>::push_back(ElementType element) {
   auto ret = emplace_back();
   *ret = std::move(element);
   return ret;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------