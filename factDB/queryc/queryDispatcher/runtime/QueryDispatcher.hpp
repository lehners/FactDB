#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/query.hpp"
#include "factDB/queryc/ExecutionMode.hpp"
#include "factDB/queryc/queryDispatcher/runtime/KnownQueries.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::queryc {
// ---------------------------------------------------------------------------------------------------
class QueryDispatcher {
   public:
   struct QueryDescriptor {
      QueryProperties qp;
      generated::QueryFunctionType query;
      generated::RequiredRelationsFunctionType requiredRelations;
      generated::SchemaIdentifierFunctionType schemaIdentifier;
   };

   static void benchmarkQuery(Database& db, const QueryDescriptor& query);
   static void runBenchmarks(queryc::ExecutionMode execution, Database& db);

   static void registerQuery(const QueryProperties& qp, generated::QueryFunctionType query, generated::RequiredRelationsFunctionType requiredRelations, generated::SchemaIdentifierFunctionType schemaIdentifier);
   static void registerQuery(QueryDescriptor descriptor);
   static const std::vector<QueryDescriptor>& getQueryDescriptors();
   static const QueryDescriptor& getQueryDescriptor(const QueryProperties& qp);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::queryc
// ---------------------------------------------------------------------------------------------------
