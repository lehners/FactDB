#include "factDB/gen/tables/table_KRelationTestThree.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
void KRelationTestThree::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.a(), in, separator, false);
         updateElem(newData.b(), in, separator, false);
         updateElem(newData.c(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void KRelationTestThree::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue KRelationTestThree::get(size_t idx, const IU& iu) const {
   if ("a" == iu.column) {
      return data[idx].a().toRTV();
   } else if ("b" == iu.column) {
      return data[idx].b().toRTV();
   } else if ("c" == iu.column) {
      return data[idx].c().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void KRelationTestThree::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<KRelationTestThree, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table KRelationTestThree::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"a", schemac::Type::Integer()},
      {"b", schemac::Type::Integer()},
      {"c", schemac::Type::Integer()}};
   return {1, "KRelationTestThree", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_KRelationTestThree() { return new KRelationTestThree(); }
/*---------------------------------------------------------------------------*/
void destroy_KRelationTestThree(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
