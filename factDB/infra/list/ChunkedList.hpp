#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include <atomic>
#include <cassert>
#include <cstdint>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
class ChunkedListBase {
   public:
   // friend class OwningChunkedList<T>;
   // friend class NonOwningChunkedList<T>;

   /// the element type
   using ElementType = ElementTypeParam;

   /// the chunk descriptions
   struct ChunkInfo {
      ElementType* begin;
      ElementType* end;

      bool operator==(const ChunkInfo& other) const { return begin == other.begin; }
   };

   protected:
   /// the chunks
   std::vector<ChunkInfo> chunks;

   /// the iterator
   template <bool isConst>
   class iteratorImpl {
      friend class ChunkedListBase;

      using value_type = std::conditional_t<isConst, const ElementType, ElementType>;

      const ChunkInfo* curChunk = nullptr;
      value_type* cur_element = nullptr;
      const ChunkInfo* lastChunk = nullptr;

      public:
      iteratorImpl() = default;
      iteratorImpl(const ChunkInfo* curChunkParam, value_type* curElementParam, const ChunkInfo* lastChunkParam) : curChunk(curChunkParam), cur_element(curElementParam), lastChunk(lastChunkParam) {}

      // Pre-increment
      iteratorImpl& operator++();
      // Pre-decrement
      iteratorImpl& operator--();

      /// compare two iterators for equality
      [[nodiscard]] bool operator==(const iteratorImpl& other) const { return curChunk == other.curChunk && cur_element == other.cur_element; }
      /// compare two iterators for inequality
      [[nodiscard]] bool operator!=(const iteratorImpl& other) const { return curChunk != other.curChunk || cur_element != other.cur_element; }

      /// Dereference
      [[nodiscard]] value_type& operator*() const { return *cur_element; }
      /// Member access
      [[nodiscard]] value_type* operator->() const { return cur_element; }

      /// sets the iterator to the end
      void setEnd() { curChunk = nullptr, lastChunk = nullptr; }
      /// points the iterator to the last element of the list?
      [[nodiscard]] inline bool isLast() const { return curChunk == lastChunk && (!lastChunk || cur_element == lastChunk->end); }
   };

   public:
   ChunkedListBase() : chunks({}) {}
   ChunkedListBase(ChunkedListBase&) = delete;
   ChunkedListBase operator=(const ChunkedListBase&) = delete;
   ChunkedListBase(ChunkedListBase&&) noexcept = default;
   ChunkedListBase& operator=(ChunkedListBase&&) noexcept = default;
   ~ChunkedListBase() = default;

   using Iterator = iteratorImpl<false>;
   using ConstIterator = iteratorImpl<true>;

   [[nodiscard]] bool empty() const { return chunks.empty(); }
   [[nodiscard]] bool containsObject(ElementType&) const { not_implemented(); }
   [[nodiscard]] ElementType& back() const { return *(chunks.back().end - 1); }

   [[nodiscard]] bool operator==(const ChunkedListBase& other) const { return chunks == other.chunks; }

   [[nodiscard]] Iterator begin() { return !chunks.empty() ? Iterator{&chunks.front(), chunks.front().begin, &chunks.back()} : Iterator{}; }
   [[nodiscard]] Iterator end() { return !chunks.empty() ? Iterator{&chunks.back(), chunks.back().end, &chunks.back()} : Iterator{}; }
   [[nodiscard]] ConstIterator begin() const { return !chunks.empty() ? ConstIterator{&chunks.front(), chunks.front().begin, &chunks.back()} : ConstIterator{}; }
   [[nodiscard]] ConstIterator end() const { return !chunks.empty() ? ConstIterator{&chunks.back(), chunks.back().end, &chunks.back()} : ConstIterator{}; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/list/ChunkedListNonOwning.hpp"
#include "factDB/infra/list/ChunkedListOwning.hpp"
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/list/ChunkedListBase.tpp"
// ---------------------------------------------------------------------------------------------------
