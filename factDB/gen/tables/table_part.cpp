#include "factDB/gen/tables/table_part.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
void part::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.p_partkey(), in, separator, false);
         updateElem(newData.p_name(), in, separator, false);
         updateElem(newData.p_mfgr(), in, separator, false);
         updateElem(newData.p_category(), in, separator, false);
         updateElem(newData.p_brand1(), in, separator, false);
         updateElem(newData.p_color(), in, separator, false);
         updateElem(newData.p_type(), in, separator, false);
         updateElem(newData.p_size(), in, separator, false);
         updateElem(newData.p_container(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void part::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue part::get(size_t idx, const IU& iu) const {
   if ("p_partkey" == iu.column) {
      return data[idx].p_partkey().toRTV();
   } else if ("p_name" == iu.column) {
      return data[idx].p_name().toRTV();
   } else if ("p_mfgr" == iu.column) {
      return data[idx].p_mfgr().toRTV();
   } else if ("p_category" == iu.column) {
      return data[idx].p_category().toRTV();
   } else if ("p_brand1" == iu.column) {
      return data[idx].p_brand1().toRTV();
   } else if ("p_color" == iu.column) {
      return data[idx].p_color().toRTV();
   } else if ("p_type" == iu.column) {
      return data[idx].p_type().toRTV();
   } else if ("p_size" == iu.column) {
      return data[idx].p_size().toRTV();
   } else if ("p_container" == iu.column) {
      return data[idx].p_container().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void part::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<part, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table part::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"p_partkey", schemac::Type::Integer()},
      {"p_name", schemac::Type::Varchar(22)},
      {"p_mfgr", schemac::Type::Char(6)},
      {"p_category", schemac::Type::Char(7)},
      {"p_brand1", schemac::Type::Char(9)},
      {"p_color", schemac::Type::Varchar(11)},
      {"p_type", schemac::Type::Varchar(25)},
      {"p_size", schemac::Type::Integer()},
      {"p_container", schemac::Type::Char(10)}};
   return {416, "part", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_part() { return new part(); }
/*---------------------------------------------------------------------------*/
void destroy_part(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
