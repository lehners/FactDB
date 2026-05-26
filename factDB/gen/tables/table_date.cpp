#include "factDB/gen/tables/table_date.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
void date::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.d_datekey(), in, separator, false);
         updateElem(newData.d_date(), in, separator, false);
         updateElem(newData.d_dayofweek(), in, separator, false);
         updateElem(newData.d_month(), in, separator, false);
         updateElem(newData.d_year(), in, separator, false);
         updateElem(newData.d_yearmonthnum(), in, separator, false);
         updateElem(newData.d_yearmonth(), in, separator, false);
         updateElem(newData.d_daynuminweek(), in, separator, false);
         updateElem(newData.d_daynuminmonth(), in, separator, false);
         updateElem(newData.d_daynuminyear(), in, separator, false);
         updateElem(newData.d_monthnuminyear(), in, separator, false);
         updateElem(newData.d_weeknuminyear(), in, separator, false);
         updateElem(newData.d_sellingseason(), in, separator, false);
         updateElem(newData.d_lastdayinweekfl(), in, separator, false);
         updateElem(newData.d_lastdayinmonthfl(), in, separator, false);
         updateElem(newData.d_holidayfl(), in, separator, false);
         updateElem(newData.d_weekdayfl(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void date::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue date::get(size_t idx, const IU& iu) const {
   if ("d_datekey" == iu.column) {
      return data[idx].d_datekey().toRTV();
   } else if ("d_date" == iu.column) {
      return data[idx].d_date().toRTV();
   } else if ("d_dayofweek" == iu.column) {
      return data[idx].d_dayofweek().toRTV();
   } else if ("d_month" == iu.column) {
      return data[idx].d_month().toRTV();
   } else if ("d_year" == iu.column) {
      return data[idx].d_year().toRTV();
   } else if ("d_yearmonthnum" == iu.column) {
      return data[idx].d_yearmonthnum().toRTV();
   } else if ("d_yearmonth" == iu.column) {
      return data[idx].d_yearmonth().toRTV();
   } else if ("d_daynuminweek" == iu.column) {
      return data[idx].d_daynuminweek().toRTV();
   } else if ("d_daynuminmonth" == iu.column) {
      return data[idx].d_daynuminmonth().toRTV();
   } else if ("d_daynuminyear" == iu.column) {
      return data[idx].d_daynuminyear().toRTV();
   } else if ("d_monthnuminyear" == iu.column) {
      return data[idx].d_monthnuminyear().toRTV();
   } else if ("d_weeknuminyear" == iu.column) {
      return data[idx].d_weeknuminyear().toRTV();
   } else if ("d_sellingseason" == iu.column) {
      return data[idx].d_sellingseason().toRTV();
   } else if ("d_lastdayinweekfl" == iu.column) {
      return data[idx].d_lastdayinweekfl().toRTV();
   } else if ("d_lastdayinmonthfl" == iu.column) {
      return data[idx].d_lastdayinmonthfl().toRTV();
   } else if ("d_holidayfl" == iu.column) {
      return data[idx].d_holidayfl().toRTV();
   } else if ("d_weekdayfl" == iu.column) {
      return data[idx].d_weekdayfl().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void date::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<date, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table date::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"d_datekey", schemac::Type::Integer()},
      {"d_date", schemac::Type::Char(18)},
      {"d_dayofweek", schemac::Type::Char(9)},
      {"d_month", schemac::Type::Char(9)},
      {"d_year", schemac::Type::Integer()},
      {"d_yearmonthnum", schemac::Type::Integer()},
      {"d_yearmonth", schemac::Type::Char(7)},
      {"d_daynuminweek", schemac::Type::Integer()},
      {"d_daynuminmonth", schemac::Type::Integer()},
      {"d_daynuminyear", schemac::Type::Integer()},
      {"d_monthnuminyear", schemac::Type::Integer()},
      {"d_weeknuminyear", schemac::Type::Integer()},
      {"d_sellingseason", schemac::Type::Varchar(12)},
      {"d_lastdayinweekfl", schemac::Type::Integer()},
      {"d_lastdayinmonthfl", schemac::Type::Integer()},
      {"d_holidayfl", schemac::Type::Integer()},
      {"d_weekdayfl", schemac::Type::Integer()}};
   return {413, "date", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_date() { return new date(); }
/*---------------------------------------------------------------------------*/
void destroy_date(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
