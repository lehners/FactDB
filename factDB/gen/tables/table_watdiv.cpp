#include "factDB/gen/tables/table_watdiv.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::tables {
// ---------------------------------------------------------------------------------------------------
void WatdivBase::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      BaseTable::readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         BaseTable::updateElem(newData.s(), in, separator, false);
         BaseTable::updateElem(newData.d(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
// ---------------------------------------------------------------------------------------------------
RuntimeValue WatdivBase::get(size_t idx, const IU& iu) const {
   if ("s" == iu.column) {
      return data[idx].s().toRTV();
   } else if ("d" == iu.column) {
      return data[idx].d().toRTV();
   } else {
      unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
void WatdivBase::unloadRelation() {
   data.clear();
   loaded = false;
}
// ---------------------------------------------------------------------------------------------------
schemac::Table WatdivBase::genSchema(uint32_t id, std::string name) {
   std::vector<factDB::schemac::Column> cols = {
      {"s", schemac::Type::Integer()},
      {"d", schemac::Type::Integer()}};
   return {id, name, std::move(cols)};
}
// ---------------------------------------------------------------------------------------------------
schemac::Table watdiv1052572::genSchema() { return WatdivBase::genSchema(232, "watdiv1052572"); }
schemac::Table watdiv1052573::genSchema() { return WatdivBase::genSchema(233, "watdiv1052573"); }
schemac::Table watdiv1052574::genSchema() { return WatdivBase::genSchema(234, "watdiv1052574"); }
schemac::Table watdiv1052575::genSchema() { return WatdivBase::genSchema(235, "watdiv1052575"); }
schemac::Table watdiv1052576::genSchema() { return WatdivBase::genSchema(236, "watdiv1052576"); }
schemac::Table watdiv1052577::genSchema() { return WatdivBase::genSchema(237, "watdiv1052577"); }
schemac::Table watdiv1052578::genSchema() { return WatdivBase::genSchema(238, "watdiv1052578"); }
schemac::Table watdiv1052579::genSchema() { return WatdivBase::genSchema(239, "watdiv1052579"); }
schemac::Table watdiv1052580::genSchema() { return WatdivBase::genSchema(240, "watdiv1052580"); }
schemac::Table watdiv1052581::genSchema() { return WatdivBase::genSchema(241, "watdiv1052581"); }
schemac::Table watdiv1052582::genSchema() { return WatdivBase::genSchema(242, "watdiv1052582"); }
schemac::Table watdiv1052583::genSchema() { return WatdivBase::genSchema(243, "watdiv1052583"); }
schemac::Table watdiv1052584::genSchema() { return WatdivBase::genSchema(244, "watdiv1052584"); }
schemac::Table watdiv1052585::genSchema() { return WatdivBase::genSchema(245, "watdiv1052585"); }
schemac::Table watdiv1052586::genSchema() { return WatdivBase::genSchema(246, "watdiv1052586"); }
schemac::Table watdiv1052587::genSchema() { return WatdivBase::genSchema(247, "watdiv1052587"); }
schemac::Table watdiv1052588::genSchema() { return WatdivBase::genSchema(248, "watdiv1052588"); }
schemac::Table watdiv1052589::genSchema() { return WatdivBase::genSchema(249, "watdiv1052589"); }
schemac::Table watdiv1052590::genSchema() { return WatdivBase::genSchema(250, "watdiv1052590"); }
schemac::Table watdiv1052591::genSchema() { return WatdivBase::genSchema(251, "watdiv1052591"); }
schemac::Table watdiv1052592::genSchema() { return WatdivBase::genSchema(252, "watdiv1052592"); }
schemac::Table watdiv1052593::genSchema() { return WatdivBase::genSchema(253, "watdiv1052593"); }
schemac::Table watdiv1052594::genSchema() { return WatdivBase::genSchema(254, "watdiv1052594"); }
schemac::Table watdiv1052595::genSchema() { return WatdivBase::genSchema(255, "watdiv1052595"); }
schemac::Table watdiv1052596::genSchema() { return WatdivBase::genSchema(256, "watdiv1052596"); }
schemac::Table watdiv1052597::genSchema() { return WatdivBase::genSchema(257, "watdiv1052597"); }
schemac::Table watdiv1052598::genSchema() { return WatdivBase::genSchema(258, "watdiv1052598"); }
schemac::Table watdiv1052599::genSchema() { return WatdivBase::genSchema(259, "watdiv1052599"); }
schemac::Table watdiv1052600::genSchema() { return WatdivBase::genSchema(260, "watdiv1052600"); }
schemac::Table watdiv1052601::genSchema() { return WatdivBase::genSchema(261, "watdiv1052601"); }
schemac::Table watdiv1052602::genSchema() { return WatdivBase::genSchema(262, "watdiv1052602"); }
schemac::Table watdiv1052603::genSchema() { return WatdivBase::genSchema(263, "watdiv1052603"); }
schemac::Table watdiv1052604::genSchema() { return WatdivBase::genSchema(264, "watdiv1052604"); }
schemac::Table watdiv1052605::genSchema() { return WatdivBase::genSchema(265, "watdiv1052605"); }
schemac::Table watdiv1052606::genSchema() { return WatdivBase::genSchema(266, "watdiv1052606"); }
schemac::Table watdiv1052607::genSchema() { return WatdivBase::genSchema(267, "watdiv1052607"); }
schemac::Table watdiv1052608::genSchema() { return WatdivBase::genSchema(268, "watdiv1052608"); }
schemac::Table watdiv1052609::genSchema() { return WatdivBase::genSchema(269, "watdiv1052609"); }
schemac::Table watdiv1052610::genSchema() { return WatdivBase::genSchema(270, "watdiv1052610"); }
schemac::Table watdiv1052611::genSchema() { return WatdivBase::genSchema(271, "watdiv1052611"); }
schemac::Table watdiv1052612::genSchema() { return WatdivBase::genSchema(272, "watdiv1052612"); }
schemac::Table watdiv1052613::genSchema() { return WatdivBase::genSchema(273, "watdiv1052613"); }
schemac::Table watdiv1052614::genSchema() { return WatdivBase::genSchema(274, "watdiv1052614"); }
schemac::Table watdiv1052615::genSchema() { return WatdivBase::genSchema(275, "watdiv1052615"); }
schemac::Table watdiv1052616::genSchema() { return WatdivBase::genSchema(276, "watdiv1052616"); }
schemac::Table watdiv1052617::genSchema() { return WatdivBase::genSchema(277, "watdiv1052617"); }
schemac::Table watdiv1052618::genSchema() { return WatdivBase::genSchema(278, "watdiv1052618"); }
schemac::Table watdiv1052619::genSchema() { return WatdivBase::genSchema(279, "watdiv1052619"); }
schemac::Table watdiv1052620::genSchema() { return WatdivBase::genSchema(280, "watdiv1052620"); }
schemac::Table watdiv1052621::genSchema() { return WatdivBase::genSchema(281, "watdiv1052621"); }
schemac::Table watdiv1052622::genSchema() { return WatdivBase::genSchema(282, "watdiv1052622"); }
schemac::Table watdiv1052623::genSchema() { return WatdivBase::genSchema(283, "watdiv1052623"); }
schemac::Table watdiv1052624::genSchema() { return WatdivBase::genSchema(284, "watdiv1052624"); }
schemac::Table watdiv1052625::genSchema() { return WatdivBase::genSchema(285, "watdiv1052625"); }
schemac::Table watdiv1052626::genSchema() { return WatdivBase::genSchema(286, "watdiv1052626"); }
schemac::Table watdiv1052627::genSchema() { return WatdivBase::genSchema(287, "watdiv1052627"); }
schemac::Table watdiv1052628::genSchema() { return WatdivBase::genSchema(288, "watdiv1052628"); }
schemac::Table watdiv1052629::genSchema() { return WatdivBase::genSchema(289, "watdiv1052629"); }
schemac::Table watdiv1052630::genSchema() { return WatdivBase::genSchema(290, "watdiv1052630"); }
schemac::Table watdiv1052631::genSchema() { return WatdivBase::genSchema(291, "watdiv1052631"); }
schemac::Table watdiv1052632::genSchema() { return WatdivBase::genSchema(292, "watdiv1052632"); }
schemac::Table watdiv1052633::genSchema() { return WatdivBase::genSchema(293, "watdiv1052633"); }
schemac::Table watdiv1052634::genSchema() { return WatdivBase::genSchema(294, "watdiv1052634"); }
schemac::Table watdiv1052635::genSchema() { return WatdivBase::genSchema(295, "watdiv1052635"); }
schemac::Table watdiv1052636::genSchema() { return WatdivBase::genSchema(296, "watdiv1052636"); }
schemac::Table watdiv1052637::genSchema() { return WatdivBase::genSchema(297, "watdiv1052637"); }
schemac::Table watdiv1052638::genSchema() { return WatdivBase::genSchema(298, "watdiv1052638"); }
schemac::Table watdiv1052639::genSchema() { return WatdivBase::genSchema(299, "watdiv1052639"); }
schemac::Table watdiv1052640::genSchema() { return WatdivBase::genSchema(300, "watdiv1052640"); }
schemac::Table watdiv1052641::genSchema() { return WatdivBase::genSchema(301, "watdiv1052641"); }
schemac::Table watdiv1052642::genSchema() { return WatdivBase::genSchema(302, "watdiv1052642"); }
schemac::Table watdiv1052643::genSchema() { return WatdivBase::genSchema(303, "watdiv1052643"); }
schemac::Table watdiv1052644::genSchema() { return WatdivBase::genSchema(304, "watdiv1052644"); }
schemac::Table watdiv1052645::genSchema() { return WatdivBase::genSchema(305, "watdiv1052645"); }
schemac::Table watdiv1052646::genSchema() { return WatdivBase::genSchema(306, "watdiv1052646"); }
schemac::Table watdiv1052647::genSchema() { return WatdivBase::genSchema(307, "watdiv1052647"); }
schemac::Table watdiv1052648::genSchema() { return WatdivBase::genSchema(308, "watdiv1052648"); }
schemac::Table watdiv1052649::genSchema() { return WatdivBase::genSchema(309, "watdiv1052649"); }
schemac::Table watdiv1052650::genSchema() { return WatdivBase::genSchema(310, "watdiv1052650"); }
schemac::Table watdiv1052651::genSchema() { return WatdivBase::genSchema(311, "watdiv1052651"); }
schemac::Table watdiv1052652::genSchema() { return WatdivBase::genSchema(312, "watdiv1052652"); }
schemac::Table watdiv1052653::genSchema() { return WatdivBase::genSchema(313, "watdiv1052653"); }
schemac::Table watdiv1052654::genSchema() { return WatdivBase::genSchema(314, "watdiv1052654"); }
schemac::Table watdiv1052655::genSchema() { return WatdivBase::genSchema(315, "watdiv1052655"); }
schemac::Table watdiv1052656::genSchema() { return WatdivBase::genSchema(316, "watdiv1052656"); }
schemac::Table watdiv1052657::genSchema() { return WatdivBase::genSchema(317, "watdiv1052657"); }
// ---------------------------------------------------------------------------------------------------
void watdiv1052572::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052572, Container>(tblPath, separator); }
void watdiv1052573::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052573, Container>(tblPath, separator); }
void watdiv1052574::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052574, Container>(tblPath, separator); }
void watdiv1052575::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052575, Container>(tblPath, separator); }
void watdiv1052576::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052576, Container>(tblPath, separator); }
void watdiv1052577::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052577, Container>(tblPath, separator); }
void watdiv1052578::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052578, Container>(tblPath, separator); }
void watdiv1052579::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052579, Container>(tblPath, separator); }
void watdiv1052580::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052580, Container>(tblPath, separator); }
void watdiv1052581::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052581, Container>(tblPath, separator); }
void watdiv1052582::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052582, Container>(tblPath, separator); }
void watdiv1052583::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052583, Container>(tblPath, separator); }
void watdiv1052584::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052584, Container>(tblPath, separator); }
void watdiv1052585::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052585, Container>(tblPath, separator); }
void watdiv1052586::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052586, Container>(tblPath, separator); }
void watdiv1052587::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052587, Container>(tblPath, separator); }
void watdiv1052588::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052588, Container>(tblPath, separator); }
void watdiv1052589::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052589, Container>(tblPath, separator); }
void watdiv1052590::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052590, Container>(tblPath, separator); }
void watdiv1052591::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052591, Container>(tblPath, separator); }
void watdiv1052592::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052592, Container>(tblPath, separator); }
void watdiv1052593::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052593, Container>(tblPath, separator); }
void watdiv1052594::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052594, Container>(tblPath, separator); }
void watdiv1052595::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052595, Container>(tblPath, separator); }
void watdiv1052596::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052596, Container>(tblPath, separator); }
void watdiv1052597::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052597, Container>(tblPath, separator); }
void watdiv1052598::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052598, Container>(tblPath, separator); }
void watdiv1052599::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052599, Container>(tblPath, separator); }
void watdiv1052600::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052600, Container>(tblPath, separator); }
void watdiv1052601::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052601, Container>(tblPath, separator); }
void watdiv1052602::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052602, Container>(tblPath, separator); }
void watdiv1052603::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052603, Container>(tblPath, separator); }
void watdiv1052604::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052604, Container>(tblPath, separator); }
void watdiv1052605::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052605, Container>(tblPath, separator); }
void watdiv1052606::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052606, Container>(tblPath, separator); }
void watdiv1052607::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052607, Container>(tblPath, separator); }
void watdiv1052608::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052608, Container>(tblPath, separator); }
void watdiv1052609::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052609, Container>(tblPath, separator); }
void watdiv1052610::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052610, Container>(tblPath, separator); }
void watdiv1052611::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052611, Container>(tblPath, separator); }
void watdiv1052612::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052612, Container>(tblPath, separator); }
void watdiv1052613::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052613, Container>(tblPath, separator); }
void watdiv1052614::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052614, Container>(tblPath, separator); }
void watdiv1052615::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052615, Container>(tblPath, separator); }
void watdiv1052616::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052616, Container>(tblPath, separator); }
void watdiv1052617::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052617, Container>(tblPath, separator); }
void watdiv1052618::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052618, Container>(tblPath, separator); }
void watdiv1052619::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052619, Container>(tblPath, separator); }
void watdiv1052620::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052620, Container>(tblPath, separator); }
void watdiv1052621::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052621, Container>(tblPath, separator); }
void watdiv1052622::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052622, Container>(tblPath, separator); }
void watdiv1052623::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052623, Container>(tblPath, separator); }
void watdiv1052624::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052624, Container>(tblPath, separator); }
void watdiv1052625::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052625, Container>(tblPath, separator); }
void watdiv1052626::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052626, Container>(tblPath, separator); }
void watdiv1052627::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052627, Container>(tblPath, separator); }
void watdiv1052628::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052628, Container>(tblPath, separator); }
void watdiv1052629::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052629, Container>(tblPath, separator); }
void watdiv1052630::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052630, Container>(tblPath, separator); }
void watdiv1052631::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052631, Container>(tblPath, separator); }
void watdiv1052632::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052632, Container>(tblPath, separator); }
void watdiv1052633::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052633, Container>(tblPath, separator); }
void watdiv1052634::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052634, Container>(tblPath, separator); }
void watdiv1052635::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052635, Container>(tblPath, separator); }
void watdiv1052636::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052636, Container>(tblPath, separator); }
void watdiv1052637::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052637, Container>(tblPath, separator); }
void watdiv1052638::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052638, Container>(tblPath, separator); }
void watdiv1052639::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052639, Container>(tblPath, separator); }
void watdiv1052640::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052640, Container>(tblPath, separator); }
void watdiv1052641::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052641, Container>(tblPath, separator); }
void watdiv1052642::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052642, Container>(tblPath, separator); }
void watdiv1052643::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052643, Container>(tblPath, separator); }
void watdiv1052644::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052644, Container>(tblPath, separator); }
void watdiv1052645::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052645, Container>(tblPath, separator); }
void watdiv1052646::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052646, Container>(tblPath, separator); }
void watdiv1052647::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052647, Container>(tblPath, separator); }
void watdiv1052648::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052648, Container>(tblPath, separator); }
void watdiv1052649::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052649, Container>(tblPath, separator); }
void watdiv1052650::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052650, Container>(tblPath, separator); }
void watdiv1052651::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052651, Container>(tblPath, separator); }
void watdiv1052652::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052652, Container>(tblPath, separator); }
void watdiv1052653::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052653, Container>(tblPath, separator); }
void watdiv1052654::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052654, Container>(tblPath, separator); }
void watdiv1052655::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052655, Container>(tblPath, separator); }
void watdiv1052656::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052656, Container>(tblPath, separator); }
void watdiv1052657::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<watdiv1052657, Container>(tblPath, separator); }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::tables
// ---------------------------------------------------------------------------------------------------
