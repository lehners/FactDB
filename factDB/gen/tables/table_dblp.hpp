#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::tables {
// ---------------------------------------------------------------------------------------------------
struct DBLPBase : public BaseTable {
   enum Columns { s,
                  d };

   struct Container {
      using SingleNodeTreeType = SingleNodeTree<Integer, Integer>;
      SingleNodeTreeType::ElementTuple tuple;
      decltype(auto) s(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) d(this auto&& self) { return std::get<1>(self.tuple); }

      auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(tuple); }
   };

   std::vector<Container> data;

   ~DBLPBase() override = default;
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;

   protected:
   static schemac::Table genSchema(uint32_t id, std::string name);
};
// ---------------------------------------------------------------------------------------------------
struct dblp1 final : public DBLPBase {
   std::string_view name() const override { return "dblp1"; };
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp2 final : public DBLPBase {
   std::string_view name() const override { return "dblp2"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp3 final : public DBLPBase {
   std::string_view name() const override { return "dblp3"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp4 final : public DBLPBase {
   std::string_view name() const override { return "dblp4"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp5 final : public DBLPBase {
   std::string_view name() const override { return "dblp5"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp6 final : public DBLPBase {
   std::string_view name() const override { return "dblp6"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp7 final : public DBLPBase {
   std::string_view name() const override { return "dblp7"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp8 final : public DBLPBase {
   std::string_view name() const override { return "dblp8"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp9 final : public DBLPBase {
   std::string_view name() const override { return "dblp9"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp10 final : public DBLPBase {
   std::string_view name() const override { return "dblp10"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp11 final : public DBLPBase {
   std::string_view name() const override { return "dblp11"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp12 final : public DBLPBase {
   std::string_view name() const override { return "dblp12"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp13 final : public DBLPBase {
   std::string_view name() const override { return "dblp13"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp14 final : public DBLPBase {
   std::string_view name() const override { return "dblp14"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp15 final : public DBLPBase {
   std::string_view name() const override { return "dblp15"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp16 final : public DBLPBase {
   std::string_view name() const override { return "dblp16"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp17 final : public DBLPBase {
   std::string_view name() const override { return "dblp17"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp18 final : public DBLPBase {
   std::string_view name() const override { return "dblp18"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp19 final : public DBLPBase {
   std::string_view name() const override { return "dblp19"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp20 final : public DBLPBase {
   std::string_view name() const override { return "dblp20"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp21 final : public DBLPBase {
   std::string_view name() const override { return "dblp21"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp22 final : public DBLPBase {
   std::string_view name() const override { return "dblp22"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp23 final : public DBLPBase {
   std::string_view name() const override { return "dblp23"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp24 final : public DBLPBase {
   std::string_view name() const override { return "dblp24"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp25 final : public DBLPBase {
   std::string_view name() const override { return "dblp25"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp26 final : public DBLPBase {
   std::string_view name() const override { return "dblp26"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct dblp27 final : public DBLPBase {
   std::string_view name() const override { return "dblp27"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
inline factDB::BaseTable* create_dblp1() { return new dblp1; }
inline factDB::BaseTable* create_dblp2() { return new dblp2; }
inline factDB::BaseTable* create_dblp3() { return new dblp3; }
inline factDB::BaseTable* create_dblp4() { return new dblp4; }
inline factDB::BaseTable* create_dblp5() { return new dblp5; }
inline factDB::BaseTable* create_dblp6() { return new dblp6; }
inline factDB::BaseTable* create_dblp7() { return new dblp7; }
inline factDB::BaseTable* create_dblp8() { return new dblp8; }
inline factDB::BaseTable* create_dblp9() { return new dblp9; }
inline factDB::BaseTable* create_dblp10() { return new dblp10; }
inline factDB::BaseTable* create_dblp11() { return new dblp11; }
inline factDB::BaseTable* create_dblp12() { return new dblp12; }
inline factDB::BaseTable* create_dblp13() { return new dblp13; }
inline factDB::BaseTable* create_dblp14() { return new dblp14; }
inline factDB::BaseTable* create_dblp15() { return new dblp15; }
inline factDB::BaseTable* create_dblp16() { return new dblp16; }
inline factDB::BaseTable* create_dblp17() { return new dblp17; }
inline factDB::BaseTable* create_dblp18() { return new dblp18; }
inline factDB::BaseTable* create_dblp19() { return new dblp19; }
inline factDB::BaseTable* create_dblp20() { return new dblp20; }
inline factDB::BaseTable* create_dblp21() { return new dblp21; }
inline factDB::BaseTable* create_dblp22() { return new dblp22; }
inline factDB::BaseTable* create_dblp23() { return new dblp23; }
inline factDB::BaseTable* create_dblp24() { return new dblp24; }
inline factDB::BaseTable* create_dblp25() { return new dblp25; }
inline factDB::BaseTable* create_dblp26() { return new dblp26; }
inline factDB::BaseTable* create_dblp27() { return new dblp27; }
// ---------------------------------------------------------------------------------------------------
inline void destroy_dblp1(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp2(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp3(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp4(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp5(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp6(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp7(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp8(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp9(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp10(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp11(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp12(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp13(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp14(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp15(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp16(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp17(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp18(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp19(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp20(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp21(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp22(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp23(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp24(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp25(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp26(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_dblp27(const factDB::BaseTable* ptr) { delete ptr; }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::tables
// ---------------------------------------------------------------------------------------------------
