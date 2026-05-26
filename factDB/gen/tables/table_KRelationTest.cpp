#include "factDB/gen/tables/table_KRelationTest.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
void KRelationTest::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.a(), in, separator, false);
         updateElem(newData.b(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void KRelationTest::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue KRelationTest::get(size_t idx, const IU& iu) const {
   if ("a" == iu.column) {
      return data[idx].a().toRTV();
   } else if ("b" == iu.column) {
      return data[idx].b().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void KRelationTest::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<KRelationTest, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table KRelationTest::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"a", schemac::Type::Integer()},
      {"b", schemac::Type::Integer()}};
   return {0, "KRelationTest", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_KRelationTest() { return new KRelationTest(); }
/*---------------------------------------------------------------------------*/
void destroy_KRelationTest(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
