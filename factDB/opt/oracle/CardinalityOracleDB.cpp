// ---------------------------------------------------------------------------------------------------
#ifdef HAS_SQLITE3
// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/oracle/CardinalityOracleDB.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/types/UInt.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/estimators/DummyEstimator.hpp"
#include "factDB/queryc/ExecutionMode.hpp"
#include "factDB/queryc/QueryParseContext.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "sqlite3.h"
#include <sstream>
// ---------------------------------------------------------------------------------------------------
using namespace factDB::opt;
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<std::string> oracleFilePathDB("opt.oracle.path.db", CURRENT_SRC_DIR "/factDB/gen/estimates.db");
static factDB::Setting<bool> generateEstimates("opt.oracle.genEstimates", false);
// ---------------------------------------------------------------------------------------------------
CardinalityOracleDB::CardinalityOracleDB(Database& db) : BaseOracle(db) {
   openCardinalityDatabase(SettingBase::getSetting<std::string>("opt.oracle.path.db")->get());
}
// ---------------------------------------------------------------------------------------------------
CardinalityOracleDB::CardinalityOracleDB(CardinalityOracleDB&& other) noexcept
   : BaseOracle(other.database), cardinalityDatabase(other.cardinalityDatabase) {
   other.cardinalityDatabase = nullptr;
}
// ---------------------------------------------------------------------------------------------------
CardinalityOracleDB& CardinalityOracleDB::operator=(factDB::opt::CardinalityOracleDB&& other) noexcept {
   if (this != &other) {
      if (cardinalityDatabase != nullptr) {
         sqlite3_close(cardinalityDatabase);
      }
      cardinalityDatabase = other.cardinalityDatabase;
      other.cardinalityDatabase = nullptr;
   }
   return *this;
}
// ---------------------------------------------------------------------------------------------------
CardinalityOracleDB::~CardinalityOracleDB() {
   if (cardinalityDatabase != nullptr)
      sqlite3_close(cardinalityDatabase);
}
// ---------------------------------------------------------------------------------------------------
size_t CardinalityOracleDB::calcEstimate(const BaseTablePlan& p, const QueryGraph& qg, const BaseEstimator& estimator) {
   auto& baseTableFromUmbra = *SettingBase::getSetting<bool>("opt.oracle.baseTableFromUmbra");
   if (baseTableFromUmbra.get()) {
      return estimateSignature(p, qg, estimator);
   } else {
      const auto& relation = qg.getRelation(p.getRelation());
      if (!database.isRelationLoaded(relation.relation))
         database.loadRelation(relation.relation);
      size_t retrievedSize = database.getTable(relation.relation).size();
      return retrievedSize;
   }
}
// ---------------------------------------------------------------------------------------------------
size_t CardinalityOracleDB::calcEstimate(const JoinPlan& p, const QueryGraph& qg, const BaseEstimator& estimator) {
   return estimateSignature(p, qg, estimator);
}
// ---------------------------------------------------------------------------------------------------
size_t CardinalityOracleDB::estimateSignature(const Plan& p, const QueryGraph& qg, const BaseEstimator& estimator) {
   auto operatorPlan = p.generateOperatorPlan(qg, estimator);
   std::unique_ptr<algebra::Operator> count = std::make_unique<algebra::Count>(std::move(operatorPlan));
   auto signature = generateSignature(count);
   if (auto [resFound, resEstimate] = getEstimate(signature); resFound) {
      return resEstimate;
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
            if (!generateEstimates.get())
               break;
         }
      }
      // store the result in the hashtable
      signature2estimate[signature] = retrievedSize;
      insertEstimate(signature, retrievedSize);
      if (SettingBase::getSetting<bool>("opt.oracle.autosave")->get()) store();
      return retrievedSize;
   }
}
// ---------------------------------------------------------------------------------------------------
void CardinalityOracleDB::openCardinalityDatabase(const std::string& dbPath) {
   assert(cardinalityDatabase == nullptr);
   if (sqlite3_open(dbPath.c_str(), &cardinalityDatabase) != SQLITE_OK) {
      throw std::runtime_error("Unable to open estimates DB at " + dbPath);
   }
   char* errMsg = nullptr;
   auto rc1 = sqlite3_exec(cardinalityDatabase, "CREATE TABLE IF NOT EXISTS Estimates (queryID TEXT primary key, schema TEXT NOT NULL, estimate INTEGER NOT NULL);", nullptr, nullptr, &errMsg);
   if (rc1 != SQLITE_OK)
      std::cerr << "SQL error: " << errMsg << "\n";
   auto rc2 = sqlite3_exec(cardinalityDatabase, "CREATE TABLE IF NOT EXISTS CachedPlans (queryname TEXT, schema TEXT NOT NULL, optimizerAlgorithm INTEGER NOT NULL, plan TEXT NOT NULL, primary key(queryname, optimizerAlgorithm));", nullptr, nullptr, &errMsg);
   if (rc2 != SQLITE_OK)
      std::cerr << "SQL error: " << errMsg << "\n";
   if (errMsg != nullptr)
      sqlite3_free(errMsg);
}
// ---------------------------------------------------------------------------------------------------
std::pair<bool, size_t> CardinalityOracleDB::getEstimate(const std::string& signature) const {
   sqlite3_stmt* stmt;
   const char* sql = "SELECT estimate FROM Estimates WHERE queryID=? and schema=?;";

   if (auto res = sqlite3_prepare_v2(cardinalityDatabase, sql, -1, &stmt, nullptr); res != SQLITE_OK) {
      throw std::runtime_error("Unable to prepare query to read cardinality DB");
   }

   sqlite3_bind_text(stmt, 1, signature.c_str(), -1, SQLITE_STATIC);
   sqlite3_bind_text(stmt, 2, SettingBase::getSetting<std::string>("schema.key")->get().c_str(), -1, SQLITE_STATIC);
   DoOnDestruction dod([&]() { sqlite3_finalize(stmt); });
   auto res = sqlite3_step(stmt);
   if (res == SQLITE_DONE) {
      return {false, 0};
   } else if (res != SQLITE_ROW) {
      std::cerr << "Error executing statement: " << sqlite3_errmsg(cardinalityDatabase) << std::endl;
      throw std::runtime_error("Unable to step statement to get tuple with signature: " + signature);
   }
   int64_t estimate = sqlite3_column_int64(stmt, 0);
   res = sqlite3_step(stmt);
   assert(res == SQLITE_DONE);
   return {true, estimate};
}
// ---------------------------------------------------------------------------------------------------
void CardinalityOracleDB::insertEstimate(const std::string& signature, size_t estimate) const {
   sqlite3_stmt* stmt;
   const char* sql = "INSERT OR REPLACE INTO Estimates (queryID, schema, estimate) VALUES (?, ?, ?);";
   if (sqlite3_prepare_v2(cardinalityDatabase, sql, -1, &stmt, nullptr) != SQLITE_OK)
      throw std::runtime_error("Unable to prepare query to insert into cardinality DB");

   sqlite3_bind_text(stmt, 1, signature.c_str(), -1, SQLITE_STATIC);
   sqlite3_bind_text(stmt, 2, SettingBase::getSetting<std::string>("schema.key")->get().c_str(), -1, SQLITE_STATIC);
   sqlite3_bind_int64(stmt, 3, estimate);
   DoOnDestruction dod([&]() { sqlite3_finalize(stmt); });

   if (sqlite3_step(stmt) != SQLITE_DONE) {
      std::cerr << "Error executing statement: " << sqlite3_errmsg(cardinalityDatabase) << std::endl;
      throw std::runtime_error("Unable to step statement to insert tuple with signature: " + signature);
   }
}
// ---------------------------------------------------------------------------------------------------
void CardinalityOracleDB::insertCachedPlan(const Plan* plan, OptimizerAlgorithm::Algorithm algorithm, const QueryGraph& queryGraph) {
   std::stringstream ss;
   FileWriter writer(ss, FileWriter::NoFinalNewline);
   plan->print(writer, queryGraph);
   auto planStr = ss.str();

   sqlite3_stmt* stmt;
   const char* sql = "INSERT OR REPLACE INTO CachedPlans (queryname, schema, optimizerAlgorithm, plan) VALUES (?, ?, ?, ?);";
   if (sqlite3_prepare_v2(cardinalityDatabase, sql, -1, &stmt, nullptr) != SQLITE_OK)
      throw std::runtime_error("Unable to prepare query to insert into cardinality DB");

   std::string queryname = SettingBase::getSetting<std::string>("queryname")->get();
   sqlite3_bind_text(stmt, 1, queryname.c_str(), -1, SQLITE_STATIC);
   sqlite3_bind_text(stmt, 2, SettingBase::getSetting<std::string>("schema.key")->get().c_str(), -1, SQLITE_STATIC);
   sqlite3_bind_int64(stmt, 3, OptimizerAlgorithm::toInt(algorithm));
   sqlite3_bind_text(stmt, 4, planStr.c_str(), -1, SQLITE_STATIC);
   DoOnDestruction dod([&]() { sqlite3_finalize(stmt); });

   if (sqlite3_step(stmt) != SQLITE_DONE) {
      std::cerr << "Error executing statement: " << sqlite3_errmsg(cardinalityDatabase) << std::endl;
      throw std::runtime_error("Unable to step statement to insert cached plan for query: " + queryname);
   }
}
// ---------------------------------------------------------------------------------------------------
std::string CardinalityOracleDB::lookupCachedPlan(opt::OptimizerAlgorithm::Algorithm algorithm) {
   if (!SettingBase::getSetting<bool>("optimizer.cache")->get())
      return "";

   sqlite3_stmt* stmt;
   const char* sql = "SELECT plan FROM CachedPlans WHERE queryname=? AND schema=? AND optimizerAlgorithm=?;";

   if (auto res = sqlite3_prepare_v2(cardinalityDatabase, sql, -1, &stmt, nullptr); res != SQLITE_OK) {
      throw std::runtime_error("Unable to prepare query to read cardinality DB");
   }

   std::string queryname = SettingBase::getSetting<std::string>("queryname")->get();
   sqlite3_bind_text(stmt, 1, queryname.c_str(), -1, SQLITE_STATIC);
   sqlite3_bind_text(stmt, 2, SettingBase::getSetting<std::string>("schema.key")->get().c_str(), -1, SQLITE_STATIC);
   sqlite3_bind_int64(stmt, 3, OptimizerAlgorithm::toInt(algorithm));
   DoOnDestruction dod([&]() { sqlite3_finalize(stmt); });
   auto res = sqlite3_step(stmt);
   if (res == SQLITE_DONE) {
      return "";
   } else if (res != SQLITE_ROW) {
      std::cerr << "Error executing statement: " << sqlite3_errmsg(cardinalityDatabase) << std::endl;
      throw std::runtime_error("Unable to step statement to get plan for query: " + queryname);
   }
   std::string plan(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
   res = sqlite3_step(stmt);
   assert(res == SQLITE_DONE);
   return plan;
}
// ---------------------------------------------------------------------------------------------------
#endif
// ---------------------------------------------------------------------------------------------------
