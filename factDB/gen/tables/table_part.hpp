#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
struct part final : public BaseTable {
   enum Columns { p_partkey,
                  p_name,
                  p_mfgr,
                  p_category,
                  p_brand1,
                  p_color,
                  p_type,
                  p_size,
                  p_container };

   struct Container {
      using SingleNodeTreeType = SingleNodeTree<Integer, Varchar<22>, Char<6>, Char<7>, Char<9>, Varchar<11>, Varchar<25>, Integer, Char<10>>;
      SingleNodeTreeType::ElementTuple tuple;

      decltype(auto) p_partkey(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) p_name(this auto&& self) { return std::get<1>(self.tuple); }
      decltype(auto) p_mfgr(this auto&& self) { return std::get<2>(self.tuple); }
      decltype(auto) p_category(this auto&& self) { return std::get<3>(self.tuple); }
      decltype(auto) p_brand1(this auto&& self) { return std::get<4>(self.tuple); }
      decltype(auto) p_color(this auto&& self) { return std::get<5>(self.tuple); }
      decltype(auto) p_type(this auto&& self) { return std::get<6>(self.tuple); }
      decltype(auto) p_size(this auto&& self) { return std::get<7>(self.tuple); }
      decltype(auto) p_container(this auto&& self) { return std::get<8>(self.tuple); }

      auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(tuple); }
   };

   std::vector<Container> data;

   ~part() override = default;
   std::string_view name() const override { return "part"; };
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;
   void genTblFiles(const std::string& tblPath, char separator) const override;

   static schemac::Table genSchema();
};

factDB::BaseTable* create_part();
void destroy_part(factDB::BaseTable* ptr);
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
