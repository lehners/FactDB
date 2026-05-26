#pragma once
// ---------------------------------------------------------------------------------------------------
#ifdef HAS_SQLITE3
// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/oracle/BaseOracle.hpp"
// ---------------------------------------------------------------------------------------------------
struct sqlite3;
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class CardinalityOracleDB : public BaseOracle {
   sqlite3* cardinalityDatabase = nullptr;

   public:
   CardinalityOracleDB(Database& db);
   ~CardinalityOracleDB();

   CardinalityOracleDB(const CardinalityOracleDB&) = delete;
   CardinalityOracleDB& operator=(const CardinalityOracleDB&) = delete;

   CardinalityOracleDB(CardinalityOracleDB&& other) noexcept;
   CardinalityOracleDB& operator=(CardinalityOracleDB&& other) noexcept;

   size_t calcEstimate(const BaseTablePlan& plan, const QueryGraph& qg, const BaseEstimator& estimator) override;
   size_t calcEstimate(const JoinPlan& plan, const QueryGraph& qg, const BaseEstimator& estimator) override;
   size_t estimateSignature(const Plan& plan, const QueryGraph& qg, const BaseEstimator& estimator);

   std::string lookupCachedPlan(OptimizerAlgorithm::Algorithm algorithm) override;
   void insertCachedPlan(const Plan* plan, OptimizerAlgorithm::Algorithm algorithm, const QueryGraph& qg) override;

   /// saves the estimate file to the default location
   void store() {}

   private:
   std::pair<bool, size_t> getEstimate(const std::string& signature) const;
   void insertEstimate(const std::string& signature, size_t estimate) const;
   void openCardinalityDatabase(const std::string& dbPath);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
#endif
// ---------------------------------------------------------------------------------------------------