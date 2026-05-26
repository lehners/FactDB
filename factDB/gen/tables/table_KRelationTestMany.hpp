#pragma once
/*---------------------------------------------------------------------------*/
#include "factDB/newftree/SingleNodeTree.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/schemac/Table.hpp"
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
struct KRelationTestMany final : public BaseTable {
   enum Columns { iu0,
                  iu1,
                  iu2,
                  iu3,
                  iu4,
                  iu5,
                  iu6,
                  iu7,
                  iu8,
                  iu9,
                  iu10,
                  iu11,
                  iu12,
                  iu13,
                  iu14,
                  iu15,
                  iu16,
                  iu17,
                  iu18,
                  iu19,
                  iu20,
                  iu21,
                  iu22,
                  iu23,
                  iu24,
                  iu25,
                  iu26,
                  iu27,
                  iu28,
                  iu29,
                  iu30,
                  iu31,
                  iu32,
                  iu33,
                  iu34,
                  iu35,
                  iu36,
                  iu37,
                  iu38,
                  iu39,
                  iu40,
                  iu41,
                  iu42,
                  iu43,
                  iu44,
                  iu45,
                  iu46,
                  iu47,
                  iu48,
                  iu49,
                  iu50,
                  iu51,
                  iu52,
                  iu53,
                  iu54,
                  iu55,
                  iu56,
                  iu57,
                  iu58,
                  iu59,
                  iu60,
                  iu61,
                  iu62,
                  iu63,
                  iu64,
                  iu65,
                  iu66,
                  iu67,
                  iu68,
                  iu69,
                  iu70,
                  iu71,
                  iu72,
                  iu73,
                  iu74,
                  iu75,
                  iu76,
                  iu77,
                  iu78,
                  iu79,
                  iu80,
                  iu81,
                  iu82,
                  iu83,
                  iu84,
                  iu85,
                  iu86,
                  iu87,
                  iu88,
                  iu89,
                  iu90,
                  iu91,
                  iu92,
                  iu93,
                  iu94,
                  iu95,
                  iu96,
                  iu97,
                  iu98,
                  iu99 };

   struct Container {
      using SingleNodeTreeType = SingleNodeTree<Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer, Integer>;
      SingleNodeTreeType::ElementTuple tuple;

      decltype(auto) iu0(this auto&& self) { return std::get<0>(self.tuple); }
      decltype(auto) iu1(this auto&& self) { return std::get<1>(self.tuple); }
      decltype(auto) iu2(this auto&& self) { return std::get<2>(self.tuple); }
      decltype(auto) iu3(this auto&& self) { return std::get<3>(self.tuple); }
      decltype(auto) iu4(this auto&& self) { return std::get<4>(self.tuple); }
      decltype(auto) iu5(this auto&& self) { return std::get<5>(self.tuple); }
      decltype(auto) iu6(this auto&& self) { return std::get<6>(self.tuple); }
      decltype(auto) iu7(this auto&& self) { return std::get<7>(self.tuple); }
      decltype(auto) iu8(this auto&& self) { return std::get<8>(self.tuple); }
      decltype(auto) iu9(this auto&& self) { return std::get<9>(self.tuple); }
      decltype(auto) iu10(this auto&& self) { return std::get<10>(self.tuple); }
      decltype(auto) iu11(this auto&& self) { return std::get<11>(self.tuple); }
      decltype(auto) iu12(this auto&& self) { return std::get<12>(self.tuple); }
      decltype(auto) iu13(this auto&& self) { return std::get<13>(self.tuple); }
      decltype(auto) iu14(this auto&& self) { return std::get<14>(self.tuple); }
      decltype(auto) iu15(this auto&& self) { return std::get<15>(self.tuple); }
      decltype(auto) iu16(this auto&& self) { return std::get<16>(self.tuple); }
      decltype(auto) iu17(this auto&& self) { return std::get<17>(self.tuple); }
      decltype(auto) iu18(this auto&& self) { return std::get<18>(self.tuple); }
      decltype(auto) iu19(this auto&& self) { return std::get<19>(self.tuple); }
      decltype(auto) iu20(this auto&& self) { return std::get<20>(self.tuple); }
      decltype(auto) iu21(this auto&& self) { return std::get<21>(self.tuple); }
      decltype(auto) iu22(this auto&& self) { return std::get<22>(self.tuple); }
      decltype(auto) iu23(this auto&& self) { return std::get<23>(self.tuple); }
      decltype(auto) iu24(this auto&& self) { return std::get<24>(self.tuple); }
      decltype(auto) iu25(this auto&& self) { return std::get<25>(self.tuple); }
      decltype(auto) iu26(this auto&& self) { return std::get<26>(self.tuple); }
      decltype(auto) iu27(this auto&& self) { return std::get<27>(self.tuple); }
      decltype(auto) iu28(this auto&& self) { return std::get<28>(self.tuple); }
      decltype(auto) iu29(this auto&& self) { return std::get<29>(self.tuple); }
      decltype(auto) iu30(this auto&& self) { return std::get<30>(self.tuple); }
      decltype(auto) iu31(this auto&& self) { return std::get<31>(self.tuple); }
      decltype(auto) iu32(this auto&& self) { return std::get<32>(self.tuple); }
      decltype(auto) iu33(this auto&& self) { return std::get<33>(self.tuple); }
      decltype(auto) iu34(this auto&& self) { return std::get<34>(self.tuple); }
      decltype(auto) iu35(this auto&& self) { return std::get<35>(self.tuple); }
      decltype(auto) iu36(this auto&& self) { return std::get<36>(self.tuple); }
      decltype(auto) iu37(this auto&& self) { return std::get<37>(self.tuple); }
      decltype(auto) iu38(this auto&& self) { return std::get<38>(self.tuple); }
      decltype(auto) iu39(this auto&& self) { return std::get<39>(self.tuple); }
      decltype(auto) iu40(this auto&& self) { return std::get<40>(self.tuple); }
      decltype(auto) iu41(this auto&& self) { return std::get<41>(self.tuple); }
      decltype(auto) iu42(this auto&& self) { return std::get<42>(self.tuple); }
      decltype(auto) iu43(this auto&& self) { return std::get<43>(self.tuple); }
      decltype(auto) iu44(this auto&& self) { return std::get<44>(self.tuple); }
      decltype(auto) iu45(this auto&& self) { return std::get<45>(self.tuple); }
      decltype(auto) iu46(this auto&& self) { return std::get<46>(self.tuple); }
      decltype(auto) iu47(this auto&& self) { return std::get<47>(self.tuple); }
      decltype(auto) iu48(this auto&& self) { return std::get<48>(self.tuple); }
      decltype(auto) iu49(this auto&& self) { return std::get<49>(self.tuple); }
      decltype(auto) iu50(this auto&& self) { return std::get<50>(self.tuple); }
      decltype(auto) iu51(this auto&& self) { return std::get<51>(self.tuple); }
      decltype(auto) iu52(this auto&& self) { return std::get<52>(self.tuple); }
      decltype(auto) iu53(this auto&& self) { return std::get<53>(self.tuple); }
      decltype(auto) iu54(this auto&& self) { return std::get<54>(self.tuple); }
      decltype(auto) iu55(this auto&& self) { return std::get<55>(self.tuple); }
      decltype(auto) iu56(this auto&& self) { return std::get<56>(self.tuple); }
      decltype(auto) iu57(this auto&& self) { return std::get<57>(self.tuple); }
      decltype(auto) iu58(this auto&& self) { return std::get<58>(self.tuple); }
      decltype(auto) iu59(this auto&& self) { return std::get<59>(self.tuple); }
      decltype(auto) iu60(this auto&& self) { return std::get<60>(self.tuple); }
      decltype(auto) iu61(this auto&& self) { return std::get<61>(self.tuple); }
      decltype(auto) iu62(this auto&& self) { return std::get<62>(self.tuple); }
      decltype(auto) iu63(this auto&& self) { return std::get<63>(self.tuple); }
      decltype(auto) iu64(this auto&& self) { return std::get<64>(self.tuple); }
      decltype(auto) iu65(this auto&& self) { return std::get<65>(self.tuple); }
      decltype(auto) iu66(this auto&& self) { return std::get<66>(self.tuple); }
      decltype(auto) iu67(this auto&& self) { return std::get<67>(self.tuple); }
      decltype(auto) iu68(this auto&& self) { return std::get<68>(self.tuple); }
      decltype(auto) iu69(this auto&& self) { return std::get<69>(self.tuple); }
      decltype(auto) iu70(this auto&& self) { return std::get<70>(self.tuple); }
      decltype(auto) iu71(this auto&& self) { return std::get<71>(self.tuple); }
      decltype(auto) iu72(this auto&& self) { return std::get<72>(self.tuple); }
      decltype(auto) iu73(this auto&& self) { return std::get<73>(self.tuple); }
      decltype(auto) iu74(this auto&& self) { return std::get<74>(self.tuple); }
      decltype(auto) iu75(this auto&& self) { return std::get<75>(self.tuple); }
      decltype(auto) iu76(this auto&& self) { return std::get<76>(self.tuple); }
      decltype(auto) iu77(this auto&& self) { return std::get<77>(self.tuple); }
      decltype(auto) iu78(this auto&& self) { return std::get<78>(self.tuple); }
      decltype(auto) iu79(this auto&& self) { return std::get<79>(self.tuple); }
      decltype(auto) iu80(this auto&& self) { return std::get<80>(self.tuple); }
      decltype(auto) iu81(this auto&& self) { return std::get<81>(self.tuple); }
      decltype(auto) iu82(this auto&& self) { return std::get<82>(self.tuple); }
      decltype(auto) iu83(this auto&& self) { return std::get<83>(self.tuple); }
      decltype(auto) iu84(this auto&& self) { return std::get<84>(self.tuple); }
      decltype(auto) iu85(this auto&& self) { return std::get<85>(self.tuple); }
      decltype(auto) iu86(this auto&& self) { return std::get<86>(self.tuple); }
      decltype(auto) iu87(this auto&& self) { return std::get<87>(self.tuple); }
      decltype(auto) iu88(this auto&& self) { return std::get<88>(self.tuple); }
      decltype(auto) iu89(this auto&& self) { return std::get<89>(self.tuple); }
      decltype(auto) iu90(this auto&& self) { return std::get<90>(self.tuple); }
      decltype(auto) iu91(this auto&& self) { return std::get<91>(self.tuple); }
      decltype(auto) iu92(this auto&& self) { return std::get<92>(self.tuple); }
      decltype(auto) iu93(this auto&& self) { return std::get<93>(self.tuple); }
      decltype(auto) iu94(this auto&& self) { return std::get<94>(self.tuple); }
      decltype(auto) iu95(this auto&& self) { return std::get<95>(self.tuple); }
      decltype(auto) iu96(this auto&& self) { return std::get<96>(self.tuple); }
      decltype(auto) iu97(this auto&& self) { return std::get<97>(self.tuple); }
      decltype(auto) iu98(this auto&& self) { return std::get<98>(self.tuple); }
      decltype(auto) iu99(this auto&& self) { return std::get<99>(self.tuple); }

      auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(tuple); }
   };

   std::vector<Container> data;

   ~KRelationTestMany() override = default;
   std::string_view name() const override { return "KRelationTestMany"; };
   void loadRelation(const std::string& in, char separator) override;
   void unloadRelation() override;
   size_t size() const override { return data.size(); }
   RuntimeValue get(size_t idx, const IU& iu) const override;
   void genTblFiles(const std::string& tblPath, char separator) const override;

   static schemac::Table genSchema();
};

factDB::BaseTable* create_KRelationTestMany();
void destroy_KRelationTestMany(factDB::BaseTable* ptr);
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
