#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include "factDB/queryc/ExecutionMode.hpp"
#include "factDB/queryc/PerformanceRecord.hpp"
// ---------------------------------------------------------------------------------------------------
#define QUERY_IMPLEMENTATION(CS, NS)                                          \
   namespace CS::NS {                                                         \
   std::string getSchemaIdentifier();                                         \
   std::vector<std::string> getRequiredRelations();                           \
   void query(factDB::Database& database, std::ostream& out_stream,           \
              queryc::PerformanceRecorderExecution pre);                      \
   }                                                                          \
                                                                              \
   template <>                                                                \
   struct QuerySelector<queryc::CS, NS> {                                     \
      static std::string getSchemaIdentifier() {                              \
         return CS::NS::getSchemaIdentifier();                                \
      }                                                                       \
      static std::vector<std::string> getRequiredRelations() {                \
         return CS::NS::getRequiredRelations();                               \
      }                                                                       \
      static void query(factDB::Database& database, std::ostream& out_stream, \
                        queryc::PerformanceRecorderExecution pre) {           \
         return CS::NS::query(database, out_stream, pre);                     \
      }                                                                       \
      static std::string_view name() {                                        \
         return #NS;                                                          \
      }                                                                       \
   };
// ---------------------------------------------------------------------------------------------------