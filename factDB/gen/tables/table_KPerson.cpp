#include "factDB/gen/tables/table_KPerson.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
void KPerson::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.creationDate(), in, separator, false);
         updateElem(newData.idx(), in, separator, false);
         updateElem(newData.last_name(), in, separator, false);
         updateElem(newData.first_name(), in, separator, false);
         updateElem(newData.gender(), in, separator, false);
         updateElem(newData.birthday(), in, separator, false);
         updateElem(newData.location_ip(), in, separator, false);
         updateElem(newData.browser_used(), in, separator, false);
         updateElem(newData.city_id(), in, separator, false);
         updateElem(newData.speaks(), in, separator, false);
         updateElem(newData.email(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void KPerson::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue KPerson::get(size_t idx_, const IU& iu) const {
   if ("creationDate" == iu.column) {
      return data[idx_].creationDate().toRTV();
   } else if ("idx" == iu.column) {
      return data[idx_].idx().toRTV();
   } else if ("last_name" == iu.column) {
      return data[idx_].last_name().toRTV();
   } else if ("first_name" == iu.column) {
      return data[idx_].first_name().toRTV();
   } else if ("gender" == iu.column) {
      return data[idx_].gender().toRTV();
   } else if ("birthday" == iu.column) {
      return data[idx_].birthday().toRTV();
   } else if ("location_ip" == iu.column) {
      return data[idx_].location_ip().toRTV();
   } else if ("browser_used" == iu.column) {
      return data[idx_].browser_used().toRTV();
   } else if ("city_id" == iu.column) {
      return data[idx_].city_id().toRTV();
   } else if ("speaks" == iu.column) {
      return data[idx_].speaks().toRTV();
   } else if ("email" == iu.column) {
      return data[idx_].email().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void KPerson::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<KPerson, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table KPerson::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"creationDate", schemac::Type::Timestamp()},
      {"idx", schemac::Type::UInt64()},
      {"last_name", schemac::Type::Varchar(1024)},
      {"first_name", schemac::Type::Varchar(1024)},
      {"gender", schemac::Type::Varchar(1024)},
      {"birthday", schemac::Type::Date()},
      {"location_ip", schemac::Type::Varchar(1024)},
      {"browser_used", schemac::Type::Varchar(1024)},
      {"city_id", schemac::Type::Integer()},
      {"speaks", schemac::Type::Varchar(1024)},
      {"email", schemac::Type::Varchar(1024)}};
   return {3, "KPerson", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_KPerson() { return new KPerson(); }
/*---------------------------------------------------------------------------*/
void destroy_KPerson(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
