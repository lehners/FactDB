// ---------------------------------------------------------------------------------------------------
#include "bench/artificial/QueryTrees.hpp"
#include "bench/artificial/DataGen.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "fmt/format.h"
#include <factDB/infra/Expression.hpp>
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
std::string genQueryName(const std::string& prefix, const DataGen& data) {
   return fmt::format("{}_{}x{}x{}", prefix, data.getTableSize(1), data.getTableSize(2), data.getTableSize(3));
}
// ---------------------------------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------------------------------
QueryTrees::ReturnType QueryTrees::genTableScanRel1(const Database& db, const DataGen& data) {
   auto ts1 = std::make_unique<algebra::TableScan>(db, "artificial1");
   auto tree = std::make_unique<algebra::Count>(std::move(ts1));

   std::string queryname = genQueryName("tableScanRel1", data);

   return {std::move(tree), queryname};
}
// ---------------------------------------------------------------------------------------------------
QueryTrees::ReturnType QueryTrees::genJoinsLeftDeep(const Database& db, const DataGen& data, algebra::JoinMode mode) {
   auto ts1 = std::make_unique<algebra::TableScan>(db, "artificial1");
   auto ts2 = std::make_unique<algebra::TableScan>(db, "artificial2");
   auto ts3 = std::make_unique<algebra::TableScan>(db, "artificial3");

   auto jc1 = JoinConditionList::create(ts1->collectIUs()[1], ts2->collectIUs()[0]);
   auto jc2 = JoinConditionList::create(ts2->collectIUs()[1], ts3->collectIUs()[0]);
   std::unique_ptr<algebra::Operator> j1;
   if (mode == algebra::JoinMode::TopInsert) {
      j1 = std::make_unique<algebra::InnerJoin>(std::move(ts1), std::move(ts2), std::move(jc1), algebra::JoinMode::TopInsert);
   } else {
      j1 = std::make_unique<algebra::InnerJoin>(std::move(ts2), std::move(ts1), std::move(jc1), algebra::JoinMode::BottomInsert);
   }
   auto j2 = std::make_unique<algebra::InnerJoin>(std::move(j1), std::move(ts3), std::move(jc2), algebra::JoinMode::TopInsert);
   auto tree = std::make_unique<algebra::Count>(std::move(j2));

   std::string queryname = genQueryName("join123", data);

   return {std::move(tree), queryname};
}
// ---------------------------------------------------------------------------------------------------
