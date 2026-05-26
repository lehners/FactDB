#include "factDB/gen/tables/table_lineorder.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
void lineorder::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.lo_orderkey(), in, separator, false);
         updateElem(newData.lo_linenumber(), in, separator, false);
         updateElem(newData.lo_custkey(), in, separator, false);
         updateElem(newData.lo_partkey(), in, separator, false);
         updateElem(newData.lo_suppkey(), in, separator, false);
         updateElem(newData.lo_orderdate(), in, separator, false);
         updateElem(newData.lo_orderpriority(), in, separator, false);
         updateElem(newData.lo_shippriority(), in, separator, false);
         updateElem(newData.lo_quantity(), in, separator, false);
         updateElem(newData.lo_extendedprice(), in, separator, false);
         updateElem(newData.lo_ordtotalprice(), in, separator, false);
         updateElem(newData.lo_discount(), in, separator, false);
         updateElem(newData.lo_revenue(), in, separator, false);
         updateElem(newData.lo_supplycost(), in, separator, false);
         updateElem(newData.lo_tax(), in, separator, false);
         updateElem(newData.lo_commitdate(), in, separator, false);
         updateElem(newData.lo_shipmode(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void lineorder::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue lineorder::get(size_t idx, const IU& iu) const {
   if ("lo_orderkey" == iu.column) {
      return data[idx].lo_orderkey().toRTV();
   } else if ("lo_linenumber" == iu.column) {
      return data[idx].lo_linenumber().toRTV();
   } else if ("lo_custkey" == iu.column) {
      return data[idx].lo_custkey().toRTV();
   } else if ("lo_partkey" == iu.column) {
      return data[idx].lo_partkey().toRTV();
   } else if ("lo_suppkey" == iu.column) {
      return data[idx].lo_suppkey().toRTV();
   } else if ("lo_orderdate" == iu.column) {
      return data[idx].lo_orderdate().toRTV();
   } else if ("lo_orderpriority" == iu.column) {
      return data[idx].lo_orderpriority().toRTV();
   } else if ("lo_shippriority" == iu.column) {
      return data[idx].lo_shippriority().toRTV();
   } else if ("lo_quantity" == iu.column) {
      return data[idx].lo_quantity().toRTV();
   } else if ("lo_extendedprice" == iu.column) {
      return data[idx].lo_extendedprice().toRTV();
   } else if ("lo_ordtotalprice" == iu.column) {
      return data[idx].lo_ordtotalprice().toRTV();
   } else if ("lo_discount" == iu.column) {
      return data[idx].lo_discount().toRTV();
   } else if ("lo_revenue" == iu.column) {
      return data[idx].lo_revenue().toRTV();
   } else if ("lo_supplycost" == iu.column) {
      return data[idx].lo_supplycost().toRTV();
   } else if ("lo_tax" == iu.column) {
      return data[idx].lo_tax().toRTV();
   } else if ("lo_commitdate" == iu.column) {
      return data[idx].lo_commitdate().toRTV();
   } else if ("lo_shipmode" == iu.column) {
      return data[idx].lo_shipmode().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void lineorder::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<lineorder, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table lineorder::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"lo_orderkey", schemac::Type::Integer()},
      {"lo_linenumber", schemac::Type::Integer()},
      {"lo_custkey", schemac::Type::Integer()},
      {"lo_partkey", schemac::Type::Integer()},
      {"lo_suppkey", schemac::Type::Integer()},
      {"lo_orderdate", schemac::Type::Integer()},
      {"lo_orderpriority", schemac::Type::Char(15)},
      {"lo_shippriority", schemac::Type::Char(1)},
      {"lo_quantity", schemac::Type::Integer()},
      {"lo_extendedprice", schemac::Type::Numeric(18, 2)},
      {"lo_ordtotalprice", schemac::Type::Numeric(18, 2)},
      {"lo_discount", schemac::Type::Integer()},
      {"lo_revenue", schemac::Type::Numeric(18, 2)},
      {"lo_supplycost", schemac::Type::Numeric(18, 2)},
      {"lo_tax", schemac::Type::Integer()},
      {"lo_commitdate", schemac::Type::Integer()},
      {"lo_shipmode", schemac::Type::Char(10)}};
   return {417, "lineorder", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_lineorder() { return new lineorder(); }
/*---------------------------------------------------------------------------*/
void destroy_lineorder(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
