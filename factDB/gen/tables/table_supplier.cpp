#include "factDB/gen/tables/table_supplier.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
void supplier::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.s_suppkey(), in, separator, false);
         updateElem(newData.s_name(), in, separator, false);
         updateElem(newData.s_address(), in, separator, false);
         updateElem(newData.s_city(), in, separator, false);
         updateElem(newData.s_nation(), in, separator, false);
         updateElem(newData.s_region(), in, separator, false);
         updateElem(newData.s_phone(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void supplier::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue supplier::get(size_t idx, const IU& iu) const {
   if ("s_suppkey" == iu.column) {
      return data[idx].s_suppkey().toRTV();
   } else if ("s_name" == iu.column) {
      return data[idx].s_name().toRTV();
   } else if ("s_address" == iu.column) {
      return data[idx].s_address().toRTV();
   } else if ("s_city" == iu.column) {
      return data[idx].s_city().toRTV();
   } else if ("s_nation" == iu.column) {
      return data[idx].s_nation().toRTV();
   } else if ("s_region" == iu.column) {
      return data[idx].s_region().toRTV();
   } else if ("s_phone" == iu.column) {
      return data[idx].s_phone().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void supplier::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<supplier, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table supplier::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"s_suppkey", schemac::Type::Integer()},
      {"s_name", schemac::Type::Char(25)},
      {"s_address", schemac::Type::Varchar(25)},
      {"s_city", schemac::Type::Char(10)},
      {"s_nation", schemac::Type::Char(15)},
      {"s_region", schemac::Type::Char(12)},
      {"s_phone", schemac::Type::Char(15)}};
   return {414, "supplier", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_supplier() { return new supplier(); }
/*---------------------------------------------------------------------------*/
void destroy_supplier(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
