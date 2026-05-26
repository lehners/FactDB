// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include "factDB/SQLExecution.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/newftree/generator/fnode/Graphviz.hpp"
#include "factDB/queryc/ExecutionMode.hpp"
#include "factDB/statement/QueryStatement.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<algebra::Count> createWorldIsNotFlatQuery(const factDB::Database& db) {
   auto rA = std::make_unique<factDB::algebra::TableScan>(db, winf::WINF_A, "rA");
   auto rB = std::make_unique<factDB::algebra::TableScan>(db, winf::WINF_B, "rB");
   auto rC = std::make_unique<factDB::algebra::TableScan>(db, winf::WINF_C, "rC");
   auto rD = std::make_unique<factDB::algebra::TableScan>(db, winf::WINF_D, "rD");

   auto predicateAD = JoinConditionList::create(rD->collectIUs()[1], rA->collectIUs()[1]);
   auto predicateBC = JoinConditionList::create(rB->collectIUs()[1], rC->collectIUs()[1]);
   auto predicateAB = JoinConditionList::create(rA->collectIUs()[1], rB->collectIUs()[1]);

   auto joinAD = std::make_unique<algebra::InnerJoin>(std::move(rD), std::move(rA), std::move(predicateAD), algebra::JoinMode::TopInsert);
   auto joinBC = std::make_unique<algebra::InnerJoin>(std::move(rB), std::move(rC), std::move(predicateBC), algebra::JoinMode::BottomInsert);
   auto joinABCD = std::make_unique<algebra::InnerJoin>(std::move(joinAD), std::move(joinBC), std::move(predicateAB), algebra::JoinMode::BottomInsert);
   return std::make_unique<algebra::Count>(std::move(joinABCD));
}
// ---------------------------------------------------------------------------------------------------
int main() {
   auto db = factDB::DatabaseLoadUtil::genById("testdb", {}, false);
   [[maybe_unused]] auto tmpCodegenExecutionMode = factDB::SettingBase::getSetting("codegen.executionMode")->setTemporary(queryc::ExecutionMode::CodegenFactorized);
   [[maybe_unused]] auto tmpOptimizerAlg = factDB::SettingBase::getSetting("optimizer.doOptimize")->setTemporary(false);
   [[maybe_unused]] auto tmpQueryname = factDB::SettingBase::getSetting<std::string>("queryname")->setTemporary("WINF");
   SettingBase::getSetting<bool>("codegen.pregenerateQueries")->set(true);

   statement::QueryStatement query_statement(createWorldIsNotFlatQuery(db), {});
   query_statement.runStatement(db);
   return 0;
}
// ---------------------------------------------------------------------------------------------------
