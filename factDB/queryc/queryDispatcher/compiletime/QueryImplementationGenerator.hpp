#ifndef H_FACTDB_FACT_DB_SCHEMAC_SCHEMACOMPILER_H_
#define H_FACTDB_FACT_DB_SCHEMAC_SCHEMACOMPILER_H_
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/ExecutionMode.hpp"
#include "factDB/queryc/queryDispatcher/runtime/KnownQueries.hpp"
#include "factDB/queryc/writer/FileWriter.hpp"
#include <fstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::queryc {
// ---------------------------------------------------------------------------------------------------
struct QueryProperties;
// ---------------------------------------------------------------------------------------------------
class QueryImplementationGenerator {
   private:
   std::ofstream headerStream, sourceStream, cmakeStream;
   fw::FileWriter header, source, cmake;

   public:
   // Constructor
   QueryImplementationGenerator();
   ~QueryImplementationGenerator();

   void genQueryHandler();
   void genCMake();
   void genDispatcher();

   static const KnownQueries& getKnownQueries();
   static void eliminateDuplicates();

   static void addQuery(const QueryProperties& qp);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::queryc
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACT_DB_SCHEMAC_SCHEMACOMPILER_H_
// ---------------------------------------------------------------------------------------------------
