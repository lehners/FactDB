#ifndef H_FACTDB_INFRA_ALLOCATORPOOLED_HPP
#define H_FACTDB_INFRA_ALLOCATORPOOLED_HPP
// ---------------------------------------------------------------------------------------------------
#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <new>
// ---------------------------------------------------------------------------------------------------
namespace factDB::pool {
// ---------------------------------------------------------------------------------------------------
template <typename T>
class PooledAllocator {
   private:
   /// A chunk of this allocator
   struct ChunkHeader {
      /// Start of currently usable memory
      T* begin;
      /// Pointer to end of usable memory
      T* end;
      /// Pointer to next chunk
      ChunkHeader* nextChunk;
   };

   /// Pointer to first chunk
   ChunkHeader* firstChunk = nullptr;
   /// Pointer to current chunk
   ChunkHeader* currentChunk = nullptr;

   /// Allocate a chunk that can hold n objects
   ChunkHeader* allocateChunk(size_t n);

   /// Lock for parallel allocations
   std::mutex allocationLock;

   /// Get the size if a ChunkHeader including padding for Type T
   static size_t getChunkHeaderSize();
   /// Get the pointer to the first allocation in the chunk
   static T* getFirstAllocation(ChunkHeader* chunk);
   /// Free all chunks
   void freeChunks();

   public:
   /// The value Type
   using value_type = T;

   /// Get the allocator for Type U
   template <typename U>
   using rebind = PooledAllocator<U>;

   /// Constructor
   PooledAllocator() = default;
   /// Copy constructor
   PooledAllocator(const PooledAllocator&) = delete;
   /// Move constructor
   PooledAllocator(PooledAllocator&&) noexcept;
   /// Destructor, deallocates all storage
   ~PooledAllocator();

   /// Copy assignment
   PooledAllocator& operator=(const PooledAllocator&) = delete;
   /// Move assignment
   PooledAllocator& operator=(PooledAllocator&&) noexcept;

   /// Allocate storage for an object of Type T
   T* allocate();
   /// Free storage previously allocated with allocate()
   // void deallocate(T* ptr);

   class Iterator {
      public:
      using value_type = T;
      using difference_type = std::ptrdiff_t;
      using reference = T&;
      using pointer = T*;
      using iterator_category = std::forward_iterator_tag;

      friend class PooledAllocator<T>;

      private:
      /// The current tuple
      T* currentTuple;
      /// The end of the current bucket
      ChunkHeader* currentChunk;

      /// Constructor
      Iterator(T* currentTupleParam, ChunkHeader* currentChunkParam) : currentTuple(currentTupleParam), currentChunk(currentChunkParam){};

      public:
      /// Default constructor
      Iterator() : currentTuple(nullptr), currentChunk(nullptr){};
      /// Copy constructor
      Iterator(const Iterator&) = default;
      /// Copy assignment
      Iterator& operator=(const Iterator&) = default;

      /// Dereference
      reference operator*() const;
      /// Member access
      pointer operator->() const;

      /// Pre-increment
      Iterator& operator++();
      /// Post-increment
      Iterator operator++(int);

      /// Equality
      bool operator==(const Iterator& it) const;
   };

   /// Return Iterator to beginning of table
   Iterator begin();
   /// Return past-the-end Iterator
   Iterator end();
};
// ---------------------------------------------------------------------------------------------------
template <typename T>
PooledAllocator<T>::PooledAllocator(PooledAllocator<T>&& other) noexcept
   : firstChunk(other.firstChunk),
     currentChunk(other.currentChunk)
// Move constructor
{
   other.firstChunk = nullptr;
   other.currentChunk = nullptr;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
PooledAllocator<T>& PooledAllocator<T>::operator=(PooledAllocator<T>&& other) noexcept
// Move assignment
{
   if (this != &other) {
      freeChunks();

      firstChunk = other.firstChunk;
      currentChunk = other.currentChunk;

      other.firstChunk = nullptr;
      other.currentChunk = nullptr;
   }

   return *this;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
void PooledAllocator<T>::freeChunks()
// Free all chunks
{
   auto* chunk = firstChunk;

   while (chunk) {
      T* curElement = getFirstAllocation(chunk);
      while (curElement != chunk->begin) {
         curElement->~T();
         ++curElement;
      }

      auto* nextChunk = chunk->nextChunk;
      std::free(chunk);
      chunk = nextChunk;
   }
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
PooledAllocator<T>::~PooledAllocator()
// Destructor, deallocates all storage
{
   freeChunks();
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
size_t PooledAllocator<T>::getChunkHeaderSize()
// Get the size if a ChunkHeader including padding for Type T
{
   if (alignof(ChunkHeader) < alignof(T)) {
      // Compute the amount of padding
      size_t padding = alignof(T) - (sizeof(ChunkHeader) % alignof(T));

      return sizeof(ChunkHeader) + padding;
   } else {
      return sizeof(ChunkHeader);
   }
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
T* PooledAllocator<T>::getFirstAllocation(ChunkHeader* chunk)
/// Get the pointer to the first allocation in the chunk
{
   return reinterpret_cast<T*>(reinterpret_cast<char*>(chunk) + getChunkHeaderSize());
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
typename PooledAllocator<T>::ChunkHeader* PooledAllocator<T>::allocateChunk(size_t n)
// Allocate a chunk that can hold n objects
{
   size_t allocSize = getChunkHeaderSize() + sizeof(T) * n;

   void* rawChunk = std::malloc(allocSize);
   auto* chunk = new (rawChunk) ChunkHeader();

   chunk->begin = getFirstAllocation(chunk);
   chunk->end = chunk->begin + n;
   chunk->nextChunk = nullptr;

   return chunk;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
T* PooledAllocator<T>::allocate()
// Allocate storage for an object of Type T
{
   std::atomic_ref firstChunkRef(firstChunk), currentChunkRef(currentChunk);
   if (firstChunkRef.load() == nullptr) {
      std::lock_guard lock(allocationLock);
      if (firstChunkRef.load() == nullptr) {
         auto* allocatedChunk = allocateChunk(std::max<size_t>(2048 / sizeof(T), 1));
         currentChunkRef.exchange(allocatedChunk);
         firstChunkRef.exchange(allocatedChunk);
      }
   }

   while (true) {
      auto* currentChunkGotOuter = currentChunkRef.load();
      std::atomic_ref curBeginOuter(currentChunkGotOuter->begin);
      auto* ptr = curBeginOuter.fetch_add(1);
      if (ptr < currentChunkGotOuter->end) {
         assert(ptr < currentChunkGotOuter->end);
         return new (ptr) T;
      } else { // allocate new space
         curBeginOuter.compare_exchange_weak(++ptr, currentChunkGotOuter->end);
         std::lock_guard lock(allocationLock);
         auto currentChunkGotInner = currentChunkRef.load();
         std::atomic_ref curBeginInner(currentChunkGotInner->begin);
         if (curBeginInner.load() < currentChunkGotInner->end)
            continue;

         auto* currentChunkBegin = getFirstAllocation(currentChunk);
         auto* newChunk = allocateChunk((currentChunk->end - currentChunkBegin) * 2);
         ptr = newChunk->begin++;
         currentChunkGotInner->nextChunk = newChunk;
         currentChunkRef.exchange(newChunk);
         return new (ptr) T;
      }
   }
}
// ---------------------------------------------------------------------------------------------------
/*template <typename T>
void PooledAllocator<T>::deallocate(T* ptr)
// Free storage previously allocated with allocate()
{
   if ((ptr + 1) == currentChunk->begin) {
      --(currentChunk->begin);
   }
}*/
// ---------------------------------------------------------------------------------------------------
template <typename T>
typename PooledAllocator<T>::Iterator PooledAllocator<T>::begin()
// Return Iterator to beginning of allocator
{
   if (firstChunk == nullptr)
      return {nullptr, nullptr};
   return {getFirstAllocation(firstChunk), firstChunk};
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
typename PooledAllocator<T>::Iterator PooledAllocator<T>::end()
// Return Iterator to beginning of allocator
{
   if (firstChunk == nullptr)
      return {nullptr, nullptr};
   return {currentChunk->begin, currentChunk};
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
typename PooledAllocator<T>::Iterator::reference PooledAllocator<T>::Iterator::operator*() const
/// Dereference
{
   return *currentTuple;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
typename PooledAllocator<T>::Iterator::pointer PooledAllocator<T>::Iterator::operator->() const
/// Member access
{
   return currentTuple;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
typename PooledAllocator<T>::Iterator& PooledAllocator<T>::Iterator::operator++()
/// Pre-increment
{
   if (++currentTuple == currentChunk->begin && currentChunk->nextChunk) {
      currentTuple = getFirstAllocation(currentChunk->nextChunk);
      currentChunk = currentChunk->nextChunk;
   }
   return *this;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
typename PooledAllocator<T>::Iterator PooledAllocator<T>::Iterator::operator++(int)
/// Post-increment
{
   Iterator it{*this};
   operator++();
   return it;
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
bool PooledAllocator<T>::Iterator::operator==(const PooledAllocator<T>::Iterator& other) const
/// Equality
{
   return currentTuple == other.currentTuple and currentChunk == other.currentChunk;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::pool
// ---------------------------------------------------------------------------------------------------
#endif