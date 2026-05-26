#include "factDB/gen/tables/table_artificial1.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
void artificial1::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.a(), in, separator, false);
         updateElem(newData.b(), in, separator, false);
         updateElem(newData.id(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void artificial1::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue artificial1::get(size_t idx, const IU& iu) const {
   if ("a" == iu.column) {
      return data[idx].a().toRTV();
   } else if ("b" == iu.column) {
      return data[idx].b().toRTV();
   } else if ("id" == iu.column) {
      return data[idx].id().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void artificial1::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<artificial1, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table artificial1::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"a", schemac::Type::Integer()},
      {"b", schemac::Type::Integer()},
      {"id", schemac::Type::Integer()}};
   return {409, "artificial1", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_artificial1() { return new artificial1(); }
/*---------------------------------------------------------------------------*/
void destroy_artificial1(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
