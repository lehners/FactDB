#pragma once
// ---------------------------------------------------------------------------------------------------
#ifdef HAS_SQLITE3
// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/oracle/BaseOracle.hpp"
#include <deque>
#include <filesystem>
// ---------------------------------------------------------------------------------------------------
struct sqlite3;
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class CardinalityOracleDBCached : public BaseOracle {
   sqlite3* cardinalityDatabase = nullptr;
   std::deque<std::string> changedEstimates;

   CardinalityOracleDBCached(Database& db, bool doRead = true);
   ~CardinalityOracleDBCached();

   size_t calcEstimate(const BaseTablePlan& plan, const QueryGraph& qg, const BaseEstimator& estimator) override;
   size_t calcEstimate(const JoinPlan& plan, const QueryGraph& qg, const BaseEstimator& estimator) override;

   /// saves the estimate file to the default location
   void store();

   private:
   /// reads stored estimates
   // void readEstimateFile(const std::filesystem::path& path);

   void openCardinalityDatabase(const std::string& dbPath);
   /// reads stored estimates from DB
   void readEstimatesFromDB();
   /// writes the new estimates to DB
   void writeEstimatesToDB(bool verbose = false);

   public:
   static void importFileIntoDB();
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
#endif
// ---------------------------------------------------------------------------------------------------