// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/oracle/CardinalityOracleCSV.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/types/UInt.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/estimators/DummyEstimator.hpp"
#include "factDB/queryc/QueryParseContext.hpp"
#include "factDB/schemac/BaseTable.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace factDB::opt;
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<std::string> oracleFilePath("opt.oracle.path.csv", CURRENT_SRC_DIR "/factDB/gen/estimates.csv");
static factDB::Setting<size_t> oracleFileBackupId("opt.oracle.path.backup", 0);
// ---------------------------------------------------------------------------------------------------
CardinalityOracleCSV::CardinalityOracleCSV(Database& db) : BaseOracle(db) {
   readEstimateFile(oracleFilePath.get());
}
// ---------------------------------------------------------------------------------------------------
size_t CardinalityOracleCSV::calcEstimate(const BaseTablePlan& p, const QueryGraph& qg, const BaseEstimator& /*estimator*/) {
   const auto& relation = qg.getRelation(p.getRelation());
   if (!database.isRelationLoaded(relation.relation))
      database.loadRelation(relation.relation);
   size_t retrievedSize = database.getTable(relation.relation).size();
   return retrievedSize;
}
// ---------------------------------------------------------------------------------------------------
size_t CardinalityOracleCSV::calcEstimate(const JoinPlan& p, const QueryGraph& qg, const BaseEstimator& estimator) {
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
void CardinalityOracleCSV::readEstimatesFromStream(std::istream& stream) {
   while (stream.peek() != EOF) {
      std::string signature, sizeStr;
      std::getline(stream, signature, ';');
      assert(!stream.eof());
      std::getline(stream, sizeStr, '\n');
      assert(!stream.eof());

      auto sizeVal = UInt64::castString(sizeStr).value;
      assert(!signature2estimate.contains(signature) && "You would override cardinalities!");
      signature2estimate[signature] = sizeVal;
   }
}
// ---------------------------------------------------------------------------------------------------
void CardinalityOracleCSV::readEstimateFile(const std::filesystem::path& path) {
   auto stream = std::fstream(path, std::ios_base::in);
   readEstimatesFromStream(stream);
}
// ---------------------------------------------------------------------------------------------------
void CardinalityOracleCSV::writeEstimateFile(const std::filesystem::path& path) {
   auto stream = std::ofstream(path, std::ofstream::trunc);
   writeEstimatesToStream(stream);
   changedEstimates.clear();
}
// ---------------------------------------------------------------------------------------------------
void CardinalityOracleCSV::writeEstimatesToStream(std::ostream& stream) {
   std::vector<std::string> signatures;
   signatures.reserve(signature2estimate.size());
   for (auto& [key, _] : signature2estimate) {
      signatures.push_back(key);
   }
   std::sort(signatures.begin(), signatures.end());

   for (auto& key : signatures) {
      stream << key << ";" << signature2estimate[key] << std::endl;
   }
}
// ---------------------------------------------------------------------------------------------------
void CardinalityOracleCSV::createBackup() {
   auto knowledgePath = oracleFilePath.get();
   // override backup every 10 files
   auto backupPath = knowledgePath + ".backup" + std::to_string(oracleFileBackupId++ % 10);
   if (!SettingBase::getSetting<bool>("statistics.silent")->get())
      std::cout << "Create Cardinality Oracle Knowledge Backup: " << backupPath << std::endl;
   std::filesystem::copy(knowledgePath, backupPath, std::filesystem::copy_options::overwrite_existing);
}
// ---------------------------------------------------------------------------------------------------
void CardinalityOracleCSV::store() {
   if (changedEstimates.empty())
      return;
   if (!SettingBase::getSetting<bool>("statistics.silent")->get())
      std::cout << "Store Cardinality Oracle Knowledge to " << oracleFilePath.get() << std::endl;
   createBackup();
   writeEstimateFile(oracleFilePath.get());
}
// ---------------------------------------------------------------------------------------------------