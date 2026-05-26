#pragma once
//---------------------------------------------------------------------------
#include "factDB/infra/threading/LocalState.hpp"
#include "factDB/infra/threading/RawObjectStorage.hpp"
#include <atomic>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <vector>
//---------------------------------------------------------------------------
// Umbra
// (c) 2022 Michael Freitag
//---------------------------------------------------------------------------
namespace factDB {
//---------------------------------------------------------------------------
/// Base class for thread-local objects
class ThreadLocalBase {
   protected:
   /// The current thread-local state (if any)
   static __thread LocalState* currentLocalState;

   friend class LocalState;

   /// The slot of the current object
   uint64_t slot;
   /// The version
   uint64_t version;

   /// Register an instance in the current thread
   void registerInstance(LocalState* localState, LocalInstanceBase* localInstance);

   /// Swap
   void swap(ThreadLocalBase& other) noexcept {
      std::swap(slot, other.slot);
      std::swap(version, other.version);
   }

   public:
   /// Constructor
   ThreadLocalBase();
   /// Destructor
   ~ThreadLocalBase();

   /// Copy constructor
   ThreadLocalBase(const ThreadLocalBase&) = delete;
   /// Move constructor
   ThreadLocalBase(ThreadLocalBase&&) = delete;
   /// Copy assignment
   void operator=(const ThreadLocalBase&) = delete;
   /// Move assignment
   void operator=(ThreadLocalBase&&) = delete;

   /// Check whether a thread-local instance exists in the current thread
   bool hasStateInCurrentThread(LocalState* localState) const noexcept {
      assert(localState);
      return (slot < localState->instances.size()) && localState->instances[slot] && (localState->instances[slot]->version == version);
   }
   /// Check whether a thread-local instance exists in the current thread, the version for tests
   bool hasStateInCurrentThread() const noexcept { return hasStateInCurrentThread(currentLocalState); }
};
//---------------------------------------------------------------------------
/// Thread-local object with thread ownership semantics
template <typename T>
class ThreadLocal : public ThreadLocalBase {
   /// A thread-local instance
   struct LocalInstance : public LocalInstanceBase {
      /// The actual value
      T value{};

      /// Constructor
      explicit LocalInstance(uint64_t version_) : LocalInstanceBase(Ownership::Thread, version_) {}
      /// Destructor
      ~LocalInstance() override {}
   };

   public:
   /// Constructor
   ThreadLocal() noexcept = default;
   /// Destructor
   ~ThreadLocal() noexcept = default;

   /// Dereference, creating a thread-local instance if none exists
   T& operator*() noexcept {
      auto localState = currentLocalState;
      assert(localState);
      if (!hasStateInCurrentThread(localState)) [[unlikely]]
         registerInstance(localState, new LocalInstance(version));

      return static_cast<LocalInstance*>(localState->instances[slot])->value;
   }

   T& local() noexcept { return operator*(); }

   /// Swap
   void swap(ThreadLocal& other) noexcept {
      ThreadLocalBase::swap(other);
   }
};
//---------------------------------------------------------------------------
/// Thread-local object with object ownership semantics
class OwningThreadLocalBase : public ThreadLocalBase {
   protected:
   /// A thread-local instance
   struct OwningLocalInstance : public LocalInstanceBase {
      /// The next value in the instance chain
      OwningLocalInstance* next = nullptr;

      /// Constructor
      explicit OwningLocalInstance(uint64_t version_) : LocalInstanceBase(Ownership::Object, version_) {}
   };

   /// The head of the instance chain
   std::atomic<OwningLocalInstance*> head = nullptr;

   /// Register an instance in the current thread
   void registerInstance(LocalState* localState, OwningLocalInstance* instance);

   /// Erase entries for which the associated thread has terminated given the predicate holds
   void eraseOrphansImpl(std::function<bool(OwningLocalInstance&)>&& predicate) noexcept;
   /// Erase entries for which the associated thread has terminated
   void eraseOrphans() noexcept;
   /// Erase entries for which the associated thread has terminated given the predicate holds
   void eraseOrphans(std::function<bool(OwningLocalInstance& localInstance)> predicate) noexcept;

   public:
   /// Constructor
   OwningThreadLocalBase() noexcept = default;
   /// Destructor
   ~OwningThreadLocalBase() noexcept;
};
//---------------------------------------------------------------------------
/// Thread-local object with object ownership semantics
template <typename T>
class OwningThreadLocal : public OwningThreadLocalBase {
   /// A thread-local instance
   struct LocalInstance : public OwningLocalInstance {
      /// The actual value
      RawObjectStorage<T> value;

      /// Constructor
      explicit LocalInstance(uint64_t version_) noexcept : OwningLocalInstance(version_) {}
      /// Destructor
      ~LocalInstance() override { std::destroy_at(value.get()); }

      T& get() noexcept { return *value; }
   };

   public:
   using value_type = T;
   using reference = T&;

   /// Constructor
   OwningThreadLocal() noexcept = default;
   /// Destructor
   ~OwningThreadLocal() noexcept = default;

   /// Dereference, creating a thread-local instance if none exists
   template <typename Init>
   T& access(Init&& init) {
      auto* localState = currentLocalState;
      assert(localState);
      if (!hasStateInCurrentThread(localState)) [[unlikely]] {
         auto* instance = new LocalInstance(version);
         init(instance->value.get()); // Initialize the value before registering it
         OwningThreadLocalBase::registerInstance(localState, instance);
      }

      return static_cast<LocalInstance*>(localState->instances[slot])->get();
   }
   /// Dereference, without creating a thread-local
   T& access() noexcept {
      auto* localState = currentLocalState;
      assert(localState && hasStateInCurrentThread(localState));
      return static_cast<LocalInstance*>(localState->instances[slot])->get();
   }
   /// Dereference, creating a thread-local instance if none exists
   T& operator*() noexcept
      requires std::is_default_constructible_v<T>
   {
      return access([](void* ptr) noexcept { return new (ptr) T{}; });
   }

   T& local() noexcept { return operator*(); }

   /// Execute a callback for each thread-local state. Thread-safe with respect to concurrent allocations of
   /// thread-local state (although thread-local state that is being allocated may not show up in the iteration).
   /// Not thread-safe with respect to concurrent eraseOrphans() calls.
   template <typename Callback>
   void doForEachThreadState(Callback callback);
   /// Execute a callback for each thread-local state. Thread-safe with respect to concurrent allocations of
   /// thread-local state (although thread-local state that is being allocated may not show up in the iteration).
   /// Not thread-safe with respect to concurrent eraseOrphans() calls.
   template <typename Callback>
   void doForEachThreadState(Callback callback) const;

   /// Erases entries for which the associated thread has terminated. Thread-safe with respect to concurrent
   /// allocations of thread-local state. Not thread-safe with respect to concurrent doForEachThreadState() calls.
   void eraseOrphans() noexcept { return OwningThreadLocalBase::eraseOrphans(); }
   /// Erase entries for which the associated thread has terminated given the predicate holds. Thread-safe with
   /// respect to concurrent allocations of thread-local state. Not thread-safe with respect to concurrent
   /// doForEachThreadState() calls.
   template <typename Predicate>
   void eraseOrphans(Predicate&& predicate) {
      OwningThreadLocalBase::eraseOrphans([&](OwningLocalInstance& localInstance) noexcept -> bool {
         return predicate(static_cast<LocalInstance&>(localInstance).get());
      });
   }

   template <bool isConst>
   class Iterator {
      using ref_type = std::conditional_t<isConst, const value_type&, value_type&>;
      using ptr_type = std::conditional_t<isConst, const value_type*, value_type*>;
      using mem_type = std::conditional_t<isConst, const OwningLocalInstance*, OwningLocalInstance*>;
      using LocalInstanceType = std::conditional_t<isConst, const LocalInstance, LocalInstance>;
      using LocalInstancePtrType = std::conditional_t<isConst, const LocalInstance*, LocalInstance*>;

      mem_type cur = nullptr;

      public:
      Iterator(mem_type c) : cur(c) {}
      Iterator() = default;

      Iterator& operator++() {
         cur = cur->next;
         return *this;
      }
      Iterator operator++(int) {
         auto iter = cur;
         cur = cur->next;
         return iter;
      }

      [[nodiscard]] bool operator==(const Iterator& other) { return other.cur == cur; }
      [[nodiscard]] bool operator!=(const Iterator& other) { return other.cur != cur; }

      [[nodiscard]] ref_type operator*() { return *static_cast<LocalInstancePtrType>(cur)->value.get(); }
      [[nodiscard]] ptr_type operator->() const { return static_cast<LocalInstancePtrType>(cur)->value.get(); }
      [[nodiscard]] bool isLast() const { return cur == nullptr; }
      void setEnd() { cur = nullptr; }

      bool operator==(const Iterator& other) const { return other.cur == cur; }
      bool operator!=(const Iterator& other) const { return other.cur != cur; }
   };

   using iterator = Iterator<false>;
   using const_iterator = Iterator<true>;

   iterator begin() { return {head.load(std::memory_order::acquire)}; }
   iterator end() { return {nullptr}; }

   const_iterator begin() const { return const_iterator(head.load(std::memory_order::acquire)); }
   const_iterator end() const { return {nullptr}; }
};
//---------------------------------------------------------------------------
template <typename T>
template <typename Callback>
void OwningThreadLocal<T>::doForEachThreadState(Callback callback)
// Execute a callback for each thread-local state
{
   auto* current = head.load(std::memory_order::acquire);
   while (current) {
      if (!callback(static_cast<LocalInstance*>(current)->get()))
         break;

      current = current->next;
   }
}
//---------------------------------------------------------------------------
template <typename T>
template <typename Callback>
void OwningThreadLocal<T>::doForEachThreadState(Callback callback) const
// Execute a callback for each thread-local state
{
   auto* current = head.load(std::memory_order::acquire);
   while (current) {
      if (!callback(static_cast<LocalInstance*>(current)->get()))
         break;

      current = current->next;
   }
}
//---------------------------------------------------------------------------
} // namespace factDB
//---------------------------------------------------------------------------
