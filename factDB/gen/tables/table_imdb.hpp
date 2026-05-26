#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::tables {
// ---------------------------------------------------------------------------------------------------
struct ImdbBase : public BaseTable {
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

   ~ImdbBase() override = default;
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;

   protected:
   static schemac::Table genSchema(uint32_t id, std::string name);
};
// ---------------------------------------------------------------------------------------------------
struct imdb1 final : public ImdbBase {
   std::string_view name() const override { return "imdb1"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb2 final : public ImdbBase {
   std::string_view name() const override { return "imdb2"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb3 final : public ImdbBase {
   std::string_view name() const override { return "imdb3"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb4 final : public ImdbBase {
   std::string_view name() const override { return "imdb4"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb5 final : public ImdbBase {
   std::string_view name() const override { return "imdb5"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb6 final : public ImdbBase {
   std::string_view name() const override { return "imdb6"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb7 final : public ImdbBase {
   std::string_view name() const override { return "imdb7"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb8 final : public ImdbBase {
   std::string_view name() const override { return "imdb8"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb9 final : public ImdbBase {
   std::string_view name() const override { return "imdb9"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb10 final : public ImdbBase {
   std::string_view name() const override { return "imdb10"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb11 final : public ImdbBase {
   std::string_view name() const override { return "imdb11"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb12 final : public ImdbBase {
   std::string_view name() const override { return "imdb12"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb13 final : public ImdbBase {
   std::string_view name() const override { return "imdb13"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb14 final : public ImdbBase {
   std::string_view name() const override { return "imdb14"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb15 final : public ImdbBase {
   std::string_view name() const override { return "imdb15"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb16 final : public ImdbBase {
   std::string_view name() const override { return "imdb16"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb17 final : public ImdbBase {
   std::string_view name() const override { return "imdb17"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb18 final : public ImdbBase {
   std::string_view name() const override { return "imdb18"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb19 final : public ImdbBase {
   std::string_view name() const override { return "imdb19"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb20 final : public ImdbBase {
   std::string_view name() const override { return "imdb20"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb21 final : public ImdbBase {
   std::string_view name() const override { return "imdb21"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb22 final : public ImdbBase {
   std::string_view name() const override { return "imdb22"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb23 final : public ImdbBase {
   std::string_view name() const override { return "imdb23"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb24 final : public ImdbBase {
   std::string_view name() const override { return "imdb24"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb25 final : public ImdbBase {
   std::string_view name() const override { return "imdb25"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb26 final : public ImdbBase {
   std::string_view name() const override { return "imdb26"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb27 final : public ImdbBase {
   std::string_view name() const override { return "imdb27"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb28 final : public ImdbBase {
   std::string_view name() const override { return "imdb28"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb29 final : public ImdbBase {
   std::string_view name() const override { return "imdb29"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb30 final : public ImdbBase {
   std::string_view name() const override { return "imdb30"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb31 final : public ImdbBase {
   std::string_view name() const override { return "imdb31"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb32 final : public ImdbBase {
   std::string_view name() const override { return "imdb32"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb33 final : public ImdbBase {
   std::string_view name() const override { return "imdb33"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb34 final : public ImdbBase {
   std::string_view name() const override { return "imdb34"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb35 final : public ImdbBase {
   std::string_view name() const override { return "imdb35"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb36 final : public ImdbBase {
   std::string_view name() const override { return "imdb36"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb37 final : public ImdbBase {
   std::string_view name() const override { return "imdb37"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb38 final : public ImdbBase {
   std::string_view name() const override { return "imdb38"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb39 final : public ImdbBase {
   std::string_view name() const override { return "imdb39"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb40 final : public ImdbBase {
   std::string_view name() const override { return "imdb40"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb41 final : public ImdbBase {
   std::string_view name() const override { return "imdb41"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb42 final : public ImdbBase {
   std::string_view name() const override { return "imdb42"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb43 final : public ImdbBase {
   std::string_view name() const override { return "imdb43"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb44 final : public ImdbBase {
   std::string_view name() const override { return "imdb44"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb45 final : public ImdbBase {
   std::string_view name() const override { return "imdb45"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb46 final : public ImdbBase {
   std::string_view name() const override { return "imdb46"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb47 final : public ImdbBase {
   std::string_view name() const override { return "imdb47"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb48 final : public ImdbBase {
   std::string_view name() const override { return "imdb48"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb49 final : public ImdbBase {
   std::string_view name() const override { return "imdb49"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb50 final : public ImdbBase {
   std::string_view name() const override { return "imdb50"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb51 final : public ImdbBase {
   std::string_view name() const override { return "imdb51"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb52 final : public ImdbBase {
   std::string_view name() const override { return "imdb52"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb53 final : public ImdbBase {
   std::string_view name() const override { return "imdb53"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb54 final : public ImdbBase {
   std::string_view name() const override { return "imdb54"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb55 final : public ImdbBase {
   std::string_view name() const override { return "imdb55"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb56 final : public ImdbBase {
   std::string_view name() const override { return "imdb56"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb57 final : public ImdbBase {
   std::string_view name() const override { return "imdb57"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb58 final : public ImdbBase {
   std::string_view name() const override { return "imdb58"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb59 final : public ImdbBase {
   std::string_view name() const override { return "imdb59"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb60 final : public ImdbBase {
   std::string_view name() const override { return "imdb60"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb61 final : public ImdbBase {
   std::string_view name() const override { return "imdb61"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb62 final : public ImdbBase {
   std::string_view name() const override { return "imdb62"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb63 final : public ImdbBase {
   std::string_view name() const override { return "imdb63"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb64 final : public ImdbBase {
   std::string_view name() const override { return "imdb64"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb65 final : public ImdbBase {
   std::string_view name() const override { return "imdb65"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb66 final : public ImdbBase {
   std::string_view name() const override { return "imdb66"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb67 final : public ImdbBase {
   std::string_view name() const override { return "imdb67"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb68 final : public ImdbBase {
   std::string_view name() const override { return "imdb68"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb69 final : public ImdbBase {
   std::string_view name() const override { return "imdb69"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb70 final : public ImdbBase {
   std::string_view name() const override { return "imdb70"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb71 final : public ImdbBase {
   std::string_view name() const override { return "imdb71"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb72 final : public ImdbBase {
   std::string_view name() const override { return "imdb72"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb73 final : public ImdbBase {
   std::string_view name() const override { return "imdb73"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb74 final : public ImdbBase {
   std::string_view name() const override { return "imdb74"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb75 final : public ImdbBase {
   std::string_view name() const override { return "imdb75"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb76 final : public ImdbBase {
   std::string_view name() const override { return "imdb76"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb77 final : public ImdbBase {
   std::string_view name() const override { return "imdb77"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb78 final : public ImdbBase {
   std::string_view name() const override { return "imdb78"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb79 final : public ImdbBase {
   std::string_view name() const override { return "imdb79"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb80 final : public ImdbBase {
   std::string_view name() const override { return "imdb80"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb81 final : public ImdbBase {
   std::string_view name() const override { return "imdb81"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb82 final : public ImdbBase {
   std::string_view name() const override { return "imdb82"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb83 final : public ImdbBase {
   std::string_view name() const override { return "imdb83"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb84 final : public ImdbBase {
   std::string_view name() const override { return "imdb84"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb85 final : public ImdbBase {
   std::string_view name() const override { return "imdb85"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb86 final : public ImdbBase {
   std::string_view name() const override { return "imdb86"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb87 final : public ImdbBase {
   std::string_view name() const override { return "imdb87"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb88 final : public ImdbBase {
   std::string_view name() const override { return "imdb88"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb89 final : public ImdbBase {
   std::string_view name() const override { return "imdb89"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb90 final : public ImdbBase {
   std::string_view name() const override { return "imdb90"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb91 final : public ImdbBase {
   std::string_view name() const override { return "imdb91"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb92 final : public ImdbBase {
   std::string_view name() const override { return "imdb92"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb93 final : public ImdbBase {
   std::string_view name() const override { return "imdb93"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb94 final : public ImdbBase {
   std::string_view name() const override { return "imdb94"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb95 final : public ImdbBase {
   std::string_view name() const override { return "imdb95"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb96 final : public ImdbBase {
   std::string_view name() const override { return "imdb96"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb97 final : public ImdbBase {
   std::string_view name() const override { return "imdb97"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb98 final : public ImdbBase {
   std::string_view name() const override { return "imdb98"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb99 final : public ImdbBase {
   std::string_view name() const override { return "imdb99"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb100 final : public ImdbBase {
   std::string_view name() const override { return "imdb100"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb101 final : public ImdbBase {
   std::string_view name() const override { return "imdb101"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb102 final : public ImdbBase {
   std::string_view name() const override { return "imdb102"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb103 final : public ImdbBase {
   std::string_view name() const override { return "imdb103"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb104 final : public ImdbBase {
   std::string_view name() const override { return "imdb104"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb105 final : public ImdbBase {
   std::string_view name() const override { return "imdb105"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb106 final : public ImdbBase {
   std::string_view name() const override { return "imdb106"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb107 final : public ImdbBase {
   std::string_view name() const override { return "imdb107"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb108 final : public ImdbBase {
   std::string_view name() const override { return "imdb108"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb109 final : public ImdbBase {
   std::string_view name() const override { return "imdb109"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb110 final : public ImdbBase {
   std::string_view name() const override { return "imdb110"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb111 final : public ImdbBase {
   std::string_view name() const override { return "imdb111"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb112 final : public ImdbBase {
   std::string_view name() const override { return "imdb112"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb113 final : public ImdbBase {
   std::string_view name() const override { return "imdb113"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb114 final : public ImdbBase {
   std::string_view name() const override { return "imdb114"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb115 final : public ImdbBase {
   std::string_view name() const override { return "imdb115"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb116 final : public ImdbBase {
   std::string_view name() const override { return "imdb116"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb117 final : public ImdbBase {
   std::string_view name() const override { return "imdb117"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb118 final : public ImdbBase {
   std::string_view name() const override { return "imdb118"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb119 final : public ImdbBase {
   std::string_view name() const override { return "imdb119"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb120 final : public ImdbBase {
   std::string_view name() const override { return "imdb120"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb121 final : public ImdbBase {
   std::string_view name() const override { return "imdb121"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb122 final : public ImdbBase {
   std::string_view name() const override { return "imdb122"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb123 final : public ImdbBase {
   std::string_view name() const override { return "imdb123"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb124 final : public ImdbBase {
   std::string_view name() const override { return "imdb124"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb125 final : public ImdbBase {
   std::string_view name() const override { return "imdb125"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb126 final : public ImdbBase {
   std::string_view name() const override { return "imdb126"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
struct imdb127 final : public ImdbBase {
   std::string_view name() const override { return "imdb127"; }
   void genTblFiles(const std::string& tblPath, char separator) const override;
   static schemac::Table genSchema();
};
// ---------------------------------------------------------------------------------------------------
inline factDB::BaseTable* create_imdb1() { return new imdb1; }
inline factDB::BaseTable* create_imdb2() { return new imdb2; }
inline factDB::BaseTable* create_imdb3() { return new imdb3; }
inline factDB::BaseTable* create_imdb4() { return new imdb4; }
inline factDB::BaseTable* create_imdb5() { return new imdb5; }
inline factDB::BaseTable* create_imdb6() { return new imdb6; }
inline factDB::BaseTable* create_imdb7() { return new imdb7; }
inline factDB::BaseTable* create_imdb8() { return new imdb8; }
inline factDB::BaseTable* create_imdb9() { return new imdb9; }
inline factDB::BaseTable* create_imdb10() { return new imdb10; }
inline factDB::BaseTable* create_imdb11() { return new imdb11; }
inline factDB::BaseTable* create_imdb12() { return new imdb12; }
inline factDB::BaseTable* create_imdb13() { return new imdb13; }
inline factDB::BaseTable* create_imdb14() { return new imdb14; }
inline factDB::BaseTable* create_imdb15() { return new imdb15; }
inline factDB::BaseTable* create_imdb16() { return new imdb16; }
inline factDB::BaseTable* create_imdb17() { return new imdb17; }
inline factDB::BaseTable* create_imdb18() { return new imdb18; }
inline factDB::BaseTable* create_imdb19() { return new imdb19; }
inline factDB::BaseTable* create_imdb20() { return new imdb20; }
inline factDB::BaseTable* create_imdb21() { return new imdb21; }
inline factDB::BaseTable* create_imdb22() { return new imdb22; }
inline factDB::BaseTable* create_imdb23() { return new imdb23; }
inline factDB::BaseTable* create_imdb24() { return new imdb24; }
inline factDB::BaseTable* create_imdb25() { return new imdb25; }
inline factDB::BaseTable* create_imdb26() { return new imdb26; }
inline factDB::BaseTable* create_imdb27() { return new imdb27; }
inline factDB::BaseTable* create_imdb28() { return new imdb28; }
inline factDB::BaseTable* create_imdb29() { return new imdb29; }
inline factDB::BaseTable* create_imdb30() { return new imdb30; }
inline factDB::BaseTable* create_imdb31() { return new imdb31; }
inline factDB::BaseTable* create_imdb32() { return new imdb32; }
inline factDB::BaseTable* create_imdb33() { return new imdb33; }
inline factDB::BaseTable* create_imdb34() { return new imdb34; }
inline factDB::BaseTable* create_imdb35() { return new imdb35; }
inline factDB::BaseTable* create_imdb36() { return new imdb36; }
inline factDB::BaseTable* create_imdb37() { return new imdb37; }
inline factDB::BaseTable* create_imdb38() { return new imdb38; }
inline factDB::BaseTable* create_imdb39() { return new imdb39; }
inline factDB::BaseTable* create_imdb40() { return new imdb40; }
inline factDB::BaseTable* create_imdb41() { return new imdb41; }
inline factDB::BaseTable* create_imdb42() { return new imdb42; }
inline factDB::BaseTable* create_imdb43() { return new imdb43; }
inline factDB::BaseTable* create_imdb44() { return new imdb44; }
inline factDB::BaseTable* create_imdb45() { return new imdb45; }
inline factDB::BaseTable* create_imdb46() { return new imdb46; }
inline factDB::BaseTable* create_imdb47() { return new imdb47; }
inline factDB::BaseTable* create_imdb48() { return new imdb48; }
inline factDB::BaseTable* create_imdb49() { return new imdb49; }
inline factDB::BaseTable* create_imdb50() { return new imdb50; }
inline factDB::BaseTable* create_imdb51() { return new imdb51; }
inline factDB::BaseTable* create_imdb52() { return new imdb52; }
inline factDB::BaseTable* create_imdb53() { return new imdb53; }
inline factDB::BaseTable* create_imdb54() { return new imdb54; }
inline factDB::BaseTable* create_imdb55() { return new imdb55; }
inline factDB::BaseTable* create_imdb56() { return new imdb56; }
inline factDB::BaseTable* create_imdb57() { return new imdb57; }
inline factDB::BaseTable* create_imdb58() { return new imdb58; }
inline factDB::BaseTable* create_imdb59() { return new imdb59; }
inline factDB::BaseTable* create_imdb60() { return new imdb60; }
inline factDB::BaseTable* create_imdb61() { return new imdb61; }
inline factDB::BaseTable* create_imdb62() { return new imdb62; }
inline factDB::BaseTable* create_imdb63() { return new imdb63; }
inline factDB::BaseTable* create_imdb64() { return new imdb64; }
inline factDB::BaseTable* create_imdb65() { return new imdb65; }
inline factDB::BaseTable* create_imdb66() { return new imdb66; }
inline factDB::BaseTable* create_imdb67() { return new imdb67; }
inline factDB::BaseTable* create_imdb68() { return new imdb68; }
inline factDB::BaseTable* create_imdb69() { return new imdb69; }
inline factDB::BaseTable* create_imdb70() { return new imdb70; }
inline factDB::BaseTable* create_imdb71() { return new imdb71; }
inline factDB::BaseTable* create_imdb72() { return new imdb72; }
inline factDB::BaseTable* create_imdb73() { return new imdb73; }
inline factDB::BaseTable* create_imdb74() { return new imdb74; }
inline factDB::BaseTable* create_imdb75() { return new imdb75; }
inline factDB::BaseTable* create_imdb76() { return new imdb76; }
inline factDB::BaseTable* create_imdb77() { return new imdb77; }
inline factDB::BaseTable* create_imdb78() { return new imdb78; }
inline factDB::BaseTable* create_imdb79() { return new imdb79; }
inline factDB::BaseTable* create_imdb80() { return new imdb80; }
inline factDB::BaseTable* create_imdb81() { return new imdb81; }
inline factDB::BaseTable* create_imdb82() { return new imdb82; }
inline factDB::BaseTable* create_imdb83() { return new imdb83; }
inline factDB::BaseTable* create_imdb84() { return new imdb84; }
inline factDB::BaseTable* create_imdb85() { return new imdb85; }
inline factDB::BaseTable* create_imdb86() { return new imdb86; }
inline factDB::BaseTable* create_imdb87() { return new imdb87; }
inline factDB::BaseTable* create_imdb88() { return new imdb88; }
inline factDB::BaseTable* create_imdb89() { return new imdb89; }
inline factDB::BaseTable* create_imdb90() { return new imdb90; }
inline factDB::BaseTable* create_imdb91() { return new imdb91; }
inline factDB::BaseTable* create_imdb92() { return new imdb92; }
inline factDB::BaseTable* create_imdb93() { return new imdb93; }
inline factDB::BaseTable* create_imdb94() { return new imdb94; }
inline factDB::BaseTable* create_imdb95() { return new imdb95; }
inline factDB::BaseTable* create_imdb96() { return new imdb96; }
inline factDB::BaseTable* create_imdb97() { return new imdb97; }
inline factDB::BaseTable* create_imdb98() { return new imdb98; }
inline factDB::BaseTable* create_imdb99() { return new imdb99; }
inline factDB::BaseTable* create_imdb100() { return new imdb100; }
inline factDB::BaseTable* create_imdb101() { return new imdb101; }
inline factDB::BaseTable* create_imdb102() { return new imdb102; }
inline factDB::BaseTable* create_imdb103() { return new imdb103; }
inline factDB::BaseTable* create_imdb104() { return new imdb104; }
inline factDB::BaseTable* create_imdb105() { return new imdb105; }
inline factDB::BaseTable* create_imdb106() { return new imdb106; }
inline factDB::BaseTable* create_imdb107() { return new imdb107; }
inline factDB::BaseTable* create_imdb108() { return new imdb108; }
inline factDB::BaseTable* create_imdb109() { return new imdb109; }
inline factDB::BaseTable* create_imdb110() { return new imdb110; }
inline factDB::BaseTable* create_imdb111() { return new imdb111; }
inline factDB::BaseTable* create_imdb112() { return new imdb112; }
inline factDB::BaseTable* create_imdb113() { return new imdb113; }
inline factDB::BaseTable* create_imdb114() { return new imdb114; }
inline factDB::BaseTable* create_imdb115() { return new imdb115; }
inline factDB::BaseTable* create_imdb116() { return new imdb116; }
inline factDB::BaseTable* create_imdb117() { return new imdb117; }
inline factDB::BaseTable* create_imdb118() { return new imdb118; }
inline factDB::BaseTable* create_imdb119() { return new imdb119; }
inline factDB::BaseTable* create_imdb120() { return new imdb120; }
inline factDB::BaseTable* create_imdb121() { return new imdb121; }
inline factDB::BaseTable* create_imdb122() { return new imdb122; }
inline factDB::BaseTable* create_imdb123() { return new imdb123; }
inline factDB::BaseTable* create_imdb124() { return new imdb124; }
inline factDB::BaseTable* create_imdb125() { return new imdb125; }
inline factDB::BaseTable* create_imdb126() { return new imdb126; }
inline factDB::BaseTable* create_imdb127() { return new imdb127; }
// ---------------------------------------------------------------------------------------------------
inline void destroy_imdb1(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb2(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb3(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb4(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb5(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb6(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb7(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb8(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb9(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb10(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb11(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb12(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb13(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb14(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb15(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb16(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb17(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb18(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb19(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb20(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb21(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb22(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb23(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb24(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb25(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb26(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb27(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb28(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb29(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb30(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb31(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb32(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb33(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb34(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb35(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb36(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb37(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb38(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb39(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb40(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb41(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb42(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb43(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb44(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb45(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb46(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb47(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb48(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb49(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb50(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb51(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb52(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb53(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb54(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb55(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb56(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb57(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb58(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb59(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb60(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb61(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb62(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb63(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb64(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb65(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb66(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb67(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb68(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb69(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb70(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb71(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb72(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb73(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb74(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb75(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb76(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb77(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb78(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb79(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb80(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb81(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb82(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb83(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb84(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb85(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb86(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb87(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb88(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb89(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb90(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb91(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb92(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb93(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb94(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb95(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb96(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb97(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb98(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb99(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb100(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb101(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb102(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb103(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb104(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb105(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb106(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb107(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb108(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb109(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb110(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb111(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb112(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb113(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb114(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb115(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb116(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb117(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb118(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb119(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb120(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb121(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb122(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb123(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb124(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb125(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb126(const factDB::BaseTable* ptr) { delete ptr; }
inline void destroy_imdb127(const factDB::BaseTable* ptr) { delete ptr; }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::tables
// ---------------------------------------------------------------------------------------------------
