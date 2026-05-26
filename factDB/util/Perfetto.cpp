#include "factDB/util/Perfetto.hpp"
#include "factDB/infra/alloc/Mmap.hpp"
#include <atomic>
#include <cassert>
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>
//---------------------------------------------------------------------------
#ifdef PERFETTO
//---------------------------------------------------------------------------
namespace factDB {
/// Bump allocator for allocating query memory
struct PerfettoMemory {
   /// MMapped memory region
   Mmap memory;
   /// The initial number of pages
   size_t byteCount = 0;
   /// The used number of pages
   std::atomic<size_t> bytes = 0;

   /// Constructor
   PerfettoMemory() {
      byteCount = 1024 * 1024 * 1024;
      memory = Mmap::mapMemory(byteCount);
      Mmap::prefault(memory.data(), byteCount);
   }
   /// Perform an allocation
   void* allocate(size_t count) {
      // memory order relaxed is sufficient as we do not care about ordering with
      // respect to other atomic operations at all...
      count = (count + 15) & ~15ull;
      size_t idx = bytes.fetch_add(count, std::memory_order_relaxed);

      assert(idx + count <= byteCount);
      return memory.data() + idx;
   }
};
static PerfettoMemory perfettoMemory;
//---------------------------------------------------------------------------
__thread PerfettoTracer::DurationEventChunk* localEvents;
//---------------------------------------------------------------------------
std::chrono::steady_clock::time_point globalTime = std::chrono::steady_clock::now();
//---------------------------------------------------------------------------
uint32_t now() {
   auto time = std::chrono::steady_clock::now();
   return std::chrono::duration_cast<std::chrono::microseconds>(time - globalTime).count();
}
//---------------------------------------------------------------------------
static PerfettoTracer* getPerfettoTracer() {
   static PerfettoTracer tracer;
   return &tracer;
}
//---------------------------------------------------------------------------
PerfettoTracer::~PerfettoTracer()
// The destructor
{
   dump("perfetto.trace");
}
//---------------------------------------------------------------------------
void PerfettoTracer::registerThread(std::string&& threadName)
// Register a thread
{
   auto tracer = getPerfettoTracer();
   std::unique_lock lock{tracer->mutex};
   localEvents = static_cast<DurationEventChunk*>(std::malloc(sizeof(DurationEventChunk)));
   localEvents->next = nullptr;
   localEvents->index = 0;
   tracer->threadInfo.emplace_back(ThreadInfo{static_cast<size_t>(gettid()), std::move(threadName)});
   tracer->events.push_back(localEvents);
}
//---------------------------------------------------------------------------
static std::once_flag perfettoWarning;
//---------------------------------------------------------------------------
static PerfettoTracer::DurationEvent* getEvent() {
   if (!localEvents) {
      std::call_once(perfettoWarning, [&]() {
         std::cerr << "did you forget to call PerfettoTracer::registerThread()?" << "\n";
         PerfettoTracer::registerThread("main?");
      });
      return nullptr;
   }

   if (localEvents->index == PerfettoTracer::DurationEventChunk::maxEvents) {
      localEvents->next = static_cast<PerfettoTracer::DurationEventChunk*>(perfettoMemory.allocate(sizeof(PerfettoTracer::DurationEventChunk)));
      localEvents = localEvents->next;
      localEvents->next = nullptr;
      localEvents->index = 0;
   }
   return &localEvents->events[localEvents->index++];
}
//---------------------------------------------------------------------------
PerfettoTracer::Trace::Trace(std::string_view name) : event(getEvent())
// The constructor
{
   thread_local bool registered = false;

   if (!registered) {
      registered = true;
      std::string tid = std::format("{}", std::this_thread::get_id());
      PerfettoTracer::registerThread("thread" + tid);
   }

   if (event)
      *event = DurationEvent{name.data(), name.length(), now(), -1u};
}
//---------------------------------------------------------------------------
PerfettoTracer::Trace::Trace(std::string&& name) : event(getEvent())
// The constructor
{
   if (event) {
      auto n = std::move(name);
      auto* data = static_cast<char*>(perfettoMemory.allocate(n.size()));
      n.copy(data, n.size());
      *event = DurationEvent{data, n.size(), now(), -1u};
   }
}
//---------------------------------------------------------------------------
PerfettoTracer::Trace::~Trace()
// The destructor
{
   if (event) {
      event->end = now();
      if (event->end < event->begin || event->end - event->begin <= 1) {
         // Too fast, ignore
         if (localEvents->index != 0)
            localEvents->index--;
      }
   }
}
//---------------------------------------------------------------------------
void PerfettoTracer::dump(std::string_view filename)
// Dump the perfetto trace
{
   std::cout << "Writing trace file ..." << "\n";

   std::ofstream trace;
   trace.open(std::string{filename});

   trace << '[' << '\n';

   auto pid = getpid();

   for (auto& thread : threadInfo) {
      trace << R"({"name":"thread_name","cat":"P","ph":"M","pid":)" << pid << R"(,"tid":)" << thread.tid << R"(,"args":{"name":")" << thread.name << R"("}},)" << '\n';
   }

   for (size_t i = 0; i < events.size(); i++) {
      auto tid = threadInfo[i].tid;
      auto ev = events[i];
      while (ev) {
         for (auto j = 0; j < ev->index; j++) {
            auto event = ev->events[j];
            trace << R"({"name":")" << std::string_view(event.namePtr, event.nameLen) << R"(","cat":"P","ph":"B","pid":)" << pid << R"(,"tid":)" << tid << R"(,"ts":)" << event.begin << R"(},)" << '\n';
            trace << R"({"name":")" << std::string_view(event.namePtr, event.nameLen) << R"(","cat":"P","ph":"E","pid":)" << pid << R"(,"tid":)" << tid << R"(,"ts":)" << event.end << R"(},)" << '\n';
         }
         ev = ev->next;
      }
   }

   // empty value at end of list for valid json
   trace << "{}" << "\n";

   // and close the list
   trace << "]" << "\n";

   trace.close();
}
//---------------------------------------------------------------------------
} // namespace factDB
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
