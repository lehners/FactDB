#pragma once
// ---------------------------------------------------------------------------------------------------
#include "tbb/scalable_allocator.h"
#include <cassert>
#include <cstddef>
#include <cstring>
#include <mutex>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra::list {
// ---------------------------------------------------------------------------------------------------
constexpr inline unsigned long next_power_of_two(unsigned long v) {
   v--;
   v |= v >> 1;
   v |= v >> 2;
   v |= v >> 4;
   v |= v >> 8;
   v |= v >> 16;
   v++;
   return v;
}
// ---------------------------------------------------------------------------------------------------
inline bool is_power_of_two(unsigned long v) {
   return (v & (v - 1)) == 0;
}
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam, size_t inlinedChunksParam = 0>
class FastDeque {
   public:
   using ElementType = ElementTypeParam;
   using Allocator = ::tbb::scalable_allocator<uint8_t>;
   using AllocTraits = std::allocator_traits<Allocator>;

   private:
   [[nodiscard]] static constexpr size_t chunkSize(size_t chunkIdx) { return 1ul << chunkIdx; }
   [[nodiscard]] static constexpr size_t chunkStartIdx(size_t chunkIdx) { return (1ul << chunkIdx) - 1; }
   [[nodiscard]] static constexpr size_t chunkCount(size_t targetSize) { return (64 - __builtin_clzll(targetSize)) * (targetSize != 0); }
   [[nodiscard]] constexpr size_t chunkCount() const { return chunkCount(size_); }

   static constexpr size_t inlinedChunks = inlinedChunksParam;
   static constexpr size_t inlinedElements = chunkStartIdx(inlinedChunks); // -1 for idx +1 for next chunk
   alignas(ElementType) uint8_t firstChunkBuffer[sizeof(ElementType) * inlinedElements];
   ElementType** chunks = nullptr;
   size_t size_ = 0;
   Allocator allocator;

   ElementType* getInlinedChunk() { return reinterpret_cast<ElementType*>(firstChunkBuffer); }
   const ElementType* getInlinedChunk() const { return reinterpret_cast<const ElementType*>(firstChunkBuffer); }

   [[gnu::always_inline]] ElementType* allocateChunk(size_t chunkIdx) {
      if (chunkIdx < inlinedChunks) return getInlinedChunk();
      ElementType* allocation = reinterpret_cast<ElementType*>(allocator.allocate(chunkSize(chunkIdx) * sizeof(ElementType)));
      allocation -= chunkStartIdx(chunkIdx); // move pointer by index for easier access (no computations required)
      return allocation;
   }

   public:
   FastDeque() = default;
   FastDeque(FastDeque&& other) noexcept : chunks(std::move(other.chunks)), size_(other.size_), allocator(std::move(other.allocator)) {
      *reinterpret_cast<ElementType*>(firstChunkBuffer) = std::move(*reinterpret_cast<ElementType*>(other.firstChunkBuffer));
      other.chunks = nullptr;
      other.size_ = 0;
      if (chunks != nullptr) {
         for (size_t i = 0; i != inlinedChunks; ++i) {
            chunks[i] = getInlinedChunk();
         }
      }
   }

   template <bool IsConst>
   class iterator_base {
      public:
      using ElementTypePtr = std::conditional_t<IsConst, const ElementType*, ElementType*>;
      using ElementTypeRef = std::conditional_t<IsConst, const ElementType&, ElementType&>;
      using pointer = ElementTypePtr;
      using reference = ElementTypeRef;
      using difference_type = std::ptrdiff_t;
      using iterator_category = std::random_access_iterator_tag;

      private:
      ElementType** curChunk = nullptr;
      ElementTypePtr curElement = nullptr;
      size_t idx_ = 0;
      size_t size_ = 0;

      public:
      iterator_base() = default;
      iterator_base(ElementType** chunk, ElementTypePtr firstChunkBuffer, size_t size, size_t idx = 0)
         : curChunk(chunk), curElement(inlinedChunks == 0 ? (chunk ? chunk[0] : nullptr) : firstChunkBuffer), size_(size) {
         if (idx != 0) *this += idx;
      }

      [[gnu::always_inline]] iterator_base& operator++() {
         assert(size_ <= inlinedElements || (curChunk != nullptr && curElement != nullptr));

         ++idx_;
         if (size_ > inlinedElements) {
            curChunk += ((idx_ + 1) & idx_) == 0;
            curElement = *curChunk + idx_;
         } else {
            ++curElement;
         }
         return *this;
      }

      [[gnu::always_inline]] iterator_base& operator--() {
         assert(idx_ != 0);
         --idx_;
         if (size_ > inlinedElements) {
            curChunk -= ((idx_ + 1) & (idx_ + 2)) == 0; // need to increment chunk and element ptr
            curElement = *curChunk + idx_;
         } else {
            --curElement;
         }
         return *this;
      }

      [[gnu::always_inline]] iterator_base& operator+=(const size_t diff) {
         assert(/*0 <= idx_ + diff &&*/ idx_ + diff <= size_);
         assert(curChunk != nullptr || size_ <= inlinedElements);
         auto curChunkIdx = chunkCount(idx_ + 1);
         auto targetChunkIdx = chunkCount(idx_ + 1 + diff);

         idx_ += diff;
         if (size_ > inlinedElements) {
            curChunk += targetChunkIdx - curChunkIdx;
            curElement = *curChunk + idx_;
         } else {
            assert(curChunk == nullptr);
            curElement += diff;
         }
         return *this;
      }

      iterator_base operator+(size_t diff) const {
         iterator_base tmp = *this;
         return tmp += diff;
      }
      iterator_base operator-(size_t diff) const {
         iterator_base tmp = *this;
         return tmp += -diff;
      }
      difference_type operator-(const iterator_base& other) const { return static_cast<difference_type>(idx_) - static_cast<difference_type>(other.idx_); }

      /// compare two iterators for equality
      bool operator==(const iterator_base& other) const { return curChunk == other.curChunk && curElement == other.curElement; }
      /// compare two iterators for inequality
      bool operator!=(const iterator_base& other) const { return !(*this == other); }

      /// Dereference
      [[nodiscard]] reference operator*() const { return *curElement; }
      /// Member access
      [[nodiscard]] pointer operator->() const { return curElement; }

      /// sets the iterator to the end
      void setEnd() {
         curChunk = nullptr;
         curElement = nullptr;
      }
      /// points the iterator to the last element of the list?
      [[nodiscard]] bool isLast() const {
         auto res = idx_ == size_ || !curElement;
         return res;
      }
   };

   using const_iterator = iterator_base<true>;
   using iterator = iterator_base<false>;
   using Iterator = iterator;
   using ConstIterator = const_iterator;

   ~FastDeque() noexcept {
      if (!chunks) { // clear inline chunks
         auto inlineChunk = getInlinedChunk();
         for (size_t i = 0; i < size_; ++i, ++inlineChunk)
            inlineChunk->~ElementType();
      } else {
         for (size_t idx = 0, count = chunkCount(); idx < count; ++idx) {
            auto chunkStart = chunkStartIdx(idx);
            auto* curChunkIter = chunks[idx] + chunkStart;
            for (size_t i = 0; i < chunkSize(idx) && (chunkStart + i) < size_; ++i, ++curChunkIter) { // increment the chunk iter after each element is deleted
               curChunkIter->~ElementType();
            }
            if (idx >= inlinedChunks)
               allocator.deallocate(reinterpret_cast<uint8_t*>(chunks[idx] + chunkStart), chunkSize(idx) * sizeof(ElementType));
         }
         auto curChunkSize = chunkCount(size_);
         allocator.deallocate(reinterpret_cast<uint8_t*>(chunks), (next_power_of_two(curChunkSize) + 1) * sizeof(ElementType**));
         chunks = nullptr;
      }
   }

   [[nodiscard]] bool empty() const { return size_ == 0; }
   [[nodiscard]] size_t size() const { return size_; }

   void reserve(size_t /*targetSize*/) { /*not_implemented();*/
   } // todo
   void resize(size_t targetSize) {
      if (targetSize > size_) {
         resizeImpl(targetSize);
         for (size_t i = 0; i < inlinedChunks; ++i) {
            chunks[i] = getInlinedChunk();
         }
      }
   }

   void resizeChunkArray(size_t targetChunkCount) {
      auto curChunkSize = chunkCount(size_);
      auto currentCapacity = next_power_of_two(curChunkSize);
      auto requiredCapacity = next_power_of_two(targetChunkCount);

      if (requiredCapacity > currentCapacity) [[unlikely]] {
         auto* oldChunks = chunks;
         chunks = reinterpret_cast<ElementType**>(allocator.allocate((requiredCapacity + 1) * sizeof(ElementType**)));
         if (oldChunks) [[likely]] {
            std::memcpy(chunks, oldChunks, currentCapacity * sizeof(ElementType**));
            allocator.deallocate(reinterpret_cast<uint8_t*>(oldChunks), (currentCapacity + 1) * sizeof(ElementType**));
         }
         // assume that all chunks between curChunkSize and targetChunkCount will be written later on, hence we do not have to set them to 0
         std::memset(chunks + targetChunkCount, 0, (requiredCapacity - targetChunkCount + 1) * sizeof(ElementType**));
      }
   }

   void resizeImpl(size_t targetSize) {
      if (targetSize < inlinedChunks)
         return;
      assert(targetSize > size_);

      auto targetChunkSize = chunkCount(targetSize);
      auto curChunkSize = chunkCount(size_);
      if (targetChunkSize == curChunkSize)
         return;

      assert(curChunkSize < targetChunkSize);
      resizeChunkArray(targetChunkSize);

      for (size_t idx = curChunkSize; idx < targetChunkSize; ++idx) {
         chunks[idx] = allocateChunk(idx);
      }

      size_ = targetSize;
   }

   [[gnu::always_inline]] inline ElementType* resizeImplByOne() {
      // returns a pointer to the last chunk
      auto curChunkSize = chunkCount(size_);
      if (curChunkSize < inlinedChunks) {
         return getInlinedChunk();
      }

      if ((size_ & (size_ + 1)) != 0) [[likely]]
         return chunks[curChunkSize - 1];

      // Grow more than one chunk ahead
      auto targetChunkSize = curChunkSize + 1;
      resizeChunkArray(targetChunkSize); // Will grow exponentially
      if (curChunkSize == inlinedChunks) {
         for (size_t i = 0; i < inlinedChunks; ++i) {
            chunks[i] = getInlinedChunk();
         }
      }
      chunks[targetChunkSize - 1] = allocateChunk(targetChunkSize - 1);
      return chunks[targetChunkSize - 1];
   }

   template <typename... Args>
   [[gnu::always_inline]] inline iterator emplace_back(Args&&... args) {
      auto* curChunk = resizeImplByOne();
      auto curIdx = size_++;
      AllocTraits::construct(allocator, curChunk + curIdx, std::forward<Args>(args)...);
      return {chunks, getInlinedChunk(), size_, curIdx};
   }

   [[nodiscard]] iterator begin() { return iterator(chunks, getInlinedChunk(), size_); }
   [[nodiscard]] iterator end() { return iterator(chunks, getInlinedChunk(), size_, size_); }
   [[nodiscard]] const_iterator begin() const { return const_iterator(chunks, getInlinedChunk(), size_); }
   [[nodiscard]] const_iterator end() const { return const_iterator(chunks, getInlinedChunk(), size_, size_); }

   ElementType& operator[](size_t idx) {
      assert(idx < size());
      auto iter = begin();
      iter += idx;
      return *iter;
   }

   bool operator==(const FastDeque& other) const {
      return size_ == other.size_ && chunks == other.chunks;
   }
};
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam, size_t inlinedChunksParam = 0>
class FastDequeLockable : public FastDeque<ElementTypeParam, inlinedChunksParam> {
   public:
   std::mutex lock = {};

   FastDequeLockable() = default;

   FastDequeLockable(const FastDequeLockable& other) = delete;
   FastDequeLockable(FastDequeLockable&& other)
      : FastDeque<ElementTypeParam, inlinedChunksParam>(std::move(other)) {}

   FastDequeLockable& operator=(const FastDequeLockable& other) = delete;
   FastDequeLockable& operator=(FastDequeLockable&& other) {
      this->list = other.list;
      return *this;
   }
};
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam, size_t inlinedChunksParam = 0>
class NonOwningFastDeque {
   public:
   using ElementType = ElementTypeParam;
   using OwningVariant = FastDeque<ElementType, inlinedChunksParam>;
   using PtrType = OwningVariant*;

   private:
   PtrType listPtr = nullptr;

   public:
   using Iterator = typename OwningVariant::iterator;
   using ConstIterator = typename OwningVariant::const_iterator;

   NonOwningFastDeque() = default;
   ~NonOwningFastDeque() = default;

   auto operator<=>(const NonOwningFastDeque&) const = default;

   void merge(OwningVariant& otherList) {
      assert(listPtr == nullptr);
      listPtr = &otherList;
   }
   void merge(const NonOwningFastDeque& otherList) {
      assert(listPtr == nullptr);
      listPtr = otherList.listPtr;
   }

   [[nodiscard]] size_t size() const { return listPtr->size(); }
   [[nodiscard]] bool empty() const { return listPtr->empty(); }

   [[nodiscard]] Iterator begin() { return listPtr->begin(); }
   [[nodiscard]] Iterator end() { return listPtr->end(); }
   [[nodiscard]] ConstIterator begin() const { return static_cast<const OwningVariant* const>(listPtr)->begin(); }
   [[nodiscard]] ConstIterator end() const { return static_cast<const OwningVariant* const>(listPtr)->end(); }
};
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
using NonOwningFastDequeInlined = NonOwningFastDeque<ElementTypeParam, 1>;
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
using FastDequeInlinedLockable = FastDequeLockable<ElementTypeParam, 1>;
// ---------------------------------------------------------------------------------------------------
template <typename ElementTypeParam>
using FastDequeInlined = FastDeque<ElementTypeParam, 1>;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra::list
// ---------------------------------------------------------------------------------------------------
