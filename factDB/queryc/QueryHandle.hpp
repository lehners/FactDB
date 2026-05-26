#ifndef H_FACTDB_FACTDB_QUERYC_QUERYHANDLE_HPP
#define H_FACTDB_FACTDB_QUERYC_QUERYHANDLE_HPP
// ---------------------------------------------------------------------------------------------------
#include <string>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class Database;
// ---------------------------------------------------------------------------------------------------
namespace queryc {
// ---------------------------------------------------------------------------------------------------
class PerformanceRecorderExecution;
// ---------------------------------------------------------------------------------------------------
class RawHandle {
   private:
   void* handle = nullptr;
   std::string path = {};

   public:
   RawHandle() noexcept = default;
   RawHandle(const std::string& pathP, bool global = true) noexcept;
   ~RawHandle();

   RawHandle(const RawHandle&) = delete;
   RawHandle& operator=(const RawHandle&) = delete;
   RawHandle(RawHandle&& other) noexcept;
   RawHandle& operator=(RawHandle&& other) noexcept;

   template <typename functor_type>
   functor_type loadFunction(const std::string& functionName) { return reinterpret_cast<functor_type>(loadFunctionRaw(functionName)); }

   auto loadQueryHandle() { return loadFunction<void (*)(factDB::Database&, std::ostream&, queryc::PerformanceRecorderExecution)>("query"); }
   auto loadIteratorHandle() { return loadFunction<void (*)(void*)>("iterate"); }

   private:
   [[nodiscard]] void* loadFunctionRaw(const std::string& functionName) const;
};
// ---------------------------------------------------------------------------------------------------
} // namespace queryc
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_QUERYC_QUERYHANDLE_HPP
