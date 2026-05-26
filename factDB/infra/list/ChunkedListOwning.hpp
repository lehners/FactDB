#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include <shared_mutex>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <typename T>
class NonOwningChunkedList;
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class OwningChunkedList : public ChunkedListBase<ElementTypeParam> {
   friend class NonOwningChunkedList<ElementTypeParam>;
   using ParentClass = ChunkedListBase<ElementTypeParam>;

   public:
   using ElementType = ElementTypeParam;
   using NonOwningVariant = NonOwningChunkedList<ElementType>;

   using Iterator = ParentClass::Iterator;
   using ConstIterator = ParentClass::ConstIterator;

   private:
   std::shared_mutex growLock;
   size_t sizeVal = 0;
   size_t emptySlots = 0;

   /// allocate memory
   ChunkedListBase<ElementType>::ChunkInfo allocate(size_t allocSize = 20);

   public:
   OwningChunkedList() = default;
   OwningChunkedList(OwningChunkedList&) = delete;
   OwningChunkedList operator=(const OwningChunkedList&) = delete;
   OwningChunkedList(OwningChunkedList&&) noexcept = default;
   OwningChunkedList& operator=(OwningChunkedList&&) noexcept = default;
   ~OwningChunkedList();

   void reserve(size_t size);
   /// emplace an element at the back of the list
   template <typename... Args>
   ParentClass::Iterator emplace(Args&&... args);
   /// emplace an element at the back of the list
   template <typename... Args>
   ParentClass::Iterator emplace_back(Args&&... args) { return emplace(std::forward<Args>(args)...); }

   Iterator push_back(ElementType type);

   /// removes unnecessary allocated space from the last chunk (lazy implementation)
   void compactLastChunk() { emptySlots = 0; }

   /// the size of the list
   [[nodiscard]] size_t size() const { return sizeVal; }

   /// retrieve the element at specified index, may be implemented very slow and only for test purpose
   [[nodiscard]] bool operator==(const OwningChunkedList& other) const { return static_cast<const ParentClass&>(*this) == static_cast<const ParentClass&>(other); }
   // [[nodiscard]] bool operator==(const NonOwningChunkedList<T>& other) const { return this->list == other.list; }
   [[nodiscard]] ElementType& operator[](size_t idx);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------
