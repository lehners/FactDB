#include "factDB/gen/tables/table_KPersonKnowsPerson.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
void KPersonKnowsPerson::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.creationDate(), in, separator, false);
         updateElem(newData.person1(), in, separator, false);
         updateElem(newData.person2(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void KPersonKnowsPerson::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue KPersonKnowsPerson::get(size_t idx, const IU& iu) const {
   if ("creationDate" == iu.column) {
      return data[idx].creationDate().toRTV();
   } else if ("person1" == iu.column) {
      return data[idx].person1().toRTV();
   } else if ("person2" == iu.column) {
      return data[idx].person2().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void KPersonKnowsPerson::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<KPersonKnowsPerson, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table KPersonKnowsPerson::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"creationDate", schemac::Type::Timestamp()},
      {"person1", schemac::Type::UInt64()},
      {"person2", schemac::Type::UInt64()}};
   return {2, "KPersonKnowsPerson", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_KPersonKnowsPerson() { return new KPersonKnowsPerson(); }
/*---------------------------------------------------------------------------*/
void destroy_KPersonKnowsPerson(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
