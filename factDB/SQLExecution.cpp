// ---------------------------------------------------------------------------------------------------
#include "factDB/SQLExecution.hpp"
#include "algebra/Operator.hpp"
#include "factDB/Database.hpp"
#include "factDB/config.h"
#include "factDB/infra/RuntimeException.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/alloc/BumpAllocator.hpp"
#include "factDB/parser/AST.hpp"
#include "factDB/parser/SchemaParseContext.hpp"
#include "factDB/parser/SemanticAnalysis.hpp"
#include "factDB/statement/CreateTableStatement.hpp"
#include "fmt/format.h"
#include <fstream>
#include <iostream>
#include <sstream>
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<std::string> queryNameSetting("queryname", "");
static factDB::Setting<bool> queryNamePrint("queryname.print", true);
static factDB::Setting<size_t> queryResultSizeSetting("resultsize", 0);
static factDB::Setting<bool> validateResultSize("codegen.validateResultSize", false);
// ---------------------------------------------------------------------------------------------------
SQLExecutor::SQLExecutor(factDB::Database& db) : database(db) {
}
// ---------------------------------------------------------------------------------------------------
void SQLExecutor::handleBackslashCommand(const std::string& line) {
   std::stringstream lineStream(line);
   std::string command;
   lineStream >> command;

   if (command == "\\q" || command == "\\quit" || command == "\\exit") {
      state = Stop;
   } else if (command == "\\i") {
      std::string filename;
      lineStream >> filename;
      std::ifstream sqlFile(CURRENT_SRC_DIR "/" + filename);
      if (!sqlFile.good()) throw RuntimeException(RuntimeError, fmt::format("file {} not found", filename));
      executeSQL(sqlFile);
   } else if (command == "\\set") {
      std::string settingName;
      lineStream >> settingName;
      auto setting = SettingBase::getSetting(settingName);
      if (setting == nullptr) throw RuntimeException(RuntimeError, fmt::format("setting {} not found", settingName));

      if (lineStream.eof()) {
         std::cout << settingName << ": " << setting->getOutput() << std::endl;
      } else {
         std::string value;
         lineStream >> value;
         if (setting->interpret(value)) {
            if (queryNamePrint.get())
               std::cerr << settingName << ": " << setting->getOutput() << std::endl;
         } else {
            std::cout << "Error setting " << settingName << " to value \"" << value << "\". Allowed values are: " << setting->getParserDescription();
         }
      }
   } else {
      std::cout << "ignoring unknown backslash command \"" << line << "\"" << std::endl;
   }
}
// ---------------------------------------------------------------------------------------------------
void SQLExecutor::parseStream(std::istream& stream, std::ostream& outStream) {
   factDB::parser::SchemaParseContext parser;

   parser.parse(stream);

   auto parsedTrees = parser.getParsedTrees();
   for (size_t i = 0; i != parser.parsedTreesSize(); ++i) {
      auto* tree = parsedTrees[i];
      auto stmt = parser::SemanticAnalysis::buildStatement(*tree, database);
      stmt->setOutStream(outStream);

      if (tree->getASTType() == parser::AST::CreateTableType && i + 1 < parsedTrees.size()) { // speedup large schema definitions
         auto* createTblStmt = static_cast<statement::CreateTableStatement*>(stmt.get());
         bool isNextAlsoCreateTable = parsedTrees[i + 1]->getASTType() == parser::AST::CreateTableType;
         createTblStmt->runStatement(database, !isNextAlsoCreateTable);
      } else if (true || tree->getASTType() != parser::AST::SelectType) {
         algebra::Operator::resetIds();
         stmt->runStatement(database);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
void SQLExecutor::executeSQL(const std::string& str, bool instantProcess) {
   std::stringstream ss(str);
   return executeSQL(ss, instantProcess);
}
// ---------------------------------------------------------------------------------------------------
void SQLExecutor::executeSQL(std::istream& stream, bool instantProcess) {
   return executeSQL(stream, std::cout, instantProcess);
}
// ---------------------------------------------------------------------------------------------------
size_t parseResultSize(const std::string& line) {
   using namespace std;
   constexpr size_t resultSizeLength = "-- Result size: "sv.length();
   auto resultSize = line.substr(resultSizeLength);
   return UInt64::castString(resultSize).value;
}
// ---------------------------------------------------------------------------------------------------
void SQLExecutor::executeSQL(std::istream& stream, std::ostream& outStream, bool instantProcess) {
   std::stringstream ss;
   std::string line;

   while (state != Stop && std::getline(stream, line)) {
      if (line.starts_with('\\')) {
         parseStream(ss, outStream);
         handleBackslashCommand(std::move(line));
         ss.clear();
      } else if (line.starts_with("--")) { // comment, ignore line
         // check if result size specified
         if (line.starts_with("-- Result size:")) {
            queryResultSizeSetting.set(parseResultSize(line));
         }
         continue;
      } else {
         ss << line << std::endl;
         if (instantProcess && !line.empty() && line[line.size() - 1] == ';') {
            parseStream(ss, outStream);
            ss.clear();
         }
      }
   }
   parseStream(ss, outStream);
}
// ---------------------------------------------------------------------------------------------------