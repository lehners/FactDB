#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
struct WINF_A final : public BaseTable {
   enum Columns { k_a,
                  a };

   struct Container {
      using SingleNodeTreeType = SingleNodeTree<Integer, Integer>;
      SingleNodeTreeType::ElementTuple tuple;

      decltype(auto) k_a(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) a(this auto&& self) { return std::get<1>(self.tuple); }

      auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(tuple); }
   };

   std::vector<Container> data;

   ~WINF_A() override = default;
   std::string_view name() const override { return "WINF_A"; };
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;
   void genTblFiles(const std::string& tblPath, char separator) const override;

   static schemac::Table genSchema();
};
/*---------------------------------------------------------------------------*/
struct WINF_B final : public BaseTable {
   enum Columns { k_b,
                  b };

   struct Container {
      using SingleNodeTreeType = SingleNodeTree<Integer, Integer>;
      SingleNodeTreeType::ElementTuple tuple;

      decltype(auto) k_b(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) b(this auto&& self) { return std::get<1>(self.tuple); }

      auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(tuple); }
   };

   std::vector<Container> data;

   ~WINF_B() override = default;
   std::string_view name() const override { return "WINF_B"; };
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;
   void genTblFiles(const std::string& tblPath, char separator) const override;

   static schemac::Table genSchema();
};
/*---------------------------------------------------------------------------*/
struct WINF_C final : public BaseTable {
   enum Columns { k_c,
                  c };

   struct Container {
      using SingleNodeTreeType = SingleNodeTree<Integer, Integer>;
      SingleNodeTreeType::ElementTuple tuple;

      decltype(auto) k_c(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) c(this auto&& self) { return std::get<1>(self.tuple); }

      auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(tuple); }
   };

   std::vector<Container> data;

   ~WINF_C() override = default;
   std::string_view name() const override { return "WINF_C"; };
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;
   void genTblFiles(const std::string& tblPath, char separator) const override;

   static schemac::Table genSchema();
};
/*---------------------------------------------------------------------------*/
struct WINF_D final : public BaseTable {
   enum Columns { k_d,
                  d };

   struct Container {
      using SingleNodeTreeType = SingleNodeTree<Integer, Integer>;
      SingleNodeTreeType::ElementTuple tuple;

      decltype(auto) k_d(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) d(this auto&& self) { return std::get<1>(self.tuple); }

      auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(tuple); }
   };

   std::vector<Container> data;

   ~WINF_D() override = default;
   std::string_view name() const override { return "WINF_D"; };
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;
   void genTblFiles(const std::string& tblPath, char separator) const override;

   static schemac::Table genSchema();
};
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_WINF_A();
void destroy_WINF_A(factDB::BaseTable* ptr);
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_WINF_B();
void destroy_WINF_B(factDB::BaseTable* ptr);
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_WINF_C();
void destroy_WINF_C(factDB::BaseTable* ptr);
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_WINF_D();
void destroy_WINF_D(factDB::BaseTable* ptr);
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
