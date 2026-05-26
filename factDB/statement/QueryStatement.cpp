// ---------------------------------------------------------------------------------------------------
#include "factDB/statement/QueryStatement.hpp"
#include "factDB/algebra/Operator.hpp"
#include "factDB/infra/IUSet.hpp"
#include "factDB/infra/RuntimeException.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/queryc/ExecutionMode.hpp"
#include "factDB/queryc/QueryParseContext.hpp"
#include <cassert>
#include <iostream>
#include <regex>
// ---------------------------------------------------------------------------------------------------
namespace factDB::statement {
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<std::string> queryNameFilter("queryname.filter", "");
// ---------------------------------------------------------------------------------------------------
QueryStatement::QueryStatement(std::unique_ptr<algebra::Operator> tree_, factDB::OrderedIUSet ius_)
   : Statement(Statement::QueryStatement), tree(std::move(tree_)), ius(std::move(ius_)) {
}
// ---------------------------------------------------------------------------------------------------
void QueryStatement::runStatement(factDB::Database& db) {
   auto printQueryname = factDB::SettingBase::getSetting<bool>("queryname.print")->get();
   auto& curQueryname = SettingBase::getSetting<std::string>("queryname")->get();
   auto& pattern = queryNameFilter.get();
   if (!pattern.empty() && !std::regex_search(curQueryname, std::regex(pattern))) {
      if (printQueryname)
         std::cout << curQueryname << " skipped, not matching regex" << std::endl;
      return;
   }

   assert(tree != nullptr && "Query was probably already executed.");
   factDB::queryc::QueryParseContext context(db);
   try {
      context.compileAndExecute(std::move(tree), ius, db, *outStream);
   } catch (factDB::RuntimeException& e) {
      switch (e.getErrorCode()) {
         case ErrorCode::UmbraTimout:
            std::cerr << curQueryname << ": Umbra Timed out, skip query..." << std::endl;
            break;
         case ErrorCode::UmbraOoM:
            std::cerr << curQueryname << ": Umbra went out of memory, skip query..." << std::endl;
            break;
         case ErrorCode::DoNotGeneratePlan:
            std::cerr << curQueryname << ": There is no cached plan for this query and setting to generate them is disabled." << std::endl;
            break;
         default:
            throw e;
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::statement
// ---------------------------------------------------------------------------------------------------