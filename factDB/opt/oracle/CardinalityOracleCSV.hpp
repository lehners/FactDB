#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/oracle/BaseOracle.hpp"
#include <deque>
#include <filesystem>
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class CardinalityOracleCSV : public BaseOracle {
   friend class CardinalityOracleDBCached;

   std::deque<std::string> changedEstimates;

   public:
   CardinalityOracleCSV(factDB::Database& db);
   ~CardinalityOracleCSV() { store(); }

   size_t calcEstimate(const BaseTablePlan& plan, const QueryGraph& qg, const BaseEstimator& estimator) override;
   size_t calcEstimate(const JoinPlan& plan, const QueryGraph& qg, const BaseEstimator& estimator) override;

   /// saves the estimate file to the default location
   void store();

   private:
   /// clears the cached estimates
   void clearEstimates() { signature2estimate.clear(); }
   /// reads stored estimates
   void readEstimateFile(const std::filesystem::path& path);
   /// stores the current estimates to the given file
   void writeEstimateFile(const std::filesystem::path& path);
   /// writes the estimates to a stream
   void writeEstimatesToStream(std::ostream& stream);
   /// reads the estimates from a stream
   void readEstimatesFromStream(std::istream& stream);
   /// creates a backup of the current estimates (overrides backups if too many exist)
   static void createBackup();
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------