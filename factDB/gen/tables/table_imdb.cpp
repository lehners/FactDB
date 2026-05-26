#include "factDB/gen/tables/table_imdb.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::tables {
// ---------------------------------------------------------------------------------------------------
void ImdbBase::loadRelation(const std::string& path, char separator) {
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
RuntimeValue ImdbBase::get(size_t idx, const IU& iu) const {
   if ("s" == iu.column) {
      return data[idx].s().toRTV();
   } else if ("d" == iu.column) {
      return data[idx].d().toRTV();
   } else {
      unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
void ImdbBase::unloadRelation() {
   data.clear();
   loaded = false;
}
// ---------------------------------------------------------------------------------------------------
schemac::Table ImdbBase::genSchema(uint32_t id, std::string name) {
   std::vector<factDB::schemac::Column> cols = {
      {"s", schemac::Type::Integer()},
      {"d", schemac::Type::Integer()}};
   return {id, name, std::move(cols)};
}
// ---------------------------------------------------------------------------------------------------
schemac::Table imdb1::genSchema() { return ImdbBase::genSchema(105, "imdb1"); }
schemac::Table imdb2::genSchema() { return ImdbBase::genSchema(106, "imdb2"); }
schemac::Table imdb3::genSchema() { return ImdbBase::genSchema(107, "imdb3"); }
schemac::Table imdb4::genSchema() { return ImdbBase::genSchema(108, "imdb4"); }
schemac::Table imdb5::genSchema() { return ImdbBase::genSchema(109, "imdb5"); }
schemac::Table imdb6::genSchema() { return ImdbBase::genSchema(110, "imdb6"); }
schemac::Table imdb7::genSchema() { return ImdbBase::genSchema(111, "imdb7"); }
schemac::Table imdb8::genSchema() { return ImdbBase::genSchema(112, "imdb8"); }
schemac::Table imdb9::genSchema() { return ImdbBase::genSchema(113, "imdb9"); }
schemac::Table imdb10::genSchema() { return ImdbBase::genSchema(114, "imdb10"); }
schemac::Table imdb11::genSchema() { return ImdbBase::genSchema(115, "imdb11"); }
schemac::Table imdb12::genSchema() { return ImdbBase::genSchema(116, "imdb12"); }
schemac::Table imdb13::genSchema() { return ImdbBase::genSchema(117, "imdb13"); }
schemac::Table imdb14::genSchema() { return ImdbBase::genSchema(118, "imdb14"); }
schemac::Table imdb15::genSchema() { return ImdbBase::genSchema(119, "imdb15"); }
schemac::Table imdb16::genSchema() { return ImdbBase::genSchema(120, "imdb16"); }
schemac::Table imdb17::genSchema() { return ImdbBase::genSchema(121, "imdb17"); }
schemac::Table imdb18::genSchema() { return ImdbBase::genSchema(122, "imdb18"); }
schemac::Table imdb19::genSchema() { return ImdbBase::genSchema(123, "imdb19"); }
schemac::Table imdb20::genSchema() { return ImdbBase::genSchema(124, "imdb20"); }
schemac::Table imdb21::genSchema() { return ImdbBase::genSchema(125, "imdb21"); }
schemac::Table imdb22::genSchema() { return ImdbBase::genSchema(126, "imdb22"); }
schemac::Table imdb23::genSchema() { return ImdbBase::genSchema(127, "imdb23"); }
schemac::Table imdb24::genSchema() { return ImdbBase::genSchema(128, "imdb24"); }
schemac::Table imdb25::genSchema() { return ImdbBase::genSchema(129, "imdb25"); }
schemac::Table imdb26::genSchema() { return ImdbBase::genSchema(130, "imdb26"); }
schemac::Table imdb27::genSchema() { return ImdbBase::genSchema(131, "imdb27"); }
schemac::Table imdb28::genSchema() { return ImdbBase::genSchema(132, "imdb28"); }
schemac::Table imdb29::genSchema() { return ImdbBase::genSchema(133, "imdb29"); }
schemac::Table imdb30::genSchema() { return ImdbBase::genSchema(134, "imdb30"); }
schemac::Table imdb31::genSchema() { return ImdbBase::genSchema(135, "imdb31"); }
schemac::Table imdb32::genSchema() { return ImdbBase::genSchema(136, "imdb32"); }
schemac::Table imdb33::genSchema() { return ImdbBase::genSchema(137, "imdb33"); }
schemac::Table imdb34::genSchema() { return ImdbBase::genSchema(138, "imdb34"); }
schemac::Table imdb35::genSchema() { return ImdbBase::genSchema(139, "imdb35"); }
schemac::Table imdb36::genSchema() { return ImdbBase::genSchema(140, "imdb36"); }
schemac::Table imdb37::genSchema() { return ImdbBase::genSchema(141, "imdb37"); }
schemac::Table imdb38::genSchema() { return ImdbBase::genSchema(142, "imdb38"); }
schemac::Table imdb39::genSchema() { return ImdbBase::genSchema(143, "imdb39"); }
schemac::Table imdb40::genSchema() { return ImdbBase::genSchema(144, "imdb40"); }
schemac::Table imdb41::genSchema() { return ImdbBase::genSchema(145, "imdb41"); }
schemac::Table imdb42::genSchema() { return ImdbBase::genSchema(146, "imdb42"); }
schemac::Table imdb43::genSchema() { return ImdbBase::genSchema(147, "imdb43"); }
schemac::Table imdb44::genSchema() { return ImdbBase::genSchema(148, "imdb44"); }
schemac::Table imdb45::genSchema() { return ImdbBase::genSchema(149, "imdb45"); }
schemac::Table imdb46::genSchema() { return ImdbBase::genSchema(150, "imdb46"); }
schemac::Table imdb47::genSchema() { return ImdbBase::genSchema(151, "imdb47"); }
schemac::Table imdb48::genSchema() { return ImdbBase::genSchema(152, "imdb48"); }
schemac::Table imdb49::genSchema() { return ImdbBase::genSchema(153, "imdb49"); }
schemac::Table imdb50::genSchema() { return ImdbBase::genSchema(154, "imdb50"); }
schemac::Table imdb51::genSchema() { return ImdbBase::genSchema(155, "imdb51"); }
schemac::Table imdb52::genSchema() { return ImdbBase::genSchema(156, "imdb52"); }
schemac::Table imdb53::genSchema() { return ImdbBase::genSchema(157, "imdb53"); }
schemac::Table imdb54::genSchema() { return ImdbBase::genSchema(158, "imdb54"); }
schemac::Table imdb55::genSchema() { return ImdbBase::genSchema(159, "imdb55"); }
schemac::Table imdb56::genSchema() { return ImdbBase::genSchema(160, "imdb56"); }
schemac::Table imdb57::genSchema() { return ImdbBase::genSchema(161, "imdb57"); }
schemac::Table imdb58::genSchema() { return ImdbBase::genSchema(162, "imdb58"); }
schemac::Table imdb59::genSchema() { return ImdbBase::genSchema(163, "imdb59"); }
schemac::Table imdb60::genSchema() { return ImdbBase::genSchema(164, "imdb60"); }
schemac::Table imdb61::genSchema() { return ImdbBase::genSchema(165, "imdb61"); }
schemac::Table imdb62::genSchema() { return ImdbBase::genSchema(166, "imdb62"); }
schemac::Table imdb63::genSchema() { return ImdbBase::genSchema(167, "imdb63"); }
schemac::Table imdb64::genSchema() { return ImdbBase::genSchema(168, "imdb64"); }
schemac::Table imdb65::genSchema() { return ImdbBase::genSchema(169, "imdb65"); }
schemac::Table imdb66::genSchema() { return ImdbBase::genSchema(170, "imdb66"); }
schemac::Table imdb67::genSchema() { return ImdbBase::genSchema(171, "imdb67"); }
schemac::Table imdb68::genSchema() { return ImdbBase::genSchema(172, "imdb68"); }
schemac::Table imdb69::genSchema() { return ImdbBase::genSchema(173, "imdb69"); }
schemac::Table imdb70::genSchema() { return ImdbBase::genSchema(174, "imdb70"); }
schemac::Table imdb71::genSchema() { return ImdbBase::genSchema(175, "imdb71"); }
schemac::Table imdb72::genSchema() { return ImdbBase::genSchema(176, "imdb72"); }
schemac::Table imdb73::genSchema() { return ImdbBase::genSchema(177, "imdb73"); }
schemac::Table imdb74::genSchema() { return ImdbBase::genSchema(178, "imdb74"); }
schemac::Table imdb75::genSchema() { return ImdbBase::genSchema(179, "imdb75"); }
schemac::Table imdb76::genSchema() { return ImdbBase::genSchema(180, "imdb76"); }
schemac::Table imdb77::genSchema() { return ImdbBase::genSchema(181, "imdb77"); }
schemac::Table imdb78::genSchema() { return ImdbBase::genSchema(182, "imdb78"); }
schemac::Table imdb79::genSchema() { return ImdbBase::genSchema(183, "imdb79"); }
schemac::Table imdb80::genSchema() { return ImdbBase::genSchema(184, "imdb80"); }
schemac::Table imdb81::genSchema() { return ImdbBase::genSchema(185, "imdb81"); }
schemac::Table imdb82::genSchema() { return ImdbBase::genSchema(186, "imdb82"); }
schemac::Table imdb83::genSchema() { return ImdbBase::genSchema(187, "imdb83"); }
schemac::Table imdb84::genSchema() { return ImdbBase::genSchema(188, "imdb84"); }
schemac::Table imdb85::genSchema() { return ImdbBase::genSchema(189, "imdb85"); }
schemac::Table imdb86::genSchema() { return ImdbBase::genSchema(190, "imdb86"); }
schemac::Table imdb87::genSchema() { return ImdbBase::genSchema(191, "imdb87"); }
schemac::Table imdb88::genSchema() { return ImdbBase::genSchema(192, "imdb88"); }
schemac::Table imdb89::genSchema() { return ImdbBase::genSchema(193, "imdb89"); }
schemac::Table imdb90::genSchema() { return ImdbBase::genSchema(194, "imdb90"); }
schemac::Table imdb91::genSchema() { return ImdbBase::genSchema(195, "imdb91"); }
schemac::Table imdb92::genSchema() { return ImdbBase::genSchema(196, "imdb92"); }
schemac::Table imdb93::genSchema() { return ImdbBase::genSchema(197, "imdb93"); }
schemac::Table imdb94::genSchema() { return ImdbBase::genSchema(198, "imdb94"); }
schemac::Table imdb95::genSchema() { return ImdbBase::genSchema(199, "imdb95"); }
schemac::Table imdb96::genSchema() { return ImdbBase::genSchema(200, "imdb96"); }
schemac::Table imdb97::genSchema() { return ImdbBase::genSchema(201, "imdb97"); }
schemac::Table imdb98::genSchema() { return ImdbBase::genSchema(202, "imdb98"); }
schemac::Table imdb99::genSchema() { return ImdbBase::genSchema(203, "imdb99"); }
schemac::Table imdb100::genSchema() { return ImdbBase::genSchema(204, "imdb100"); }
schemac::Table imdb101::genSchema() { return ImdbBase::genSchema(205, "imdb101"); }
schemac::Table imdb102::genSchema() { return ImdbBase::genSchema(206, "imdb102"); }
schemac::Table imdb103::genSchema() { return ImdbBase::genSchema(207, "imdb103"); }
schemac::Table imdb104::genSchema() { return ImdbBase::genSchema(208, "imdb104"); }
schemac::Table imdb105::genSchema() { return ImdbBase::genSchema(209, "imdb105"); }
schemac::Table imdb106::genSchema() { return ImdbBase::genSchema(210, "imdb106"); }
schemac::Table imdb107::genSchema() { return ImdbBase::genSchema(211, "imdb107"); }
schemac::Table imdb108::genSchema() { return ImdbBase::genSchema(212, "imdb108"); }
schemac::Table imdb109::genSchema() { return ImdbBase::genSchema(213, "imdb109"); }
schemac::Table imdb110::genSchema() { return ImdbBase::genSchema(214, "imdb110"); }
schemac::Table imdb111::genSchema() { return ImdbBase::genSchema(215, "imdb111"); }
schemac::Table imdb112::genSchema() { return ImdbBase::genSchema(216, "imdb112"); }
schemac::Table imdb113::genSchema() { return ImdbBase::genSchema(217, "imdb113"); }
schemac::Table imdb114::genSchema() { return ImdbBase::genSchema(218, "imdb114"); }
schemac::Table imdb115::genSchema() { return ImdbBase::genSchema(219, "imdb115"); }
schemac::Table imdb116::genSchema() { return ImdbBase::genSchema(220, "imdb116"); }
schemac::Table imdb117::genSchema() { return ImdbBase::genSchema(221, "imdb117"); }
schemac::Table imdb118::genSchema() { return ImdbBase::genSchema(222, "imdb118"); }
schemac::Table imdb119::genSchema() { return ImdbBase::genSchema(223, "imdb119"); }
schemac::Table imdb120::genSchema() { return ImdbBase::genSchema(224, "imdb120"); }
schemac::Table imdb121::genSchema() { return ImdbBase::genSchema(225, "imdb121"); }
schemac::Table imdb122::genSchema() { return ImdbBase::genSchema(226, "imdb122"); }
schemac::Table imdb123::genSchema() { return ImdbBase::genSchema(227, "imdb123"); }
schemac::Table imdb124::genSchema() { return ImdbBase::genSchema(228, "imdb124"); }
schemac::Table imdb125::genSchema() { return ImdbBase::genSchema(229, "imdb125"); }
schemac::Table imdb126::genSchema() { return ImdbBase::genSchema(230, "imdb126"); }
schemac::Table imdb127::genSchema() { return ImdbBase::genSchema(231, "imdb127"); }
// ---------------------------------------------------------------------------------------------------
void imdb1::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb1, Container>(tblPath, separator); }
void imdb2::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb2, Container>(tblPath, separator); }
void imdb3::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb3, Container>(tblPath, separator); }
void imdb4::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb4, Container>(tblPath, separator); }
void imdb5::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb5, Container>(tblPath, separator); }
void imdb6::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb6, Container>(tblPath, separator); }
void imdb7::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb7, Container>(tblPath, separator); }
void imdb8::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb8, Container>(tblPath, separator); }
void imdb9::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb9, Container>(tblPath, separator); }
void imdb10::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb10, Container>(tblPath, separator); }
void imdb11::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb11, Container>(tblPath, separator); }
void imdb12::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb12, Container>(tblPath, separator); }
void imdb13::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb13, Container>(tblPath, separator); }
void imdb14::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb14, Container>(tblPath, separator); }
void imdb15::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb15, Container>(tblPath, separator); }
void imdb16::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb16, Container>(tblPath, separator); }
void imdb17::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb17, Container>(tblPath, separator); }
void imdb18::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb18, Container>(tblPath, separator); }
void imdb19::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb19, Container>(tblPath, separator); }
void imdb20::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb20, Container>(tblPath, separator); }
void imdb21::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb21, Container>(tblPath, separator); }
void imdb22::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb22, Container>(tblPath, separator); }
void imdb23::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb23, Container>(tblPath, separator); }
void imdb24::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb24, Container>(tblPath, separator); }
void imdb25::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb25, Container>(tblPath, separator); }
void imdb26::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb26, Container>(tblPath, separator); }
void imdb27::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb27, Container>(tblPath, separator); }
void imdb28::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb28, Container>(tblPath, separator); }
void imdb29::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb29, Container>(tblPath, separator); }
void imdb30::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb30, Container>(tblPath, separator); }
void imdb31::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb31, Container>(tblPath, separator); }
void imdb32::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb32, Container>(tblPath, separator); }
void imdb33::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb33, Container>(tblPath, separator); }
void imdb34::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb34, Container>(tblPath, separator); }
void imdb35::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb35, Container>(tblPath, separator); }
void imdb36::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb36, Container>(tblPath, separator); }
void imdb37::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb37, Container>(tblPath, separator); }
void imdb38::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb38, Container>(tblPath, separator); }
void imdb39::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb39, Container>(tblPath, separator); }
void imdb40::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb40, Container>(tblPath, separator); }
void imdb41::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb41, Container>(tblPath, separator); }
void imdb42::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb42, Container>(tblPath, separator); }
void imdb43::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb43, Container>(tblPath, separator); }
void imdb44::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb44, Container>(tblPath, separator); }
void imdb45::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb45, Container>(tblPath, separator); }
void imdb46::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb46, Container>(tblPath, separator); }
void imdb47::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb47, Container>(tblPath, separator); }
void imdb48::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb48, Container>(tblPath, separator); }
void imdb49::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb49, Container>(tblPath, separator); }
void imdb50::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb50, Container>(tblPath, separator); }
void imdb51::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb51, Container>(tblPath, separator); }
void imdb52::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb52, Container>(tblPath, separator); }
void imdb53::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb53, Container>(tblPath, separator); }
void imdb54::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb54, Container>(tblPath, separator); }
void imdb55::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb55, Container>(tblPath, separator); }
void imdb56::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb56, Container>(tblPath, separator); }
void imdb57::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb57, Container>(tblPath, separator); }
void imdb58::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb58, Container>(tblPath, separator); }
void imdb59::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb59, Container>(tblPath, separator); }
void imdb60::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb60, Container>(tblPath, separator); }
void imdb61::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb61, Container>(tblPath, separator); }
void imdb62::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb62, Container>(tblPath, separator); }
void imdb63::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb63, Container>(tblPath, separator); }
void imdb64::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb64, Container>(tblPath, separator); }
void imdb65::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb65, Container>(tblPath, separator); }
void imdb66::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb66, Container>(tblPath, separator); }
void imdb67::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb67, Container>(tblPath, separator); }
void imdb68::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb68, Container>(tblPath, separator); }
void imdb69::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb69, Container>(tblPath, separator); }
void imdb70::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb70, Container>(tblPath, separator); }
void imdb71::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb71, Container>(tblPath, separator); }
void imdb72::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb72, Container>(tblPath, separator); }
void imdb73::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb73, Container>(tblPath, separator); }
void imdb74::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb74, Container>(tblPath, separator); }
void imdb75::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb75, Container>(tblPath, separator); }
void imdb76::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb76, Container>(tblPath, separator); }
void imdb77::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb77, Container>(tblPath, separator); }
void imdb78::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb78, Container>(tblPath, separator); }
void imdb79::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb79, Container>(tblPath, separator); }
void imdb80::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb80, Container>(tblPath, separator); }
void imdb81::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb81, Container>(tblPath, separator); }
void imdb82::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb82, Container>(tblPath, separator); }
void imdb83::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb83, Container>(tblPath, separator); }
void imdb84::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb84, Container>(tblPath, separator); }
void imdb85::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb85, Container>(tblPath, separator); }
void imdb86::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb86, Container>(tblPath, separator); }
void imdb87::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb87, Container>(tblPath, separator); }
void imdb88::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb88, Container>(tblPath, separator); }
void imdb89::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb89, Container>(tblPath, separator); }
void imdb90::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb90, Container>(tblPath, separator); }
void imdb91::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb91, Container>(tblPath, separator); }
void imdb92::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb92, Container>(tblPath, separator); }
void imdb93::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb93, Container>(tblPath, separator); }
void imdb94::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb94, Container>(tblPath, separator); }
void imdb95::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb95, Container>(tblPath, separator); }
void imdb96::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb96, Container>(tblPath, separator); }
void imdb97::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb97, Container>(tblPath, separator); }
void imdb98::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb98, Container>(tblPath, separator); }
void imdb99::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb99, Container>(tblPath, separator); }
void imdb100::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb100, Container>(tblPath, separator); }
void imdb101::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb101, Container>(tblPath, separator); }
void imdb102::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb102, Container>(tblPath, separator); }
void imdb103::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb103, Container>(tblPath, separator); }
void imdb104::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb104, Container>(tblPath, separator); }
void imdb105::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb105, Container>(tblPath, separator); }
void imdb106::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb106, Container>(tblPath, separator); }
void imdb107::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb107, Container>(tblPath, separator); }
void imdb108::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb108, Container>(tblPath, separator); }
void imdb109::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb109, Container>(tblPath, separator); }
void imdb110::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb110, Container>(tblPath, separator); }
void imdb111::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb111, Container>(tblPath, separator); }
void imdb112::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb112, Container>(tblPath, separator); }
void imdb113::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb113, Container>(tblPath, separator); }
void imdb114::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb114, Container>(tblPath, separator); }
void imdb115::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb115, Container>(tblPath, separator); }
void imdb116::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb116, Container>(tblPath, separator); }
void imdb117::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb117, Container>(tblPath, separator); }
void imdb118::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb118, Container>(tblPath, separator); }
void imdb119::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb119, Container>(tblPath, separator); }
void imdb120::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb120, Container>(tblPath, separator); }
void imdb121::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb121, Container>(tblPath, separator); }
void imdb122::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb122, Container>(tblPath, separator); }
void imdb123::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb123, Container>(tblPath, separator); }
void imdb124::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb124, Container>(tblPath, separator); }
void imdb125::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb125, Container>(tblPath, separator); }
void imdb126::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb126, Container>(tblPath, separator); }
void imdb127::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<imdb127, Container>(tblPath, separator); }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::tables
// ---------------------------------------------------------------------------------------------------
