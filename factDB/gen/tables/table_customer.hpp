#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
struct customer final : public BaseTable {
   enum Columns { c_custkey,
                  c_name,
                  c_address,
                  c_city,
                  c_nation,
                  c_region,
                  c_phone,
                  c_mktsegment };

   struct Container {
      using SingleNodeTreeType = SingleNodeTree<Integer, Varchar<25>, Varchar<25>, Char<10>, Char<15>, Char<12>, Char<15>, Char<10>>;
      SingleNodeTreeType::ElementTuple tuple;

      decltype(auto) a(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) b(this auto&& self) { return std::get<1>(self.tuple); }
      decltype(auto) c_custkey(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) c_name(this auto&& self) { return std::get<1>(self.tuple); }
      decltype(auto) c_address(this auto&& self) { return std::get<2>(self.tuple); }
      decltype(auto) c_city(this auto&& self) { return std::get<3>(self.tuple); }
      decltype(auto) c_nation(this auto&& self) { return std::get<4>(self.tuple); }
      decltype(auto) c_region(this auto&& self) { return std::get<5>(self.tuple); }
      decltype(auto) c_phone(this auto&& self) { return std::get<6>(self.tuple); }
      decltype(auto) c_mktsegment(this auto&& self) { return std::get<7>(self.tuple); }

      auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(tuple); }
   };

   std::vector<Container> data;

   ~customer() override = default;
   std::string_view name() const override { return "customer"; };
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;
   void genTblFiles(const std::string& tblPath, char separator) const override;

   static schemac::Table genSchema();
};

factDB::BaseTable* create_customer();
void destroy_customer(factDB::BaseTable* ptr);
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
