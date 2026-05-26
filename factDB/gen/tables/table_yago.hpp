#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::tables {
// ---------------------------------------------------------------------------------------------------
struct YagoBase : public BaseTable {
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

   ~YagoBase() override = default;
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;

   protected:
   static schemac::Table genSchema(uint32_t id, std::string name);
};
// ---------------------------------------------------------------------------------------------------
struct yago0 final : public YagoBase {
   std::string_view name() const override { return "yago0"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago1 final : public YagoBase {
   std::string_view name() const override { return "yago1"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago2 final : public YagoBase {
   std::string_view name() const override { return "yago2"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago3 final : public YagoBase {
   std::string_view name() const override { return "yago3"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago4 final : public YagoBase {
   std::string_view name() const override { return "yago4"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago5 final : public YagoBase {
   std::string_view name() const override { return "yago5"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago6 final : public YagoBase {
   std::string_view name() const override { return "yago6"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago7 final : public YagoBase {
   std::string_view name() const override { return "yago7"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago8 final : public YagoBase {
   std::string_view name() const override { return "yago8"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago9 final : public YagoBase {
   std::string_view name() const override { return "yago9"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago10 final : public YagoBase {
   std::string_view name() const override { return "yago10"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago11 final : public YagoBase {
   std::string_view name() const override { return "yago11"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago12 final : public YagoBase {
   std::string_view name() const override { return "yago12"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago13 final : public YagoBase {
   std::string_view name() const override { return "yago13"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago14 final : public YagoBase {
   std::string_view name() const override { return "yago14"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago15 final : public YagoBase {
   std::string_view name() const override { return "yago15"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago16 final : public YagoBase {
   std::string_view name() const override { return "yago16"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago17 final : public YagoBase {
   std::string_view name() const override { return "yago17"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago18 final : public YagoBase {
   std::string_view name() const override { return "yago18"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago19 final : public YagoBase {
   std::string_view name() const override { return "yago19"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago20 final : public YagoBase {
   std::string_view name() const override { return "yago20"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago21 final : public YagoBase {
   std::string_view name() const override { return "yago21"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago22 final : public YagoBase {
   std::string_view name() const override { return "yago22"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago23 final : public YagoBase {
   std::string_view name() const override { return "yago23"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago24 final : public YagoBase {
   std::string_view name() const override { return "yago24"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago25 final : public YagoBase {
   std::string_view name() const override { return "yago25"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago26 final : public YagoBase {
   std::string_view name() const override { return "yago26"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago27 final : public YagoBase {
   std::string_view name() const override { return "yago27"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago28 final : public YagoBase {
   std::string_view name() const override { return "yago28"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago29 final : public YagoBase {
   std::string_view name() const override { return "yago29"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago30 final : public YagoBase {
   std::string_view name() const override { return "yago30"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago31 final : public YagoBase {
   std::string_view name() const override { return "yago31"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago32 final : public YagoBase {
   std::string_view name() const override { return "yago32"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago33 final : public YagoBase {
   std::string_view name() const override { return "yago33"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago34 final : public YagoBase {
   std::string_view name() const override { return "yago34"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago35 final : public YagoBase {
   std::string_view name() const override { return "yago35"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago36 final : public YagoBase {
   std::string_view name() const override { return "yago36"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago37 final : public YagoBase {
   std::string_view name() const override { return "yago37"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago38 final : public YagoBase {
   std::string_view name() const override { return "yago38"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago39 final : public YagoBase {
   std::string_view name() const override { return "yago39"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago40 final : public YagoBase {
   std::string_view name() const override { return "yago40"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago41 final : public YagoBase {
   std::string_view name() const override { return "yago41"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago42 final : public YagoBase {
   std::string_view name() const override { return "yago42"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago43 final : public YagoBase {
   std::string_view name() const override { return "yago43"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago44 final : public YagoBase {
   std::string_view name() const override { return "yago44"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago45 final : public YagoBase {
   std::string_view name() const override { return "yago45"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago46 final : public YagoBase {
   std::string_view name() const override { return "yago46"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago47 final : public YagoBase {
   std::string_view name() const override { return "yago47"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago48 final : public YagoBase {
   std::string_view name() const override { return "yago48"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago49 final : public YagoBase {
   std::string_view name() const override { return "yago49"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago50 final : public YagoBase {
   std::string_view name() const override { return "yago50"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago51 final : public YagoBase {
   std::string_view name() const override { return "yago51"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago52 final : public YagoBase {
   std::string_view name() const override { return "yago52"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago53 final : public YagoBase {
   std::string_view name() const override { return "yago53"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago54 final : public YagoBase {
   std::string_view name() const override { return "yago54"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago55 final : public YagoBase {
   std::string_view name() const override { return "yago55"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago56 final : public YagoBase {
   std::string_view name() const override { return "yago56"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago57 final : public YagoBase {
   std::string_view name() const override { return "yago57"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago58 final : public YagoBase {
   std::string_view name() const override { return "yago58"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago59 final : public YagoBase {
   std::string_view name() const override { return "yago59"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago60 final : public YagoBase {
   std::string_view name() const override { return "yago60"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago61 final : public YagoBase {
   std::string_view name() const override { return "yago61"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago62 final : public YagoBase {
   std::string_view name() const override { return "yago62"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago63 final : public YagoBase {
   std::string_view name() const override { return "yago63"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago64 final : public YagoBase {
   std::string_view name() const override { return "yago64"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago65 final : public YagoBase {
   std::string_view name() const override { return "yago65"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago66 final : public YagoBase {
   std::string_view name() const override { return "yago66"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago67 final : public YagoBase {
   std::string_view name() const override { return "yago67"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago68 final : public YagoBase {
   std::string_view name() const override { return "yago68"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago69 final : public YagoBase {
   std::string_view name() const override { return "yago69"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago70 final : public YagoBase {
   std::string_view name() const override { return "yago70"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago71 final : public YagoBase {
   std::string_view name() const override { return "yago71"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago72 final : public YagoBase {
   std::string_view name() const override { return "yago72"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago73 final : public YagoBase {
   std::string_view name() const override { return "yago73"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago74 final : public YagoBase {
   std::string_view name() const override { return "yago74"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago75 final : public YagoBase {
   std::string_view name() const override { return "yago75"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago76 final : public YagoBase {
   std::string_view name() const override { return "yago76"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago77 final : public YagoBase {
   std::string_view name() const override { return "yago77"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago78 final : public YagoBase {
   std::string_view name() const override { return "yago78"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago79 final : public YagoBase {
   std::string_view name() const override { return "yago79"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago80 final : public YagoBase {
   std::string_view name() const override { return "yago80"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago81 final : public YagoBase {
   std::string_view name() const override { return "yago81"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago82 final : public YagoBase {
   std::string_view name() const override { return "yago82"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago83 final : public YagoBase {
   std::string_view name() const override { return "yago83"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago84 final : public YagoBase {
   std::string_view name() const override { return "yago84"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago85 final : public YagoBase {
   std::string_view name() const override { return "yago85"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago86 final : public YagoBase {
   std::string_view name() const override { return "yago86"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago87 final : public YagoBase {
   std::string_view name() const override { return "yago87"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago88 final : public YagoBase {
   std::string_view name() const override { return "yago88"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago89 final : public YagoBase {
   std::string_view name() const override { return "yago89"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct yago90 final : public YagoBase {
   std::string_view name() const override { return "yago90"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
inline factDB::BaseTable* create_yago0() { return new yago0; }
inline factDB::BaseTable* create_yago1() { return new yago1; }
inline factDB::BaseTable* create_yago2() { return new yago2; }
inline factDB::BaseTable* create_yago3() { return new yago3; }
inline factDB::BaseTable* create_yago4() { return new yago4; }
inline factDB::BaseTable* create_yago5() { return new yago5; }
inline factDB::BaseTable* create_yago6() { return new yago6; }
inline factDB::BaseTable* create_yago7() { return new yago7; }
inline factDB::BaseTable* create_yago8() { return new yago8; }
inline factDB::BaseTable* create_yago9() { return new yago9; }
inline factDB::BaseTable* create_yago10() { return new yago10; }
inline factDB::BaseTable* create_yago11() { return new yago11; }
inline factDB::BaseTable* create_yago12() { return new yago12; }
inline factDB::BaseTable* create_yago13() { return new yago13; }
inline factDB::BaseTable* create_yago14() { return new yago14; }
inline factDB::BaseTable* create_yago15() { return new yago15; }
inline factDB::BaseTable* create_yago16() { return new yago16; }
inline factDB::BaseTable* create_yago17() { return new yago17; }
inline factDB::BaseTable* create_yago18() { return new yago18; }
inline factDB::BaseTable* create_yago19() { return new yago19; }
inline factDB::BaseTable* create_yago20() { return new yago20; }
inline factDB::BaseTable* create_yago21() { return new yago21; }
inline factDB::BaseTable* create_yago22() { return new yago22; }
inline factDB::BaseTable* create_yago23() { return new yago23; }
inline factDB::BaseTable* create_yago24() { return new yago24; }
inline factDB::BaseTable* create_yago25() { return new yago25; }
inline factDB::BaseTable* create_yago26() { return new yago26; }
inline factDB::BaseTable* create_yago27() { return new yago27; }
inline factDB::BaseTable* create_yago28() { return new yago28; }
inline factDB::BaseTable* create_yago29() { return new yago29; }
inline factDB::BaseTable* create_yago30() { return new yago30; }
inline factDB::BaseTable* create_yago31() { return new yago31; }
inline factDB::BaseTable* create_yago32() { return new yago32; }
inline factDB::BaseTable* create_yago33() { return new yago33; }
inline factDB::BaseTable* create_yago34() { return new yago34; }
inline factDB::BaseTable* create_yago35() { return new yago35; }
inline factDB::BaseTable* create_yago36() { return new yago36; }
inline factDB::BaseTable* create_yago37() { return new yago37; }
inline factDB::BaseTable* create_yago38() { return new yago38; }
inline factDB::BaseTable* create_yago39() { return new yago39; }
inline factDB::BaseTable* create_yago40() { return new yago40; }
inline factDB::BaseTable* create_yago41() { return new yago41; }
inline factDB::BaseTable* create_yago42() { return new yago42; }
inline factDB::BaseTable* create_yago43() { return new yago43; }
inline factDB::BaseTable* create_yago44() { return new yago44; }
inline factDB::BaseTable* create_yago45() { return new yago45; }
inline factDB::BaseTable* create_yago46() { return new yago46; }
inline factDB::BaseTable* create_yago47() { return new yago47; }
inline factDB::BaseTable* create_yago48() { return new yago48; }
inline factDB::BaseTable* create_yago49() { return new yago49; }
inline factDB::BaseTable* create_yago50() { return new yago50; }
inline factDB::BaseTable* create_yago51() { return new yago51; }
inline factDB::BaseTable* create_yago52() { return new yago52; }
inline factDB::BaseTable* create_yago53() { return new yago53; }
inline factDB::BaseTable* create_yago54() { return new yago54; }
inline factDB::BaseTable* create_yago55() { return new yago55; }
inline factDB::BaseTable* create_yago56() { return new yago56; }
inline factDB::BaseTable* create_yago57() { return new yago57; }
inline factDB::BaseTable* create_yago58() { return new yago58; }
inline factDB::BaseTable* create_yago59() { return new yago59; }
inline factDB::BaseTable* create_yago60() { return new yago60; }
inline factDB::BaseTable* create_yago61() { return new yago61; }
inline factDB::BaseTable* create_yago62() { return new yago62; }
inline factDB::BaseTable* create_yago63() { return new yago63; }
inline factDB::BaseTable* create_yago64() { return new yago64; }
inline factDB::BaseTable* create_yago65() { return new yago65; }
inline factDB::BaseTable* create_yago66() { return new yago66; }
inline factDB::BaseTable* create_yago67() { return new yago67; }
inline factDB::BaseTable* create_yago68() { return new yago68; }
inline factDB::BaseTable* create_yago69() { return new yago69; }
inline factDB::BaseTable* create_yago70() { return new yago70; }
inline factDB::BaseTable* create_yago71() { return new yago71; }
inline factDB::BaseTable* create_yago72() { return new yago72; }
inline factDB::BaseTable* create_yago73() { return new yago73; }
inline factDB::BaseTable* create_yago74() { return new yago74; }
inline factDB::BaseTable* create_yago75() { return new yago75; }
inline factDB::BaseTable* create_yago76() { return new yago76; }
inline factDB::BaseTable* create_yago77() { return new yago77; }
inline factDB::BaseTable* create_yago78() { return new yago78; }
inline factDB::BaseTable* create_yago79() { return new yago79; }
inline factDB::BaseTable* create_yago80() { return new yago80; }
inline factDB::BaseTable* create_yago81() { return new yago81; }
inline factDB::BaseTable* create_yago82() { return new yago82; }
inline factDB::BaseTable* create_yago83() { return new yago83; }
inline factDB::BaseTable* create_yago84() { return new yago84; }
inline factDB::BaseTable* create_yago85() { return new yago85; }
inline factDB::BaseTable* create_yago86() { return new yago86; }
inline factDB::BaseTable* create_yago87() { return new yago87; }
inline factDB::BaseTable* create_yago88() { return new yago88; }
inline factDB::BaseTable* create_yago89() { return new yago89; }
inline factDB::BaseTable* create_yago90() { return new yago90; }
// ---------------------------------------------------------------------------------------------------
inline void destroy_yago0(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago1(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago2(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago3(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago4(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago5(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago6(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago7(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago8(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago9(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago10(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago11(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago12(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago13(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago14(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago15(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago16(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago17(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago18(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago19(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago20(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago21(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago22(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago23(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago24(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago25(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago26(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago27(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago28(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago29(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago30(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago31(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago32(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago33(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago34(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago35(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago36(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago37(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago38(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago39(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago40(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago41(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago42(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago43(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago44(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago45(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago46(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago47(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago48(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago49(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago50(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago51(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago52(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago53(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago54(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago55(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago56(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago57(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago58(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago59(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago60(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago61(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago62(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago63(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago64(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago65(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago66(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago67(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago68(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago69(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago70(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago71(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago72(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago73(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago74(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago75(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago76(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago77(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago78(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago79(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago80(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago81(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago82(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago83(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago84(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago85(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago86(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago87(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago88(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago89(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_yago90(const factDB::BaseTable* ptr) { delete ptr; }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::tables
// ---------------------------------------------------------------------------------------------------