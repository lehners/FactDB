#pragma once
//---------------------------------------------------------------------------
#include "factDB/infra/threading/LocalState.hpp"
#include <atomic>
#include <barrier>
#include <functional>
#include <latch>
#include <thread>
#include <vector>
//---------------------------------------------------------------------------
namespace factDB {
//---------------------------------------------------------------------------
/// A simple blocked range [begin, end) with a grain size
struct BlockedRange {
   size_t m_begin;
   size_t m_end;
   size_t grainSize;

   BlockedRange(size_t begin_, size_t end_, size_t grainSize_ = 500)
      : m_begin(begin_), m_end(end_), grainSize(grainSize_) {}

   bool empty() const { return m_begin >= m_end; }
   size_t size() const { return m_end - m_begin; }

   size_t begin() const { return m_begin; }
   size_t end() const { return m_end; }
};
//---------------------------------------------------------------------------
/// Thread pool with grain-size based dynamic scheduling.
///
/// - Threads are created once and kept alive across parallelFor calls.
/// - Each worker thread owns a LocalState for the duration of its lifetime.
/// - Work is distributed dynamically via an atomic counter in chunks of grainSize,
///   giving the same load-balancing semantics as tbb::parallel_for.
/// - The main thread participates in work as well.
/// - Completion is signaled via std::latch — no busy-waiting.
class ThreadPool {
   //---------------------------------------------------------------------------
   /// Internal task descriptor set before each parallelFor call
   struct Task {
      std::function<void(BlockedRange)>* func = nullptr;
      size_t end = 0;
      size_t grainSize = 1;
   };
   //---------------------------------------------------------------------------
   /// Worker threads
   std::vector<std::thread> workers;

   /// Atomic work cursor — workers fetch_add(grainSize) to claim a chunk
   std::atomic<size_t> workCursor{0};

   /// Current task — written before startBarrier, read after
   Task currentTask{};

   /// Signals workers to start a new task (or shut down)
   /// +1 for the main thread
   std::barrier<> startBarrier;
   std::barrier<> endBarrier;

   /// Shutdown flag — checked after startBarrier
   std::atomic<bool> shutdown{false};
   std::atomic<size_t> running{0};

   //---------------------------------------------------------------------------
   /// Claim and execute chunks until the range is exhausted
   void processWork() {
      ++running;
      const auto* task = &currentTask;
      while (true) {
         size_t begin = workCursor.fetch_add(task->grainSize);
         if (begin >= task->end)
            break;
         size_t end = std::min(begin + task->grainSize, task->end);
         (*task->func)(BlockedRange{begin, end, task->grainSize});
      }
   }
   //---------------------------------------------------------------------------
   /// Worker thread body — owns a LocalState for its entire lifetime
   void workerLoop() {
      // LocalState constructor sets ThreadLocalBase::currentLocalState = this
      // Destructor clears it and frees all thread-local instances
      LocalState localState;

      while (true) {
         startBarrier.arrive_and_wait(); // sleep until next task

         if (shutdown.load(std::memory_order_acquire))
            return;

         processWork();
         endBarrier.arrive_and_wait(); // workers block here until everyone is done
      }
   }

   public:
   //---------------------------------------------------------------------------
   explicit ThreadPool(size_t numWorkers = std::thread::hardware_concurrency() - 1)
      : startBarrier(static_cast<ptrdiff_t>(numWorkers) + 1), endBarrier(static_cast<ptrdiff_t>(numWorkers) + 1) // +1 for main thread
   {
      workers.reserve(numWorkers);
      for (size_t i = 0; i < numWorkers; ++i)
         workers.emplace_back(&ThreadPool::workerLoop, this);
   }

   //---------------------------------------------------------------------------
   ~ThreadPool() {
      shutdown.store(true, std::memory_order_release);
      startBarrier.arrive_and_wait(); // wake workers so they can exit
      for (auto& w : workers)
         w.join();
   }

   static ThreadPool& getPool() {
      static ThreadPool pool;
      return pool;
   }

   //---------------------------------------------------------------------------
   /// Parallel for over [0, n) with the given grain size.
   /// func receives a BlockedRange(begin, end, grainSize) for each chunk.
   /// The main thread participates in work — no CPU is wasted.
   /// Blocks until all chunks are complete.
   void parallelFor(size_t n, size_t grainSize, std::function<void(BlockedRange)> func) {
      if (n == 0)
         return;
      if (grainSize == 0)
         throw std::runtime_error("Illegal grainsize 0!");

      // Publish task before releasing workers
      currentTask = Task{&func, n, grainSize};
      workCursor.store(0, std::memory_order_release);

      startBarrier.arrive_and_wait(); // release workers

      // Main thread participates
      processWork();

      endBarrier.arrive_and_wait();
   }

   //---------------------------------------------------------------------------
   /// Convenience overload: iterate over a range [range.begin, range.end)
   void parallelFor(BlockedRange range, std::function<void(BlockedRange)> func) {
      if (range.empty())
         return;
      // Shift the cursor so chunks start at range.begin
      // by offsetting the end and adjusting work claimed inside func
      parallelFor(range.size(), range.grainSize,
                  [begin = range.begin(), &func](BlockedRange r) {
                     func(BlockedRange{begin + r.begin(), begin + r.end(), r.grainSize});
                  });
   }

   //---------------------------------------------------------------------------
   /// Number of worker threads (excluding the main thread)
   size_t numWorkers() const { return workers.size(); }

   //---------------------------------------------------------------------------
   // Non-copyable, non-movable
   ThreadPool(const ThreadPool&) = delete;
   ThreadPool& operator=(const ThreadPool&) = delete;
};
//---------------------------------------------------------------------------
} // namespace factDB