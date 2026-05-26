#include "factDB/infra/threading/ThreadLocal.hpp"
// #include "infra/parallel/LocalState.hpp"
//---------------------------------------------------------------------------
// Umbra
// (c) 2022 Michael Freitag
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace factDB {
//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
/// Slot info for a free slot
struct SlotInfo {
   /// The index of the slot
   uint64_t index = 0;
   /// The highest version this slot had in the past
   uint64_t highestVersion = 0;
};
/// Mutex to guard the free slots
mutex freeSlotsMutex;
/// Thread-local list of freed slots
std::vector<SlotInfo> freeSlots;
/// Index of the next slot to allocate
atomic<uint64_t> nextSlot{0};
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
__thread LocalState* ThreadLocalBase::currentLocalState = nullptr;
//---------------------------------------------------------------------------
LocalState::LocalState() noexcept
// Constructor
{
   assert(!ThreadLocalBase::currentLocalState);
   ThreadLocalBase::currentLocalState = this;
}
//---------------------------------------------------------------------------
LocalState::~LocalState() noexcept
// Destructor
{
   // Process all thread-local instances within this thread
   for (auto* instance : instances) {
      if (!instance)
         continue;

      switch (instance->ownership) {
         case LocalInstanceBase::Ownership::Thread: {
            // The thread owns the instance, destroy it
            delete instance;
         } break;
         case LocalInstanceBase::Ownership::Object: {
            // The wrapper owns the instance, only reset if the wrapper is dead, which it marks by setting the parent to nullptr
            if (instance->expired.test() || instance->expired.test_and_set()) {
               // The wrapper is dead, we must free the instance
               delete instance;
            }
         } break;
      }
   }

   instances.clear();

   assert(ThreadLocalBase::currentLocalState == this);
   ThreadLocalBase::currentLocalState = nullptr;
}
//---------------------------------------------------------------------------
ThreadLocalBase::ThreadLocalBase()
// Constructor
{
   unique_lock lock(freeSlotsMutex);

   auto& fs = freeSlots;
   if (!fs.empty()) {
      auto slotInfo = fs.back();
      slot = slotInfo.index;
      version = slotInfo.highestVersion + 1;
      fs.pop_back();
   } else {
      slot = nextSlot.fetch_add(1);
      version = 1;
   }
}
//---------------------------------------------------------------------------
ThreadLocalBase::~ThreadLocalBase()
// Destructor
{
   unique_lock lock(freeSlotsMutex);

   auto& fs = freeSlots;
   fs.push_back({slot, version});
}
//---------------------------------------------------------------------------
void ThreadLocalBase::registerInstance(LocalState* localState, LocalInstanceBase* localInstance)
// Register an instance in the current thread
{
   // Lazily allocate an index entry
   if (slot >= localState->instances.size())
      localState->instances.resize(slot + 1);

   // Cleanup any leftover instances
   if (localState->instances[slot]) {
      assert(localState->instances[slot]->ownership == LocalInstanceBase::Ownership::Thread || localState->instances[slot]->expired.test());
      delete localState->instances[slot];
   }

   // Register the object in the thread-local index
   localState->instances[slot] = localInstance;
}
//---------------------------------------------------------------------------
OwningThreadLocalBase::~OwningThreadLocalBase() noexcept
// Destructor
{
   // Destroy all thread-local instances
   auto* current = head.load(std::memory_order::acquire);
   while (current) {
      auto* next = current->next;

      // Reset the thread-local pointer if the corresponding thread has exited as well.
      if (current->expired.test() || current->expired.test_and_set()) {
         // The corresponding thread is dead, we must free the instance
         delete current;
      }

      current = next;
   }
}
//---------------------------------------------------------------------------
void OwningThreadLocalBase::registerInstance(LocalState* localState, OwningLocalInstance* localInstance)
// Create an instance
{
   ThreadLocalBase::registerInstance(localState, localInstance);

   // Reset the expired flag
   localInstance->expired.clear();

   // And push the object into the chain
   auto* currentHead = head.load(std::memory_order::acquire);
   do {
      localInstance->next = currentHead;
   } while (!head.compare_exchange_weak(currentHead, localInstance, std::memory_order::acq_rel));
}
//---------------------------------------------------------------------------
void OwningThreadLocalBase::eraseOrphansImpl(std::function<bool(OwningLocalInstance&)>&& predicate) noexcept
// Erases entries for which the associated thread has terminated
{
restart:
   auto* current = head.load(std::memory_order::acquire);
   if (!current)
      return;

   // When erasing the head of the chain we have to guard against concurrent allocations
   if (current->expired.test(std::memory_order::acquire) && predicate(*current)) {
      auto* next = current->next;
      if (head.compare_exchange_weak(current, next, std::memory_order::acq_rel))
         delete current;

      // Restart unconditionally, in case the new head should also be erased
      goto restart;
   }

   // The remainder of the chain can be processed unsynchronized
   auto* next = current->next;
   while (next) {
      if (next->expired.test(std::memory_order::acquire) && predicate(*next)) {
         auto* toDelete = next;
         current->next = next->next;
         next = next->next;
         delete toDelete;
      } else {
         current = next;
         next = next->next;
      }
   }
}
//---------------------------------------------------------------------------
void OwningThreadLocalBase::eraseOrphans() noexcept
// Erases entries for which the associated thread has terminated
{
   eraseOrphansImpl([](auto&) noexcept { return true; });
}
//---------------------------------------------------------------------------
void OwningThreadLocalBase::eraseOrphans(std::function<bool(OwningLocalInstance&)> callback) noexcept
// Erase entries for which the associated thread has terminated given the predicate holds
{
   eraseOrphansImpl(std::move(callback));
}
//---------------------------------------------------------------------------
} // namespace factDB
//---------------------------------------------------------------------------
