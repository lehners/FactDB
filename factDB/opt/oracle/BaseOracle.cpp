// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/oracle/BaseOracle.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/visitors/SQLGeneratorSemijoinGroupBy.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/opt/Plan.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/QuerySignature.hpp"
#include <sstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<bool> oracleAutoSave("opt.oracle.autosave", true);
// ---------------------------------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------------------------------
std::string BaseOracle::generateSignature(std::unique_ptr<algebra::Operator>& op) {
   std::stringstream ss;
   assert(op->getType() == algebra::Operator::Count);
   OrderedIUSet reqIUs = static_cast<algebra::Count*>(op.get())->getRequiredIus();
   algebra::visitors::SQLGeneratorSemijoinGroupBy::genSQL(op, database, reqIUs, ss);
   return ss.str();
}
// ---------------------------------------------------------------------------------------------------
size_t BaseOracle::estimate(const Plan* plan, const QueryGraph& qg, const BaseEstimator& estimator) {
   switch (plan->getType()) {
      case Uninitialized: not_implemented(); break;
      case Join: return calcEstimate(*static_cast<const JoinPlan*>(plan), qg, estimator);
      case TableScan: not_implemented(); break;
      case BaseTable: return calcEstimate(*static_cast<const BaseTablePlan*>(plan), qg, estimator);
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
