#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
struct date final : public BaseTable {
   enum Columns { d_datekey,
                  d_date,
                  d_dayofweek,
                  d_month,
                  d_year,
                  d_yearmonthnum,
                  d_yearmonth,
                  d_daynuminweek,
                  d_daynuminmonth,
                  d_daynuminyear,
                  d_monthnuminyear,
                  d_weeknuminyear,
                  d_sellingseason,
                  d_lastdayinweekfl,
                  d_lastdayinmonthfl,
                  d_holidayfl,
                  d_weekdayfl };

   struct Container {
      using SingleNodeTreeType = SingleNodeTree<Integer, Char<18>, Char<9>, Char<9>, Integer, Integer, Char<7>, Integer, Integer, Integer, Integer, Integer, Varchar<12>, Integer, Integer, Integer, Integer>;
      SingleNodeTreeType::ElementTuple tuple;

      decltype(auto) d_datekey(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) d_date(this auto&& self) { return std::get<1>(self.tuple); }
      decltype(auto) d_dayofweek(this auto&& self) { return std::get<2>(self.tuple); }
      decltype(auto) d_month(this auto&& self) { return std::get<3>(self.tuple); }
      decltype(auto) d_year(this auto&& self) { return std::get<4>(self.tuple); }
      decltype(auto) d_yearmonthnum(this auto&& self) { return std::get<5>(self.tuple); }
      decltype(auto) d_yearmonth(this auto&& self) { return std::get<6>(self.tuple); }
      decltype(auto) d_daynuminweek(this auto&& self) { return std::get<7>(self.tuple); }
      decltype(auto) d_daynuminmonth(this auto&& self) { return std::get<8>(self.tuple); }
      decltype(auto) d_daynuminyear(this auto&& self) { return std::get<9>(self.tuple); }
      decltype(auto) d_monthnuminyear(this auto&& self) { return std::get<10>(self.tuple); }
      decltype(auto) d_weeknuminyear(this auto&& self) { return std::get<11>(self.tuple); }
      decltype(auto) d_sellingseason(this auto&& self) { return std::get<12>(self.tuple); }
      decltype(auto) d_lastdayinweekfl(this auto&& self) { return std::get<13>(self.tuple); }
      decltype(auto) d_lastdayinmonthfl(this auto&& self) { return std::get<14>(self.tuple); }
      decltype(auto) d_holidayfl(this auto&& self) { return std::get<15>(self.tuple); }
      decltype(auto) d_weekdayfl(this auto&& self) { return std::get<16>(self.tuple); }

      auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(tuple); }
   };

   std::vector<Container> data;

   ~date() override = default;
   std::string_view name() const override { return "date"; };
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;
   void genTblFiles(const std::string& tblPath, char separator) const override;

   static schemac::Table genSchema();
};

factDB::BaseTable* create_date();
void destroy_date(factDB::BaseTable* ptr);
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
