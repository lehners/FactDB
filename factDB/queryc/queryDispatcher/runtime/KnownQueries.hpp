#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/ExecutionMode.hpp"
#include <unordered_set>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::queryc {
// ---------------------------------------------------------------------------------------------------
enum class QueryFlag : uint8_t {
   None = 0,
   Naive = 1 << 0,
   Parallel = 1 << 1,
   Inlined = 1 << 2,
   CacheCountStar = 1 << 3,
   BottomInserts = 1 << 4
};
// ---------------------------------------------------------------------------------------------------
inline QueryFlag operator|(QueryFlag a, QueryFlag b) {
   return static_cast<QueryFlag>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}
// ---------------------------------------------------------------------------------------------------
inline QueryFlag operator&(QueryFlag a, QueryFlag b) {
   return static_cast<QueryFlag>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}
// ---------------------------------------------------------------------------------------------------
inline QueryFlag& operator|=(QueryFlag& a, QueryFlag b) {
   a = a | b;
   return a;
}
// ---------------------------------------------------------------------------------------------------
struct QueryProperties {
   ExecutionMode mode;
   std::string queryname;
   QueryFlag flags;

   QueryProperties(ExecutionMode mode_, std::string queryname_, QueryFlag flags_)
      : mode(mode_), queryname(std::move(queryname_)), flags(flags_) {}

   bool has(QueryFlag f) const { return static_cast<uint8_t>(flags & f) != 0; }
   std::string genModeString() const;
   uint8_t getFlagUInt() const { return static_cast<uint8_t>(flags); }
   auto operator<=>(const QueryProperties&) const = default;

   static QueryProperties fromCurrentSettings();
   static QueryFlag fromModeString(const std::string& str);
};
// ---------------------------------------------------------------------------------------------------
class KnownQueries {
   std::vector<QueryProperties> generatedQueries;

   public:
   KnownQueries();
   ~KnownQueries();

   void addQuery(const QueryProperties& qp);
   void fromQueryFile();
   void sort();
   void eliminateDuplicates();
   void writeQueries() const;

   auto begin() const { return generatedQueries.begin(); }
   auto end() const { return generatedQueries.end(); }
   std::unordered_set<std::string_view> querynames() const;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::queryc
// ---------------------------------------------------------------------------------------------------
