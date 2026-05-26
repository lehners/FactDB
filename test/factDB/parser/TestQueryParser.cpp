// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include "factDB/infra/RuntimeException.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/util/LineSortingStream.hpp"
#include "factDB/parser/AST.hpp"
#include "factDB/parser/SchemaParseContext.hpp"
#include "factDB/parser/SemanticAnalysis.hpp"
#include "factDB/statement/QueryStatement.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "gtest/gtest.h"
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
using namespace std;
// ---------------------------------------------------------------------------------------------------
namespace {
template <class TupType, size_t... I>
void print(std::stringstream& ss, const TupType& tup, std::index_sequence<I...>, std::string separator) {
   (..., (ss << (I == 0 ? "" : separator) << std::get<I>(tup))); // NOLINT
}

template <class... T>
void print(std::stringstream& ss, const std::tuple<T...>& tup, std::string separator) {
   print(ss, tup, std::make_index_sequence<sizeof...(T)>(), separator);
}

template <class... T>
std::string genTupleString(std::vector<std::tuple<T...>> tuples, std::string separator = ",") {
   std::sort(tuples.begin(), tuples.end());
   std::stringstream ss;
   for (auto& t : tuples) {
      print(ss, t, separator);
      ss << "\n";
   }
   return ss.str();
}
} // anonymous namespace
// ---------------------------------------------------------------------------------------------------
std::string executeQuery(Database& db, const std::string& query, std::string separator = ",") { // NOLINT(bugprone-easily-swappable-parameters)
   auto temporary = factDB::SettingBase::getSetting("algebra.delimiter")->setTemporary<std::string>(std::move(separator));

   std::stringstream queryStream;
   queryStream << query;

   LineSortingStream lss;

   factDB::parser::SchemaParseContext parser;
   parser.parse(queryStream);

   for (auto& tree : parser.getParsedTrees()) {
      auto stmt = factDB::parser::SemanticAnalysis::buildStatement(*tree, db);
      stmt->setOutStream(lss);
      stmt->runStatement(db);

      lss.sortLines();
   }
   return lss.concatLines();
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_parser_QueryParser, SimpleSQL) {
   auto tmpReorder = SettingBase::getSetting("opt.reorderPlans")->setTemporary(true);
   auto tmp = SettingBase::getSetting("opt.oracle.baseTableFromUmbra")->setTemporary(false);
   Database db = DatabaseLoadUtil::genTestDB();
   {
      auto res = genTupleString<int, int>({{1, 2}, {1, 2}, {2, 3}, {2, 4}, {3, 5}});
      ASSERT_EQ(executeQuery(db, "SELECT * FROM KRelationTest"), res);
   }
   {
      auto res = genTupleString<int, int, int>({{1, 1, 2}, {1, 1, 2}, {2, 2, 3}, {2, 2, 4}, {3, 3, 5}});
      ASSERT_EQ(executeQuery(db, "SELECT a, * FROM KRelationTest"), res);
   }
   {
      auto res = genTupleString<int, int>({{2, 1}, {2, 1}, {3, 2}, {4, 2}, {5, 3}});
      ASSERT_EQ(executeQuery(db, "SELECT b, a FROM KRelationTest"), res);
   }
   {
      auto temporary = factDB::SettingBase::getSetting("optimizer.doOptimize")->setTemporaryBool(false);
      auto res = genTupleString<int, int>({{1, 2}, {1, 2}});
      ASSERT_EQ(executeQuery(db, "SELECT a, b FROM KRelationTest WHERE a = 1"), res);
   }
   {
      auto temporary = factDB::SettingBase::getSetting("optimizer.doOptimize")->setTemporaryBool(false);
      auto res = genTupleString<int, int>({{3, 5}});
      ASSERT_EQ(executeQuery(db, "SELECT a,b FROM KRelationTest WHERE b=5"), res);
   }

   ASSERT_THROW(executeQuery(db, "SELECT b, a, c FROM KRelationTest"), RuntimeException);
   ASSERT_THROW(executeQuery(db, "SELECT b, a, c FROM KRelationTes"), RuntimeException);
   ASSERT_THROW(executeQuery(db, "SELECT b, a, c FROM"), factDB::parser::SchemaCompilationError);
   ASSERT_THROW(executeQuery(db, "SELECT FROM"), factDB::parser::SchemaCompilationError);
}
// ---------------------------------------------------------------------------------------------------
