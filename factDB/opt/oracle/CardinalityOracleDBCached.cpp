// ---------------------------------------------------------------------------------------------------
#ifdef HAS_SQLITE3
// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/oracle/CardinalityOracleDBCached.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/types/UInt.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/estimators/DummyEstimator.hpp"
#include "factDB/queryc/QueryParseContext.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "sqlite3.h"
// ---------------------------------------------------------------------------------------------------
using namespace factDB::opt;
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<bool> importDB("opt.oracle.import.db", true);
// ---------------------------------------------------------------------------------------------------
CardinalityOracleDBCached::CardinalityOracleDBCached(factDB::Database& db, bool doRead) : BaseOracle(db) {
   if (doRead)
      openCardinalityDatabase(SettingBase::getSetting<std::string>("opt.oracle.path.db")->get());
}
// ---------------------------------------------------------------------------------------------------
CardinalityOracleDBCached::~CardinalityOracleDBCached() {
   store();
   if (cardinalityDatabase != nullptr)
      sqlite3_close(cardinalityDatabase);
}
// ---------------------------------------------------------------------------------------------------
size_t CardinalityOracleDBCached::calcEstimate(const BaseTablePlan& p, const QueryGraph& qg, const BaseEstimator& estimator) {
   assert(false && "needs update");
   auto operatorPlan = p.generateOperatorPlan(qg, estimator);
   auto signature = generateSignature(operatorPlan);
   if (signature2estimate.contains(signature)) {
      return signature2estimate[signature];
   } else { // base tables
      const auto& relation = qg.getRelation(p.getRelation());
      if (!database.isRelationLoaded(relation.relation))
         database.loadRelation(relation.relation);
      size_t retrievedSize = database.getTable(relation.relation).size();
      signature2estimate[signature] = retrievedSize;
      changedEstimates.emplace_back(signature);
      return retrievedSize;
   }
}
// ---------------------------------------------------------------------------------------------------
size_t CardinalityOracleDBCached::calcEstimate(const JoinPlan& p, const QueryGraph& qg, const BaseEstimator& estimator) {
   auto operatorPlan = p.generateOperatorPlan(qg, estimator);
   std::unique_ptr<algebra::Operator> count = std::make_unique<algebra::Count>(std::move(operatorPlan));
   auto signature = generateSignature(count);
   if (signature2estimate.contains(signature) && signature2estimate[signature] != std::numeric_limits<size_t>::max()) {
      return signature2estimate[signature];
   } else { // joins
      // prepare the query for execution

      auto set = SettingBase::getSetting("codegen.executionMode")->setTemporary(queryc::ExecutionMode::UmbraServer);

      // execute query
      size_t retrievedSize;
      while (true) {
         try {
            factDB::queryc::QueryParseContext context(database);
            std::stringstream resultStream;
            context.compileAndExecute(std::move(count), {}, database, resultStream, false, true);

            // retrieve the result from the string stream

            resultStream >> retrievedSize;
            if (!SettingBase::getSetting<bool>("statistics.silent")->get())
               std::cout << "estimate size for \"" << signature << "\" as " << retrievedSize << std::endl;
            break;
         } catch (std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
            retrievedSize = std::numeric_limits<size_t>::max();
            operatorPlan = p.generateOperatorPlan(qg, estimator);
            count = std::make_unique<algebra::Count>(std::move(operatorPlan));
         }
      }
      // store the result in the hashtable
      signature2estimate[signature] = retrievedSize;
      changedEstimates.emplace_back(signature);
      if (SettingBase::getSetting<bool>("opt.oracle.autosave")->get()) store();
      return retrievedSize;
   }
}
// ---------------------------------------------------------------------------------------------------
void CardinalityOracleDBCached::openCardinalityDatabase(const std::string& dbPath) {
   assert(cardinalityDatabase == nullptr);
   if (sqlite3_open(dbPath.c_str(), &cardinalityDatabase) != SQLITE_OK)
      throw std::runtime_error("Unable to open estimates DB at " + dbPath);

   sqlite3_exec(cardinalityDatabase, "CREATE TABLE IF NOT EXISTS Estimates (queryID TEXT primary key, estimate INTEGER NOT NULL);", nullptr, nullptr, nullptr);
}
// ---------------------------------------------------------------------------------------------------
void CardinalityOracleDBCached::readEstimatesFromDB() {
   sqlite3_stmt* stmt;
   const char* sql = "SELECT queryID, estimate FROM Estimates;";

   if (sqlite3_prepare_v2(cardinalityDatabase, sql, -1, &stmt, nullptr) != SQLITE_OK)
      throw std::runtime_error("Unable to prepare query to read cardinality DB");

   while (sqlite3_step(stmt) == SQLITE_ROW) {
      const unsigned char* queryIdRaw = sqlite3_column_text(stmt, 0);
      std::string queryId = reinterpret_cast<const char*>(queryIdRaw);
      int64_t estimate = sqlite3_column_int64(stmt, 1);

      assert(!signature2estimate.contains(queryId) && "You would override cardinalities!");
      signature2estimate[queryId] = estimate;
   }
   sqlite3_finalize(stmt);
}
// ---------------------------------------------------------------------------------------------------
void CardinalityOracleDBCached::store() {
   if (changedEstimates.empty())
      return;
   auto& dbPath = SettingBase::getSetting<std::string>("opt.oracle.path.db")->get();
   if (!SettingBase::getSetting<bool>("statistics.silent")->get())
      std::cout << "Store Cardinality Oracle Knowledge to " << dbPath << std::endl;
   if (cardinalityDatabase == nullptr)
      openCardinalityDatabase(dbPath);
   writeEstimatesToDB();
}
// ---------------------------------------------------------------------------------------------------
void CardinalityOracleDBCached::writeEstimatesToDB(bool verbose) {
   sqlite3_exec(cardinalityDatabase, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

   sqlite3_stmt* stmt;
   const char* sql = "INSERT OR REPLACE INTO Estimates (queryID, estimate) VALUES (?, ?);";
   if (sqlite3_prepare_v2(cardinalityDatabase, sql, -1, &stmt, nullptr) != SQLITE_OK)
      throw std::runtime_error("Unable to prepare query to insert into cardinality DB");
   size_t idx = 0;
   for (auto changed : changedEstimates) {
      assert(signature2estimate.contains(changed));
      sqlite3_bind_text(stmt, 1, changed.c_str(), -1, SQLITE_STATIC);
      sqlite3_bind_int64(stmt, 2, signature2estimate[changed]);

      if (sqlite3_step(stmt) != SQLITE_DONE) {
         std::cerr << "Error executing statement: " << sqlite3_errmsg(cardinalityDatabase) << std::endl;
         throw std::runtime_error("Unable to step statement to insert tuple with signatur: " + changed);
      }
      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);
      if (verbose && ++idx % 100 == 0)
         std::cout << "Added " << idx << " elements of " << changedEstimates.size() << " to the Database." << std::endl;
   }
   sqlite3_finalize(stmt);
   sqlite3_exec(cardinalityDatabase, "COMMIT;", nullptr, nullptr, nullptr);
   changedEstimates.clear();
}
// ---------------------------------------------------------------------------------------------------
void CardinalityOracleDBCached::importFileIntoDB() {
   Database dummyDB("", false);
   CardinalityOracleDBCached oracleDBLoad(dummyDB, false);
   CardinalityOracleCSV oracleCSVLoad(dummyDB);
   CardinalityOracleDBCached oracleDB(dummyDB);

   if (importDB.get()) {
      oracleDBLoad.openCardinalityDatabase(CURRENT_SRC_DIR "/factDB/gen/estimates.db.import");
      oracleDBLoad.readEstimatesFromDB();

      std::cout << "Read estimates DONE: " << oracleDBLoad.signature2estimate.size() << " Elements" << std::endl;
   } else {
      // read the estimate file
      auto stream = std::fstream(factDB::SettingBase::getSetting<std::string>("opt.oracle.path.csv")->get(), std::ios_base::in);
      oracleCSVLoad.readEstimatesFromStream(stream);
      std::cout << "Read estimates DONE" << std::endl;
   }

   auto& signature2estimate = importDB.get() ? oracleDBLoad.signature2estimate : oracleCSVLoad.signature2estimate;

   oracleDB.readEstimatesFromDB();
   // add all keys currently loaded to updated keys
   for (auto& [signature, estimate] : signature2estimate) {
      if (!oracleDB.signature2estimate.contains(signature)) {
         oracleDB.signature2estimate[signature] = estimate;
         oracleDB.changedEstimates.push_back(signature);
      } else if (oracleDB.signature2estimate[signature] != estimate) {
         std::cout << "WARNING: estimates for " << signature << " differ: " << estimate << " vs. " << oracleDB.signature2estimate[signature];
         oracleDB.signature2estimate[signature] = estimate;
         oracleDB.changedEstimates.push_back(signature);
      }
   }

   std::cout << "Start writing " << oracleDB.changedEstimates.size() << " Elements to Database." << std::endl;
   oracleDB.writeEstimatesToDB(true);
}
// ---------------------------------------------------------------------------------------------------
#endif
// ---------------------------------------------------------------------------------------------------