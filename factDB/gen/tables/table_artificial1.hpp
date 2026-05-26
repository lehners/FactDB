#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
struct artificial1 final : public BaseTable {
   enum Columns { a,
                  b,
                  id };

   struct Container {
      using SingleNodeTreeType = SingleNodeTree<Integer, Integer, Integer>;
      SingleNodeTreeType::ElementTuple tuple;

      decltype(auto) a(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) b(this auto&& self) { return std::get<1>(self.tuple); }
      decltype(auto) id(this auto&& self) { return std::get<2>(self.tuple); }

      auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(tuple); }
   };

   std::vector<Container> data;

   ~artificial1() override = default;
   std::string_view name() const override { return "artificial1"; };
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;
   void genTblFiles(const std::string& tblPath, char separator) const override;

   static schemac::Table genSchema();
};

factDB::BaseTable* create_artificial1();
void destroy_artificial1(factDB::BaseTable* ptr);
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
