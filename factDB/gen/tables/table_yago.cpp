#include "factDB/gen/tables/table_yago.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::tables {
// ---------------------------------------------------------------------------------------------------
void YagoBase::loadRelation(const std::string& path, char separator) {
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
RuntimeValue YagoBase::get(size_t idx, const IU& iu) const {
   if ("s" == iu.column) {
      return data[idx].s().toRTV();
   } else if ("d" == iu.column) {
      return data[idx].d().toRTV();
   } else {
      unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
void YagoBase::unloadRelation() {
   data.clear();
   loaded = false;
}
// ---------------------------------------------------------------------------------------------------
schemac::Table YagoBase::genSchema(uint32_t id, std::string name) {
   std::vector<factDB::schemac::Column> cols = {
      {"s", schemac::Type::Integer()},
      {"d", schemac::Type::Integer()}};
   return {id, name, std::move(cols)};
}
// ---------------------------------------------------------------------------------------------------

schemac::Table yago0::genSchema() { return YagoBase::genSchema(318, "yago0"); }
schemac::Table yago1::genSchema() { return YagoBase::genSchema(319, "yago1"); }
schemac::Table yago2::genSchema() { return YagoBase::genSchema(320, "yago2"); }
schemac::Table yago3::genSchema() { return YagoBase::genSchema(321, "yago3"); }
schemac::Table yago4::genSchema() { return YagoBase::genSchema(322, "yago4"); }
schemac::Table yago5::genSchema() { return YagoBase::genSchema(323, "yago5"); }
schemac::Table yago6::genSchema() { return YagoBase::genSchema(324, "yago6"); }
schemac::Table yago7::genSchema() { return YagoBase::genSchema(325, "yago7"); }
schemac::Table yago8::genSchema() { return YagoBase::genSchema(326, "yago8"); }
schemac::Table yago9::genSchema() { return YagoBase::genSchema(327, "yago9"); }
schemac::Table yago10::genSchema() { return YagoBase::genSchema(328, "yago10"); }
schemac::Table yago11::genSchema() { return YagoBase::genSchema(329, "yago11"); }
schemac::Table yago12::genSchema() { return YagoBase::genSchema(330, "yago12"); }
schemac::Table yago13::genSchema() { return YagoBase::genSchema(331, "yago13"); }
schemac::Table yago14::genSchema() { return YagoBase::genSchema(332, "yago14"); }
schemac::Table yago15::genSchema() { return YagoBase::genSchema(333, "yago15"); }
schemac::Table yago16::genSchema() { return YagoBase::genSchema(334, "yago16"); }
schemac::Table yago17::genSchema() { return YagoBase::genSchema(335, "yago17"); }
schemac::Table yago18::genSchema() { return YagoBase::genSchema(336, "yago18"); }
schemac::Table yago19::genSchema() { return YagoBase::genSchema(337, "yago19"); }
schemac::Table yago20::genSchema() { return YagoBase::genSchema(338, "yago20"); }
schemac::Table yago21::genSchema() { return YagoBase::genSchema(339, "yago21"); }
schemac::Table yago22::genSchema() { return YagoBase::genSchema(340, "yago22"); }
schemac::Table yago23::genSchema() { return YagoBase::genSchema(341, "yago23"); }
schemac::Table yago24::genSchema() { return YagoBase::genSchema(342, "yago24"); }
schemac::Table yago25::genSchema() { return YagoBase::genSchema(343, "yago25"); }
schemac::Table yago26::genSchema() { return YagoBase::genSchema(344, "yago26"); }
schemac::Table yago27::genSchema() { return YagoBase::genSchema(345, "yago27"); }
schemac::Table yago28::genSchema() { return YagoBase::genSchema(346, "yago28"); }
schemac::Table yago29::genSchema() { return YagoBase::genSchema(347, "yago29"); }
schemac::Table yago30::genSchema() { return YagoBase::genSchema(348, "yago30"); }
schemac::Table yago31::genSchema() { return YagoBase::genSchema(349, "yago31"); }
schemac::Table yago32::genSchema() { return YagoBase::genSchema(350, "yago32"); }
schemac::Table yago33::genSchema() { return YagoBase::genSchema(351, "yago33"); }
schemac::Table yago34::genSchema() { return YagoBase::genSchema(352, "yago34"); }
schemac::Table yago35::genSchema() { return YagoBase::genSchema(353, "yago35"); }
schemac::Table yago36::genSchema() { return YagoBase::genSchema(354, "yago36"); }
schemac::Table yago37::genSchema() { return YagoBase::genSchema(355, "yago37"); }
schemac::Table yago38::genSchema() { return YagoBase::genSchema(356, "yago38"); }
schemac::Table yago39::genSchema() { return YagoBase::genSchema(357, "yago39"); }
schemac::Table yago40::genSchema() { return YagoBase::genSchema(358, "yago40"); }
schemac::Table yago41::genSchema() { return YagoBase::genSchema(359, "yago41"); }
schemac::Table yago42::genSchema() { return YagoBase::genSchema(360, "yago42"); }
schemac::Table yago43::genSchema() { return YagoBase::genSchema(361, "yago43"); }
schemac::Table yago44::genSchema() { return YagoBase::genSchema(362, "yago44"); }
schemac::Table yago45::genSchema() { return YagoBase::genSchema(363, "yago45"); }
schemac::Table yago46::genSchema() { return YagoBase::genSchema(364, "yago46"); }
schemac::Table yago47::genSchema() { return YagoBase::genSchema(365, "yago47"); }
schemac::Table yago48::genSchema() { return YagoBase::genSchema(366, "yago48"); }
schemac::Table yago49::genSchema() { return YagoBase::genSchema(367, "yago49"); }
schemac::Table yago50::genSchema() { return YagoBase::genSchema(368, "yago50"); }
schemac::Table yago51::genSchema() { return YagoBase::genSchema(369, "yago51"); }
schemac::Table yago52::genSchema() { return YagoBase::genSchema(370, "yago52"); }
schemac::Table yago53::genSchema() { return YagoBase::genSchema(371, "yago53"); }
schemac::Table yago54::genSchema() { return YagoBase::genSchema(372, "yago54"); }
schemac::Table yago55::genSchema() { return YagoBase::genSchema(373, "yago55"); }
schemac::Table yago56::genSchema() { return YagoBase::genSchema(374, "yago56"); }
schemac::Table yago57::genSchema() { return YagoBase::genSchema(375, "yago57"); }
schemac::Table yago58::genSchema() { return YagoBase::genSchema(376, "yago58"); }
schemac::Table yago59::genSchema() { return YagoBase::genSchema(377, "yago59"); }
schemac::Table yago60::genSchema() { return YagoBase::genSchema(378, "yago60"); }
schemac::Table yago61::genSchema() { return YagoBase::genSchema(379, "yago61"); }
schemac::Table yago62::genSchema() { return YagoBase::genSchema(380, "yago62"); }
schemac::Table yago63::genSchema() { return YagoBase::genSchema(381, "yago63"); }
schemac::Table yago64::genSchema() { return YagoBase::genSchema(382, "yago64"); }
schemac::Table yago65::genSchema() { return YagoBase::genSchema(383, "yago65"); }
schemac::Table yago66::genSchema() { return YagoBase::genSchema(384, "yago66"); }
schemac::Table yago67::genSchema() { return YagoBase::genSchema(385, "yago67"); }
schemac::Table yago68::genSchema() { return YagoBase::genSchema(386, "yago68"); }
schemac::Table yago69::genSchema() { return YagoBase::genSchema(387, "yago69"); }
schemac::Table yago70::genSchema() { return YagoBase::genSchema(388, "yago70"); }
schemac::Table yago71::genSchema() { return YagoBase::genSchema(389, "yago71"); }
schemac::Table yago72::genSchema() { return YagoBase::genSchema(390, "yago72"); }
schemac::Table yago73::genSchema() { return YagoBase::genSchema(391, "yago73"); }
schemac::Table yago74::genSchema() { return YagoBase::genSchema(392, "yago74"); }
schemac::Table yago75::genSchema() { return YagoBase::genSchema(393, "yago75"); }
schemac::Table yago76::genSchema() { return YagoBase::genSchema(394, "yago76"); }
schemac::Table yago77::genSchema() { return YagoBase::genSchema(395, "yago77"); }
schemac::Table yago78::genSchema() { return YagoBase::genSchema(396, "yago78"); }
schemac::Table yago79::genSchema() { return YagoBase::genSchema(397, "yago79"); }
schemac::Table yago80::genSchema() { return YagoBase::genSchema(398, "yago80"); }
schemac::Table yago81::genSchema() { return YagoBase::genSchema(399, "yago81"); }
schemac::Table yago82::genSchema() { return YagoBase::genSchema(400, "yago82"); }
schemac::Table yago83::genSchema() { return YagoBase::genSchema(401, "yago83"); }
schemac::Table yago84::genSchema() { return YagoBase::genSchema(402, "yago84"); }
schemac::Table yago85::genSchema() { return YagoBase::genSchema(403, "yago85"); }
schemac::Table yago86::genSchema() { return YagoBase::genSchema(404, "yago86"); }
schemac::Table yago87::genSchema() { return YagoBase::genSchema(405, "yago87"); }
schemac::Table yago88::genSchema() { return YagoBase::genSchema(406, "yago88"); }
schemac::Table yago89::genSchema() { return YagoBase::genSchema(407, "yago89"); }
schemac::Table yago90::genSchema() { return YagoBase::genSchema(408, "yago90"); }
// ---------------------------------------------------------------------------------------------------
void yago0::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago0, Container>(tblPath, separator); }
void yago1::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago1, Container>(tblPath, separator); }
void yago2::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago2, Container>(tblPath, separator); }
void yago3::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago3, Container>(tblPath, separator); }
void yago4::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago4, Container>(tblPath, separator); }
void yago5::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago5, Container>(tblPath, separator); }
void yago6::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago6, Container>(tblPath, separator); }
void yago7::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago7, Container>(tblPath, separator); }
void yago8::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago8, Container>(tblPath, separator); }
void yago9::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago9, Container>(tblPath, separator); }
void yago10::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago10, Container>(tblPath, separator); }
void yago11::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago11, Container>(tblPath, separator); }
void yago12::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago12, Container>(tblPath, separator); }
void yago13::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago13, Container>(tblPath, separator); }
void yago14::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago14, Container>(tblPath, separator); }
void yago15::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago15, Container>(tblPath, separator); }
void yago16::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago16, Container>(tblPath, separator); }
void yago17::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago17, Container>(tblPath, separator); }
void yago18::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago18, Container>(tblPath, separator); }
void yago19::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago19, Container>(tblPath, separator); }
void yago20::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago20, Container>(tblPath, separator); }
void yago21::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago21, Container>(tblPath, separator); }
void yago22::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago22, Container>(tblPath, separator); }
void yago23::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago23, Container>(tblPath, separator); }
void yago24::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago24, Container>(tblPath, separator); }
void yago25::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago25, Container>(tblPath, separator); }
void yago26::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago26, Container>(tblPath, separator); }
void yago27::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago27, Container>(tblPath, separator); }
void yago28::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago28, Container>(tblPath, separator); }
void yago29::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago29, Container>(tblPath, separator); }
void yago30::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago30, Container>(tblPath, separator); }
void yago31::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago31, Container>(tblPath, separator); }
void yago32::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago32, Container>(tblPath, separator); }
void yago33::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago33, Container>(tblPath, separator); }
void yago34::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago34, Container>(tblPath, separator); }
void yago35::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago35, Container>(tblPath, separator); }
void yago36::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago36, Container>(tblPath, separator); }
void yago37::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago37, Container>(tblPath, separator); }
void yago38::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago38, Container>(tblPath, separator); }
void yago39::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago39, Container>(tblPath, separator); }
void yago40::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago40, Container>(tblPath, separator); }
void yago41::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago41, Container>(tblPath, separator); }
void yago42::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago42, Container>(tblPath, separator); }
void yago43::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago43, Container>(tblPath, separator); }
void yago44::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago44, Container>(tblPath, separator); }
void yago45::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago45, Container>(tblPath, separator); }
void yago46::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago46, Container>(tblPath, separator); }
void yago47::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago47, Container>(tblPath, separator); }
void yago48::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago48, Container>(tblPath, separator); }
void yago49::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago49, Container>(tblPath, separator); }
void yago50::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago50, Container>(tblPath, separator); }
void yago51::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago51, Container>(tblPath, separator); }
void yago52::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago52, Container>(tblPath, separator); }
void yago53::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago53, Container>(tblPath, separator); }
void yago54::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago54, Container>(tblPath, separator); }
void yago55::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago55, Container>(tblPath, separator); }
void yago56::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago56, Container>(tblPath, separator); }
void yago57::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago57, Container>(tblPath, separator); }
void yago58::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago58, Container>(tblPath, separator); }
void yago59::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago59, Container>(tblPath, separator); }
void yago60::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago60, Container>(tblPath, separator); }
void yago61::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago61, Container>(tblPath, separator); }
void yago62::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago62, Container>(tblPath, separator); }
void yago63::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago63, Container>(tblPath, separator); }
void yago64::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago64, Container>(tblPath, separator); }
void yago65::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago65, Container>(tblPath, separator); }
void yago66::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago66, Container>(tblPath, separator); }
void yago67::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago67, Container>(tblPath, separator); }
void yago68::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago68, Container>(tblPath, separator); }
void yago69::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago69, Container>(tblPath, separator); }
void yago70::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago70, Container>(tblPath, separator); }
void yago71::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago71, Container>(tblPath, separator); }
void yago72::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago72, Container>(tblPath, separator); }
void yago73::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago73, Container>(tblPath, separator); }
void yago74::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago74, Container>(tblPath, separator); }
void yago75::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago75, Container>(tblPath, separator); }
void yago76::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago76, Container>(tblPath, separator); }
void yago77::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago77, Container>(tblPath, separator); }
void yago78::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago78, Container>(tblPath, separator); }
void yago79::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago79, Container>(tblPath, separator); }
void yago80::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago80, Container>(tblPath, separator); }
void yago81::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago81, Container>(tblPath, separator); }
void yago82::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago82, Container>(tblPath, separator); }
void yago83::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago83, Container>(tblPath, separator); }
void yago84::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago84, Container>(tblPath, separator); }
void yago85::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago85, Container>(tblPath, separator); }
void yago86::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago86, Container>(tblPath, separator); }
void yago87::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago87, Container>(tblPath, separator); }
void yago88::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago88, Container>(tblPath, separator); }
void yago89::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago89, Container>(tblPath, separator); }
void yago90::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<yago90, Container>(tblPath, separator); }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::tables
// ---------------------------------------------------------------------------------------------------