#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
struct supplier final : public BaseTable {
   enum Columns { s_suppkey,
                  s_name,
                  s_address,
                  s_city,
                  s_nation,
                  s_region,
                  s_phone };

   struct Container {
      using SingleNodeTreeType = SingleNodeTree<Integer, Char<25>, Varchar<25>, Char<10>, Char<15>, Char<12>, Char<15>>;
      SingleNodeTreeType::ElementTuple tuple;

      decltype(auto) s_suppkey(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) s_name(this auto&& self) { return std::get<1>(self.tuple); }
      decltype(auto) s_address(this auto&& self) { return std::get<2>(self.tuple); }
      decltype(auto) s_city(this auto&& self) { return std::get<3>(self.tuple); }
      decltype(auto) s_nation(this auto&& self) { return std::get<4>(self.tuple); }
      decltype(auto) s_region(this auto&& self) { return std::get<5>(self.tuple); }
      decltype(auto) s_phone(this auto&& self) { return std::get<6>(self.tuple); }

      auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(tuple); }
   };

   std::vector<Container> data;

   ~supplier() override = default;
   std::string_view name() const override { return "supplier"; };
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;
   void genTblFiles(const std::string& tblPath, char separator) const override;

   static schemac::Table genSchema();
};

factDB::BaseTable* create_supplier();
void destroy_supplier(factDB::BaseTable* ptr);
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
