#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
struct lineorder final : public BaseTable {
   enum Columns { lo_orderkey,
                  lo_linenumber,
                  lo_custkey,
                  lo_partkey,
                  lo_suppkey,
                  lo_orderdate,
                  lo_orderpriority,
                  lo_shippriority,
                  lo_quantity,
                  lo_extendedprice,
                  lo_ordtotalprice,
                  lo_discount,
                  lo_revenue,
                  lo_supplycost,
                  lo_tax,
                  lo_commitdate,
                  lo_shipmode };

   struct Container {
      using SingleNodeTreeType = SingleNodeTree<Integer, Integer, Integer, Integer, Integer, Integer, Char<15>, Char<1>, Integer, Numeric18_2, Numeric18_2, Integer, Numeric18_2, Numeric18_2, Integer, Integer, Char<10>>;
      SingleNodeTreeType::ElementTuple tuple;

      decltype(auto) lo_orderkey(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) lo_linenumber(this auto&& self) { return std::get<1>(self.tuple); }
      decltype(auto) lo_custkey(this auto&& self) { return std::get<2>(self.tuple); }
      decltype(auto) lo_partkey(this auto&& self) { return std::get<3>(self.tuple); }
      decltype(auto) lo_suppkey(this auto&& self) { return std::get<4>(self.tuple); }
      decltype(auto) lo_orderdate(this auto&& self) { return std::get<5>(self.tuple); }
      decltype(auto) lo_orderpriority(this auto&& self) { return std::get<6>(self.tuple); }
      decltype(auto) lo_shippriority(this auto&& self) { return std::get<7>(self.tuple); }
      decltype(auto) lo_quantity(this auto&& self) { return std::get<8>(self.tuple); }
      decltype(auto) lo_extendedprice(this auto&& self) { return std::get<9>(self.tuple); }
      decltype(auto) lo_ordtotalprice(this auto&& self) { return std::get<10>(self.tuple); }
      decltype(auto) lo_discount(this auto&& self) { return std::get<11>(self.tuple); }
      decltype(auto) lo_revenue(this auto&& self) { return std::get<12>(self.tuple); }
      decltype(auto) lo_supplycost(this auto&& self) { return std::get<13>(self.tuple); }
      decltype(auto) lo_tax(this auto&& self) { return std::get<14>(self.tuple); }
      decltype(auto) lo_commitdate(this auto&& self) { return std::get<15>(self.tuple); }
      decltype(auto) lo_shipmode(this auto&& self) { return std::get<16>(self.tuple); }

      auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(tuple); }
   };

   std::vector<Container> data;

   ~lineorder() override = default;
   std::string_view name() const override { return "lineorder"; };
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;
   void genTblFiles(const std::string& tblPath, char separator) const override;

   static schemac::Table genSchema();
};

factDB::BaseTable* create_lineorder();
void destroy_lineorder(factDB::BaseTable* ptr);
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
