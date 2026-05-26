#include "factDB/gen/tables/table_customer.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
void customer::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.c_custkey(), in, separator, false);
         updateElem(newData.c_name(), in, separator, false);
         updateElem(newData.c_address(), in, separator, false);
         updateElem(newData.c_city(), in, separator, false);
         updateElem(newData.c_nation(), in, separator, false);
         updateElem(newData.c_region(), in, separator, false);
         updateElem(newData.c_phone(), in, separator, false);
         updateElem(newData.c_mktsegment(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void customer::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue customer::get(size_t idx, const IU& iu) const {
   if ("c_custkey" == iu.column) {
      return data[idx].c_custkey().toRTV();
   } else if ("c_name" == iu.column) {
      return data[idx].c_name().toRTV();
   } else if ("c_address" == iu.column) {
      return data[idx].c_address().toRTV();
   } else if ("c_city" == iu.column) {
      return data[idx].c_city().toRTV();
   } else if ("c_nation" == iu.column) {
      return data[idx].c_nation().toRTV();
   } else if ("c_region" == iu.column) {
      return data[idx].c_region().toRTV();
   } else if ("c_phone" == iu.column) {
      return data[idx].c_phone().toRTV();
   } else if ("c_mktsegment" == iu.column) {
      return data[idx].c_mktsegment().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void customer::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<customer, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table customer::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"c_custkey", schemac::Type::Integer()},
      {"c_name", schemac::Type::Varchar(25)},
      {"c_address", schemac::Type::Varchar(25)},
      {"c_city", schemac::Type::Char(10)},
      {"c_nation", schemac::Type::Char(15)},
      {"c_region", schemac::Type::Char(12)},
      {"c_phone", schemac::Type::Char(15)},
      {"c_mktsegment", schemac::Type::Char(10)}};
   return {415, "customer", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_customer() { return new customer(); }
/*---------------------------------------------------------------------------*/
void destroy_customer(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
