// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/umbra/UmbraVisitor.hpp"
#include <factDB/Database.hpp>
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/visitors/SQLGenerator.hpp"
#include "factDB/algebra/visitors/SQLGeneratorSemijoinGroupBy.hpp"
#include "factDB/algebra/visitors/umbra/UmbraConnector.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
#include <iostream>
#include <sstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<bool> sqlGeneratorGroupby("opt.oracle.sqlgenerator.groupby", true);
// ---------------------------------------------------------------------------------------------------
UmbraConnector& getUmbraInstance() {
   auto& binary = SettingBase::getSetting<std::string>("visitor.umbra.bin")->get();
   auto args = {SettingBase::getSetting<std::string>("visitor.umbra.db")->get()};
   static UmbraConnector umbraExecutor(binary, args);
   return umbraExecutor;
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<algebra::visitors::AlgebraVisitor> getSQLGenerator(std::ostream& writer, const Database& db) {
   if (sqlGeneratorGroupby.get()) {
      return std::make_unique<SQLGeneratorSemijoinGroupBy>(writer, db);
   } else {
      return std::make_unique<SQLGenerator>(writer, db);
   }
}
// ---------------------------------------------------------------------------------------------------
UmbraServerConnector& getUmbraServerInstance() {
   static UmbraServerConnector instance;
   return instance;
}
// ---------------------------------------------------------------------------------------------------
UmbraVisitor::UmbraVisitor(const Database& db, std::ostream& writer)
   : AlgebraVisitorDB(db), out(writer) {
}
// ---------------------------------------------------------------------------------------------------
UmbraVisitorServer::UmbraVisitorServer(const factDB::Database& db, std::ostream& writer)
   : UmbraVisitor(db, writer), umbraExecutor(getUmbraServerInstance()) {
}
// ---------------------------------------------------------------------------------------------------
UmbraVisitorLocal::UmbraVisitorLocal(const factDB::Database& db, std::ostream& writer)
   : UmbraVisitor(db, writer), umbraExecutor(getUmbraInstance()) {
}
// ---------------------------------------------------------------------------------------------------
std::string prepareProduce(Print& print, const Database& database) {
   std::string sqlStatement;
   {
      std::stringstream ostream;
      // generate sql statement:
      auto sqlGen = getSQLGenerator(ostream, database);
      print.prepare(*sqlGen, print.getRequiredIus(), nullptr);
      print.produce(*sqlGen);
      sqlStatement = ostream.str();
   }
   if (!SettingBase::getSetting<bool>("statistics.silent")->get())
      std::cout << sqlStatement << std::endl;
   return sqlStatement;
}
// ---------------------------------------------------------------------------------------------------
void afterProduce(FileWriter& out, std::string& res) {
   size_t charIdx = 0;
   while (res[charIdx++] != '\n');
   for (; charIdx != res.size(); ++charIdx) {
      const char curChar = res[charIdx];
      if (curChar != '\n') [[likely]] {
         out << curChar;
      } else {
         out << fw::endl();
      }
   }
}
// ---------------------------------------------------------------------------------------------------
void UmbraVisitorServer::visitProduce(Print& print) {
   auto sqlStatement = prepareProduce(print, database);
   auto res = umbraExecutor.sendCmd(sqlStatement, true);
   afterProduce(out, res);
}
// ---------------------------------------------------------------------------------------------------
void UmbraVisitorLocal::visitProduce(Print& print) {
   auto sqlStatement = prepareProduce(print, database);
   auto res = umbraExecutor.sendCmd(sqlStatement, true);
   afterProduce(out, res);
}
// ---------------------------------------------------------------------------------------------------
std::string prepareProduce(Count& count, const Database& database) {
   std::string sqlStatement;
   {
      std::stringstream ostream;
      // generate sql statement:
      auto sqlGen = getSQLGenerator(ostream, database);
      OrderedIUSet iuSet = count.getRequiredIus();
      count.prepare(*sqlGen, count.getRequiredIus(), nullptr);
      count.produce(*sqlGen);
      sqlStatement = ostream.str();
   }
   if (!SettingBase::getSetting<bool>("statistics.silent")->get())
      std::cout << sqlStatement << std::endl;
   return sqlStatement;
}
// ---------------------------------------------------------------------------------------------------
void UmbraVisitorServer::visitProduce(Count& count) {
   auto sqlStatement = prepareProduce(count, database);
   auto res = umbraExecutor.sendCountStar(sqlStatement);
   out << res << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void UmbraVisitorLocal::visitProduce(Count& count) {
   auto sqlStatement = prepareProduce(count, database);
   auto res = umbraExecutor.sendCountStar(sqlStatement);
   out << res << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
