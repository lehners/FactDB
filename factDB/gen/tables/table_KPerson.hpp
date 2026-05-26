#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
struct KPerson final : public BaseTable {
   enum Columns { creationDate,
                  idx,
                  last_name,
                  first_name,
                  gender,
                  birthday,
                  location_ip,
                  browser_used,
                  city_id,
                  speaks,
                  email };

   struct Container {
      using SingleNodeTreeType = SingleNodeTree<Timestamp, UInt64, Varchar<1024>, Varchar<1024>, Varchar<1024>, Date, Varchar<1024>, Varchar<1024>, Integer, Varchar<1024>, Varchar<1024>>;
      SingleNodeTreeType::ElementTuple tuple;

      decltype(auto) creationDate(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) idx(this auto&& self) { return std::get<1>(self.tuple); }
      decltype(auto) last_name(this auto&& self) { return std::get<2>(self.tuple); }
      decltype(auto) first_name(this auto&& self) { return std::get<3>(self.tuple); }
      decltype(auto) gender(this auto&& self) { return std::get<4>(self.tuple); }
      decltype(auto) birthday(this auto&& self) { return std::get<5>(self.tuple); }
      decltype(auto) location_ip(this auto&& self) { return std::get<6>(self.tuple); }
      decltype(auto) browser_used(this auto&& self) { return std::get<7>(self.tuple); }
      decltype(auto) city_id(this auto&& self) { return std::get<8>(self.tuple); }
      decltype(auto) speaks(this auto&& self) { return std::get<9>(self.tuple); }
      decltype(auto) email(this auto&& self) { return std::get<10>(self.tuple); }

      auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(tuple); }
   };

   std::vector<Container> data;

   ~KPerson() override = default;
   std::string_view name() const override { return "KPerson"; };
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;
   void genTblFiles(const std::string& tblPath, char separator) const override;

   static schemac::Table genSchema();
};

factDB::BaseTable* create_KPerson();
void destroy_KPerson(factDB::BaseTable* ptr);
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
