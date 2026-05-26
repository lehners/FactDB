#ifndef H_infra_parallel_LocalState
#define H_infra_parallel_LocalState
//---------------------------------------------------------------------------
#include <atomic>
#include <vector>
//---------------------------------------------------------------------------
// Umbra
// (c) 2025 Altan Birler
//---------------------------------------------------------------------------
namespace factDB {
//---------------------------------------------------------------------------
class LocalState;
//---------------------------------------------------------------------------
/// Base class for all thread-local object instances
struct LocalInstanceBase {
   /// Possible ownership semantics
   enum class Ownership {
      /// The thread owns the thread-local instance, i.e. it is destroyed when the thread terminates
      Thread,
      /// The thread-local wrapper owns the thread-local instance, i.e. it is destroyed when the wrapper
      /// is destroyed, or when it is explicitly garbage-collected.
      Object
   };

   /// The ownership semantics for the instance
   Ownership ownership;
   /// The version
   uint64_t version;
   /// Did the thread-local instance expire (either the thread terminated or the owning thread-local object was destructed)?
   std::atomic_flag expired;

   /// Constructor
   LocalInstanceBase(Ownership ownership_, uint64_t version_) noexcept : ownership(ownership_), version(version_) {};
   /// Destructor
   virtual ~LocalInstanceBase() noexcept = default;
};
//---------------------------------------------------------------------------
/// Local state maintained within a thread
class LocalState {
   /// Pointers to the thread-local instances in this thread
   std::vector<LocalInstanceBase*> instances;

   friend class ThreadLocalBase;
   template <typename T>
   friend class ThreadLocal;
   friend class OwningThreadLocalBase;
   template <typename T>
   friend class OwningThreadLocal;

   public:
   /// Constructor
   LocalState() noexcept;
   /// Destructor
   ~LocalState() noexcept;

   /// Reset
   void reset() noexcept;
};
//---------------------------------------------------------------------------
} // namespace factDB
//---------------------------------------------------------------------------
#endif
