#ifndef H_FACTDB_FACTDB_OPT_ORACLE_BASEORACLE_HPP
#define H_FACTDB_FACTDB_OPT_ORACLE_BASEORACLE_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include "factDB/opt/algs/OptimizerAlgorithms.hpp"
#include <memory>
#include <string>
#include <unordered_map>
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB { class Database; };
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra { class Operator; };
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
struct sqlite3;
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class Plan;
class BaseTablePlan;
class JoinPlan;
class QueryGraph;
// ---------------------------------------------------------------------------------------------------
class BaseOracle {
   protected:
   std::unordered_map<std::string, size_t> signature2estimate;
   Database& database;

   BaseOracle(Database& db) : database(db){};

   public:
   /// estimates the size of the results, tries to remember previous estimates
   size_t estimate(const Plan* plan, const QueryGraph& qg, const BaseEstimator& estimator);
   size_t estimate(const BaseTablePlan& plan, const QueryGraph& qg, const BaseEstimator& estimator) { return calcEstimate(plan, qg, estimator); }
   size_t estimate(const JoinPlan& plan, const QueryGraph& qg, const BaseEstimator& estimator) { return calcEstimate(plan, qg, estimator); }

   /// generates the signature for a query.
   std::string generateSignature(std::unique_ptr<algebra::Operator>& op);

   virtual std::string lookupCachedPlan(OptimizerAlgorithm::Algorithm) { return ""; }
   virtual void insertCachedPlan(const Plan*, OptimizerAlgorithm::Algorithm, const QueryGraph&) {}

   virtual size_t calcEstimate(const BaseTablePlan& plan, const QueryGraph& qg, const BaseEstimator& estimator) = 0;
   virtual size_t calcEstimate(const JoinPlan& plan, const QueryGraph& qg, const BaseEstimator& estimator) = 0;

   Database& getDatabase() { return database; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_OPT_ORACLE_BASEORACLE_HPP