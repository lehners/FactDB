#ifndef H_FACTDB_CHAININGHASHTABLE_HPP
#define H_FACTDB_CHAININGHASHTABLE_HPP

#include "factDB/infra/MultiVectorView.hpp"
#include "factDB/infra/threading/ThreadLocal.hpp"
#include "factDB/infra/threading/ThreadPool.hpp"
#include "factDB/infra/util/IntegerOperations.hpp"
#include "factDB/util/Perfetto.hpp"
#include "tbb/enumerable_thread_specific.h"
#include "tbb/parallel_for_each.h"
#include "tbb/scalable_allocator.h"
#include <atomic>
#include <cstdint>
#include <deque>
#include <iostream>
#include <ostream>
#include <vector>
// ---------------------------------------------------------------------------------------------------
#define USE_POOL 0
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
template <typename K, typename V>
class ChainingHashTable {
   public:
   struct Entry {
      private:
      uint64_t hash = 0;
      Entry* next = nullptr;

      public:
      K key;
      V val;
      friend class ChainingHashTable;

      Entry(uint64_t h, K&& k, V&& v) : hash(h), key(std::move(k)), val(std::move(v)) {}
   };

   // at the beginning, the table is empty. The index is created after calling finalize.
   private:
   // using EntryContainer = infra::list::FastDeque<Entry>;
   // using EntryContainer = std::deque<Entry, tbb::scalable_allocator<Entry>>;
   using EntryContainer = std::vector<Entry, tbb::scalable_allocator<Entry>>;
#if USE_POOL
   factDB::OwningThreadLocal<EntryContainer> entries;
#else
   tbb::enumerable_thread_specific<EntryContainer> entries;
#endif
   Entry** hashtable = nullptr;
   uint32_t hashTableShift;
   size_t sizeVal = 0;

   size_t chunkSize() const { return 1ull << (64 - hashTableShift); }

   public:
   ChainingHashTable() = default;
   ~ChainingHashTable() noexcept;
   ChainingHashTable(ChainingHashTable&) = delete;
   ChainingHashTable(ChainingHashTable&&) noexcept;

   ChainingHashTable& operator=(const ChainingHashTable&) = delete;
   ChainingHashTable& operator=(ChainingHashTable&&) noexcept = default;

   EntryContainer& localEntries() { return entries.local(); }

   // insert an element into the hashtable, only collects the element, the index is built afterward with finalize (see below).
   void insert(uint64_t hash, K key, V&& val) noexcept;
   void insert(uint64_t hash, K key, V&& val, EntryContainer& local) noexcept;
   [[nodiscard]] bool couldContain(uint64_t hash) const noexcept;
   [[nodiscard]] bool contains(uint64_t hash) const noexcept;
   [[nodiscard]] const Entry* lookup(uint64_t hash) const noexcept;
   [[nodiscard]] const Entry* lookupNext(const Entry* ptr) const noexcept;
   [[nodiscard]] size_t size() const { return sizeVal; };

   [[nodiscard]] const Entry* lookup(uint64_t hash, std::tuple<size_t, size_t, size_t, size_t>& stats) const noexcept;
   void genStatistics(std::ostream& ostream) const noexcept;

   // build the hashtable
   template <bool parallel>
   void finalize();
   void finalizeParallel() { return finalize<true>(); };
   void finalizeSingleThreaded() { return finalize<false>(); };

   static std::string getEntryStructName();
};
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
inline uint32_t getHashTableShift(uint64_t elementCount)
// compute the shift for a given size
{
   const uint64_t absolute_min_size = 16;
   uint64_t minSize = std::max(elementCount + (elementCount / 8), absolute_min_size);
   unsigned minSizeLog2 = 64 - IntegerOperations<uint64_t>::clz(minSize);
   minSizeLog2 += ((1ull << minSizeLog2) < minSize);

   return minSizeLog2;
}
// ---------------------------------------------------------------------------------------------------
static inline bool mightHaveHit(const void* entry, [[maybe_unused]] uint64_t hash) noexcept
// Do we potentially have a hit in the list?
{
   return (!!entry);
}
// ---------------------------------------------------------------------------------------------------
static inline uint64_t computeSlot(uint64_t hash, uint64_t hashTableShift) noexcept
// Compute the hash table slot
{
   return (hash >> hashTableShift);
}
// ---------------------------------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------------------------------
template <typename K, typename V>
ChainingHashTable<K, V>::~ChainingHashTable() noexcept {
   if (!!hashtable) {
      free(hashtable);
      hashtable = nullptr;
   }
}
// ---------------------------------------------------------------------------------------------------
template <typename K, typename V>
ChainingHashTable<K, V>::ChainingHashTable(ChainingHashTable&& other) noexcept
   : entries(std::move(other.entries)), hashtable(other.hashtable), hashTableShift(other.hashTableShift), sizeVal(other.sizeVal) {
   other.hashtable = nullptr;
}

template <typename K, typename V>
void ChainingHashTable<K, V>::insert(uint64_t hash, K key, V&& val) noexcept {
   auto& localEntries = entries.local();
   localEntries.emplace_back(hash, std::move(key), std::move(val));
   ++std::atomic_ref(sizeVal);
}

template <typename K, typename V>
void ChainingHashTable<K, V>::insert(uint64_t hash, K key, V&& val, EntryContainer& localEntries) noexcept {
   localEntries.emplace_back(hash, std::move(key), std::move(val));
}

template <typename K, typename V>
template <bool parallel>
void ChainingHashTable<K, V>::finalize() {
   sizeVal = 0;
#if USE_POOL
   MultiVectorView<Entry> mvv;
   entries.doForEachThreadState([&](EntryContainer& localEntries) {
      sizeVal += localEntries.size();
      mvv.insert(localEntries);
      return true;
   });
#else
   MultiVectorView<Entry> mvv;
   for (auto& tls : entries) {
      sizeVal += tls.size();
      mvv.insert(tls);
   }
#endif
   hashTableShift = 64 - getHashTableShift(size());
   hashtable = reinterpret_cast<Entry**>(calloc(1ull << (64 - hashTableShift), sizeof(Entry*)));

   Entry** hashtablePtr = hashtable;
   uint32_t hashTableShiftLocal = hashTableShift;

   auto processEntry = [hashtablePtr, hashTableShiftLocal](auto& entryIter, size_t& idx, size_t limit) {
      auto slot = computeSlot(entryIter->hash, hashTableShiftLocal);
      // assert(slot < chunkSize());

      assert(entryIter->next == nullptr);
      auto last = entryIter;
      const auto hash = entryIter->hash;
      while (idx + 1 != limit) {
         auto next = last;
         ++next;
         if (next->hash != hash) {
            __builtin_prefetch(hashtablePtr + computeSlot(next->hash, hashTableShiftLocal)); // prefetch slot for next step
            break;
         }
         last->next = &(*next);
         last = next;
         ++idx;
      }

      auto atomicSlot = std::atomic_ref(hashtablePtr[slot]);
      last->next = atomicSlot.exchange(&*entryIter);
      entryIter = last;
   };

   if constexpr (parallel) {
#if USE_POOL
      ThreadPool::getPool().parallelFor(
         BlockedRange(0, sizeVal, 500), [&](BlockedRange range) {
            size_t idx = range.begin(), limit = range.end();
            for (auto elem = mvv.iterator_at(idx); idx != limit; ++idx, ++elem) {
               processEntry(elem, idx, limit);
            }
         });
#else
      tbb::parallel_for(tbb::blocked_range<size_t>(0, sizeVal, 500), [&](auto& range) {
         size_t idx = range.begin(), limit = range.end();
         for (auto elem = mvv.iterator_at(idx); idx != limit; ++idx, ++elem) {
            processEntry(elem, idx, limit);
         }
      });
#endif
   } else {
      for (EntryContainer& entry : entries) {
         size_t idx = 0, limit = entry.size();
         for (auto elem = entry.begin(); idx != limit; ++idx, ++elem) {
            processEntry(elem, idx, limit);
         }
      }
   }
}
// ---------------------------------------------------------------------------------------------------
template <typename K, typename V>
bool ChainingHashTable<K, V>::couldContain(uint64_t hash) const noexcept {
   auto slot = computeSlot(hash, hashTableShift);
   assert(slot < chunkSize());
   return mightHaveHit(hashtable[slot], hash);
}
// ---------------------------------------------------------------------------------------------------
template <typename K, typename V>
bool ChainingHashTable<K, V>::contains(uint64_t hash) const noexcept {
   return (!!lookup(hash));
}
// ---------------------------------------------------------------------------------------------------
template <typename K, typename V>
const typename ChainingHashTable<K, V>::Entry* ChainingHashTable<K, V>::lookup(uint64_t hash) const noexcept {
   auto slot = computeSlot(hash, hashTableShift);
   assert(slot < chunkSize());
   auto current = hashtable[slot];
   if (mightHaveHit(current, hash)) {
      do {
         if (current->hash == hash)
            return current;
         current = current->next;
      } while (current);
   }
   return nullptr;
}

#if 0
template <typename K, typename V>
const util::TypedTaggedPointer<typename ChainingHashTable<K, V>::Entry> ChainingHashTable<K, V>::lookupTagged(uint64_t hash) const noexcept {
   auto slot = computeSlot(hash, hashTableShift);
   assert(slot < chunkSize());
   auto current = util::TypedTaggedPointer<ChainingHashTable::Entry>::fromRawPtr(hashtable[slot]);
   if (mightHaveHit(current.getPointer(), hash)) {
      auto* curPtr = current.getPointer();
      auto curSize = current.getTag();
      do {
         if (curPtr->hash == hash)
            return util::TypedTaggedPointer<Entry>(curPtr, curSize);
         curPtr = curPtr->next;
         --curSize;
      } while (curPtr);
   }
   return util::TypedTaggedPointer<Entry>(nullptr, 0);
}
#endif

template <typename K, typename V>
const typename ChainingHashTable<K, V>::Entry* ChainingHashTable<K, V>::lookup(uint64_t hash, std::tuple<size_t, size_t, size_t, size_t>& stats) const noexcept {
   auto slot = computeSlot(hash, hashTableShift);
   assert(slot < chunkSize());
   auto current = hashtable[slot];
   size_t chain_length = 0;
   if (mightHaveHit(current, hash)) {
      do {
         chain_length++;
         if (current->hash == hash) {
            std::atomic_ref(std::get<0>(stats)) += chain_length;
            std::atomic_ref(std::get<2>(stats))++;
            return current;
         }
         current = current->next;
      } while (current);
   }
   std::atomic_ref(std::get<1>(stats)) += chain_length;
   std::atomic_ref(std::get<3>(stats))++;
   return nullptr;
}

template <typename K, typename V>
const typename ChainingHashTable<K, V>::Entry* ChainingHashTable<K, V>::lookupNext(const ChainingHashTable<K, V>::Entry* current) const noexcept {
   assert(current);
   auto hash = current->hash;
   auto firstKey = current->key;
   current = current->next;
   while (current) {
      if (current->hash == hash && current->key == firstKey)
         return current;
      current = current->next;
   }
   return nullptr;
}

template <typename K, typename V>
void ChainingHashTable<K, V>::genStatistics(std::ostream& ostream) const noexcept {
   if (hashtable == nullptr) {
      ostream << "No hashtable built so far." << std::endl;
      return;
   }
   size_t min_chain = std::numeric_limits<size_t>::max(), max_chain = 0, total_chain_length = 0, count_not_empty = 0, longChains = 0;
   std::vector<size_t> historgram;
   historgram.resize(300, 0);
   for (Entry** c = hashtable; c != hashtable + chunkSize(); ++c) {
      size_t cur_chain = 0;
      Entry* cur_entry = *c;
      while (!!cur_entry) {
         cur_chain++;

         if (cur_chain == 100) {
            longChains++;
            //constexpr size_t idx = 1;
            //ostream << "long chain: " << cur_chain << /*"\t|" << std::get<idx>(cur_entry->val) << "\t" << std::get<idx>(c->val) <<*/ std::endl;
            /*ostream << Hash::hashTuple(std::make_tuple(std::get<idx>(cur_entry->val).value)) << "\t"
                    << Hash::hashTuple(std::make_tuple(std::get<idx>(c->val).value)) << std::endl;*/
         }
         cur_entry = cur_entry->next;
      }

      if (cur_chain < historgram.size())
         ++historgram.back();

      min_chain = std::min(min_chain, cur_chain);
      max_chain = std::max(max_chain, cur_chain);
      total_chain_length += cur_chain;
      count_not_empty += cur_chain ? 1 : 0;
   }
#if 0
   std::cout << std::endl;
   for (auto& i : historgram)
      std::cout << i << ",";
   std::cout << std::endl;
#endif

   assert(total_chain_length == sizeVal);
   ostream << "htStats: [chunks: " << chunkSize()
           << ", min: " << min_chain
           << ", max: " << max_chain
           << ", total: " << total_chain_length
           << ", avg: " << total_chain_length * 1. / chunkSize() // NOLINT
           << ", non-empty: " << count_not_empty
           << ", avg n-e: " << total_chain_length * 1. / count_not_empty // NOLINT
           << ", long-chains: " << longChains << "]"
           << std::endl;
}

} // namespace factDB

#undef USE_POOL
#endif // H_FACTDB_CHAININGHASHTABLE_HPP
