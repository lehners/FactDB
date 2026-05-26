#include "factDB/gen/tables/table_KRelationTestMany.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
void KRelationTestMany::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.iu0(), in, separator, false);
         updateElem(newData.iu1(), in, separator, false);
         updateElem(newData.iu2(), in, separator, false);
         updateElem(newData.iu3(), in, separator, false);
         updateElem(newData.iu4(), in, separator, false);
         updateElem(newData.iu5(), in, separator, false);
         updateElem(newData.iu6(), in, separator, false);
         updateElem(newData.iu7(), in, separator, false);
         updateElem(newData.iu8(), in, separator, false);
         updateElem(newData.iu9(), in, separator, false);
         updateElem(newData.iu10(), in, separator, false);
         updateElem(newData.iu11(), in, separator, false);
         updateElem(newData.iu12(), in, separator, false);
         updateElem(newData.iu13(), in, separator, false);
         updateElem(newData.iu14(), in, separator, false);
         updateElem(newData.iu15(), in, separator, false);
         updateElem(newData.iu16(), in, separator, false);
         updateElem(newData.iu17(), in, separator, false);
         updateElem(newData.iu18(), in, separator, false);
         updateElem(newData.iu19(), in, separator, false);
         updateElem(newData.iu20(), in, separator, false);
         updateElem(newData.iu21(), in, separator, false);
         updateElem(newData.iu22(), in, separator, false);
         updateElem(newData.iu23(), in, separator, false);
         updateElem(newData.iu24(), in, separator, false);
         updateElem(newData.iu25(), in, separator, false);
         updateElem(newData.iu26(), in, separator, false);
         updateElem(newData.iu27(), in, separator, false);
         updateElem(newData.iu28(), in, separator, false);
         updateElem(newData.iu29(), in, separator, false);
         updateElem(newData.iu30(), in, separator, false);
         updateElem(newData.iu31(), in, separator, false);
         updateElem(newData.iu32(), in, separator, false);
         updateElem(newData.iu33(), in, separator, false);
         updateElem(newData.iu34(), in, separator, false);
         updateElem(newData.iu35(), in, separator, false);
         updateElem(newData.iu36(), in, separator, false);
         updateElem(newData.iu37(), in, separator, false);
         updateElem(newData.iu38(), in, separator, false);
         updateElem(newData.iu39(), in, separator, false);
         updateElem(newData.iu40(), in, separator, false);
         updateElem(newData.iu41(), in, separator, false);
         updateElem(newData.iu42(), in, separator, false);
         updateElem(newData.iu43(), in, separator, false);
         updateElem(newData.iu44(), in, separator, false);
         updateElem(newData.iu45(), in, separator, false);
         updateElem(newData.iu46(), in, separator, false);
         updateElem(newData.iu47(), in, separator, false);
         updateElem(newData.iu48(), in, separator, false);
         updateElem(newData.iu49(), in, separator, false);
         updateElem(newData.iu50(), in, separator, false);
         updateElem(newData.iu51(), in, separator, false);
         updateElem(newData.iu52(), in, separator, false);
         updateElem(newData.iu53(), in, separator, false);
         updateElem(newData.iu54(), in, separator, false);
         updateElem(newData.iu55(), in, separator, false);
         updateElem(newData.iu56(), in, separator, false);
         updateElem(newData.iu57(), in, separator, false);
         updateElem(newData.iu58(), in, separator, false);
         updateElem(newData.iu59(), in, separator, false);
         updateElem(newData.iu60(), in, separator, false);
         updateElem(newData.iu61(), in, separator, false);
         updateElem(newData.iu62(), in, separator, false);
         updateElem(newData.iu63(), in, separator, false);
         updateElem(newData.iu64(), in, separator, false);
         updateElem(newData.iu65(), in, separator, false);
         updateElem(newData.iu66(), in, separator, false);
         updateElem(newData.iu67(), in, separator, false);
         updateElem(newData.iu68(), in, separator, false);
         updateElem(newData.iu69(), in, separator, false);
         updateElem(newData.iu70(), in, separator, false);
         updateElem(newData.iu71(), in, separator, false);
         updateElem(newData.iu72(), in, separator, false);
         updateElem(newData.iu73(), in, separator, false);
         updateElem(newData.iu74(), in, separator, false);
         updateElem(newData.iu75(), in, separator, false);
         updateElem(newData.iu76(), in, separator, false);
         updateElem(newData.iu77(), in, separator, false);
         updateElem(newData.iu78(), in, separator, false);
         updateElem(newData.iu79(), in, separator, false);
         updateElem(newData.iu80(), in, separator, false);
         updateElem(newData.iu81(), in, separator, false);
         updateElem(newData.iu82(), in, separator, false);
         updateElem(newData.iu83(), in, separator, false);
         updateElem(newData.iu84(), in, separator, false);
         updateElem(newData.iu85(), in, separator, false);
         updateElem(newData.iu86(), in, separator, false);
         updateElem(newData.iu87(), in, separator, false);
         updateElem(newData.iu88(), in, separator, false);
         updateElem(newData.iu89(), in, separator, false);
         updateElem(newData.iu90(), in, separator, false);
         updateElem(newData.iu91(), in, separator, false);
         updateElem(newData.iu92(), in, separator, false);
         updateElem(newData.iu93(), in, separator, false);
         updateElem(newData.iu94(), in, separator, false);
         updateElem(newData.iu95(), in, separator, false);
         updateElem(newData.iu96(), in, separator, false);
         updateElem(newData.iu97(), in, separator, false);
         updateElem(newData.iu98(), in, separator, false);
         updateElem(newData.iu99(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void KRelationTestMany::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue KRelationTestMany::get(size_t idx, const IU& iu) const {
   if ("iu0" == iu.column) {
      return data[idx].iu0().toRTV();
   } else if ("iu1" == iu.column) {
      return data[idx].iu1().toRTV();
   } else if ("iu2" == iu.column) {
      return data[idx].iu2().toRTV();
   } else if ("iu3" == iu.column) {
      return data[idx].iu3().toRTV();
   } else if ("iu4" == iu.column) {
      return data[idx].iu4().toRTV();
   } else if ("iu5" == iu.column) {
      return data[idx].iu5().toRTV();
   } else if ("iu6" == iu.column) {
      return data[idx].iu6().toRTV();
   } else if ("iu7" == iu.column) {
      return data[idx].iu7().toRTV();
   } else if ("iu8" == iu.column) {
      return data[idx].iu8().toRTV();
   } else if ("iu9" == iu.column) {
      return data[idx].iu9().toRTV();
   } else if ("iu10" == iu.column) {
      return data[idx].iu10().toRTV();
   } else if ("iu11" == iu.column) {
      return data[idx].iu11().toRTV();
   } else if ("iu12" == iu.column) {
      return data[idx].iu12().toRTV();
   } else if ("iu13" == iu.column) {
      return data[idx].iu13().toRTV();
   } else if ("iu14" == iu.column) {
      return data[idx].iu14().toRTV();
   } else if ("iu15" == iu.column) {
      return data[idx].iu15().toRTV();
   } else if ("iu16" == iu.column) {
      return data[idx].iu16().toRTV();
   } else if ("iu17" == iu.column) {
      return data[idx].iu17().toRTV();
   } else if ("iu18" == iu.column) {
      return data[idx].iu18().toRTV();
   } else if ("iu19" == iu.column) {
      return data[idx].iu19().toRTV();
   } else if ("iu20" == iu.column) {
      return data[idx].iu20().toRTV();
   } else if ("iu21" == iu.column) {
      return data[idx].iu21().toRTV();
   } else if ("iu22" == iu.column) {
      return data[idx].iu22().toRTV();
   } else if ("iu23" == iu.column) {
      return data[idx].iu23().toRTV();
   } else if ("iu24" == iu.column) {
      return data[idx].iu24().toRTV();
   } else if ("iu25" == iu.column) {
      return data[idx].iu25().toRTV();
   } else if ("iu26" == iu.column) {
      return data[idx].iu26().toRTV();
   } else if ("iu27" == iu.column) {
      return data[idx].iu27().toRTV();
   } else if ("iu28" == iu.column) {
      return data[idx].iu28().toRTV();
   } else if ("iu29" == iu.column) {
      return data[idx].iu29().toRTV();
   } else if ("iu30" == iu.column) {
      return data[idx].iu30().toRTV();
   } else if ("iu31" == iu.column) {
      return data[idx].iu31().toRTV();
   } else if ("iu32" == iu.column) {
      return data[idx].iu32().toRTV();
   } else if ("iu33" == iu.column) {
      return data[idx].iu33().toRTV();
   } else if ("iu34" == iu.column) {
      return data[idx].iu34().toRTV();
   } else if ("iu35" == iu.column) {
      return data[idx].iu35().toRTV();
   } else if ("iu36" == iu.column) {
      return data[idx].iu36().toRTV();
   } else if ("iu37" == iu.column) {
      return data[idx].iu37().toRTV();
   } else if ("iu38" == iu.column) {
      return data[idx].iu38().toRTV();
   } else if ("iu39" == iu.column) {
      return data[idx].iu39().toRTV();
   } else if ("iu40" == iu.column) {
      return data[idx].iu40().toRTV();
   } else if ("iu41" == iu.column) {
      return data[idx].iu41().toRTV();
   } else if ("iu42" == iu.column) {
      return data[idx].iu42().toRTV();
   } else if ("iu43" == iu.column) {
      return data[idx].iu43().toRTV();
   } else if ("iu44" == iu.column) {
      return data[idx].iu44().toRTV();
   } else if ("iu45" == iu.column) {
      return data[idx].iu45().toRTV();
   } else if ("iu46" == iu.column) {
      return data[idx].iu46().toRTV();
   } else if ("iu47" == iu.column) {
      return data[idx].iu47().toRTV();
   } else if ("iu48" == iu.column) {
      return data[idx].iu48().toRTV();
   } else if ("iu49" == iu.column) {
      return data[idx].iu49().toRTV();
   } else if ("iu50" == iu.column) {
      return data[idx].iu50().toRTV();
   } else if ("iu51" == iu.column) {
      return data[idx].iu51().toRTV();
   } else if ("iu52" == iu.column) {
      return data[idx].iu52().toRTV();
   } else if ("iu53" == iu.column) {
      return data[idx].iu53().toRTV();
   } else if ("iu54" == iu.column) {
      return data[idx].iu54().toRTV();
   } else if ("iu55" == iu.column) {
      return data[idx].iu55().toRTV();
   } else if ("iu56" == iu.column) {
      return data[idx].iu56().toRTV();
   } else if ("iu57" == iu.column) {
      return data[idx].iu57().toRTV();
   } else if ("iu58" == iu.column) {
      return data[idx].iu58().toRTV();
   } else if ("iu59" == iu.column) {
      return data[idx].iu59().toRTV();
   } else if ("iu60" == iu.column) {
      return data[idx].iu60().toRTV();
   } else if ("iu61" == iu.column) {
      return data[idx].iu61().toRTV();
   } else if ("iu62" == iu.column) {
      return data[idx].iu62().toRTV();
   } else if ("iu63" == iu.column) {
      return data[idx].iu63().toRTV();
   } else if ("iu64" == iu.column) {
      return data[idx].iu64().toRTV();
   } else if ("iu65" == iu.column) {
      return data[idx].iu65().toRTV();
   } else if ("iu66" == iu.column) {
      return data[idx].iu66().toRTV();
   } else if ("iu67" == iu.column) {
      return data[idx].iu67().toRTV();
   } else if ("iu68" == iu.column) {
      return data[idx].iu68().toRTV();
   } else if ("iu69" == iu.column) {
      return data[idx].iu69().toRTV();
   } else if ("iu70" == iu.column) {
      return data[idx].iu70().toRTV();
   } else if ("iu71" == iu.column) {
      return data[idx].iu71().toRTV();
   } else if ("iu72" == iu.column) {
      return data[idx].iu72().toRTV();
   } else if ("iu73" == iu.column) {
      return data[idx].iu73().toRTV();
   } else if ("iu74" == iu.column) {
      return data[idx].iu74().toRTV();
   } else if ("iu75" == iu.column) {
      return data[idx].iu75().toRTV();
   } else if ("iu76" == iu.column) {
      return data[idx].iu76().toRTV();
   } else if ("iu77" == iu.column) {
      return data[idx].iu77().toRTV();
   } else if ("iu78" == iu.column) {
      return data[idx].iu78().toRTV();
   } else if ("iu79" == iu.column) {
      return data[idx].iu79().toRTV();
   } else if ("iu80" == iu.column) {
      return data[idx].iu80().toRTV();
   } else if ("iu81" == iu.column) {
      return data[idx].iu81().toRTV();
   } else if ("iu82" == iu.column) {
      return data[idx].iu82().toRTV();
   } else if ("iu83" == iu.column) {
      return data[idx].iu83().toRTV();
   } else if ("iu84" == iu.column) {
      return data[idx].iu84().toRTV();
   } else if ("iu85" == iu.column) {
      return data[idx].iu85().toRTV();
   } else if ("iu86" == iu.column) {
      return data[idx].iu86().toRTV();
   } else if ("iu87" == iu.column) {
      return data[idx].iu87().toRTV();
   } else if ("iu88" == iu.column) {
      return data[idx].iu88().toRTV();
   } else if ("iu89" == iu.column) {
      return data[idx].iu89().toRTV();
   } else if ("iu90" == iu.column) {
      return data[idx].iu90().toRTV();
   } else if ("iu91" == iu.column) {
      return data[idx].iu91().toRTV();
   } else if ("iu92" == iu.column) {
      return data[idx].iu92().toRTV();
   } else if ("iu93" == iu.column) {
      return data[idx].iu93().toRTV();
   } else if ("iu94" == iu.column) {
      return data[idx].iu94().toRTV();
   } else if ("iu95" == iu.column) {
      return data[idx].iu95().toRTV();
   } else if ("iu96" == iu.column) {
      return data[idx].iu96().toRTV();
   } else if ("iu97" == iu.column) {
      return data[idx].iu97().toRTV();
   } else if ("iu98" == iu.column) {
      return data[idx].iu98().toRTV();
   } else if ("iu99" == iu.column) {
      return data[idx].iu99().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void KRelationTestMany::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<KRelationTestMany, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table KRelationTestMany::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"iu0", schemac::Type::Integer()},
      {"iu1", schemac::Type::Integer()},
      {"iu2", schemac::Type::Integer()},
      {"iu3", schemac::Type::Integer()},
      {"iu4", schemac::Type::Integer()},
      {"iu5", schemac::Type::Integer()},
      {"iu6", schemac::Type::Integer()},
      {"iu7", schemac::Type::Integer()},
      {"iu8", schemac::Type::Integer()},
      {"iu9", schemac::Type::Integer()},
      {"iu10", schemac::Type::Integer()},
      {"iu11", schemac::Type::Integer()},
      {"iu12", schemac::Type::Integer()},
      {"iu13", schemac::Type::Integer()},
      {"iu14", schemac::Type::Integer()},
      {"iu15", schemac::Type::Integer()},
      {"iu16", schemac::Type::Integer()},
      {"iu17", schemac::Type::Integer()},
      {"iu18", schemac::Type::Integer()},
      {"iu19", schemac::Type::Integer()},
      {"iu20", schemac::Type::Integer()},
      {"iu21", schemac::Type::Integer()},
      {"iu22", schemac::Type::Integer()},
      {"iu23", schemac::Type::Integer()},
      {"iu24", schemac::Type::Integer()},
      {"iu25", schemac::Type::Integer()},
      {"iu26", schemac::Type::Integer()},
      {"iu27", schemac::Type::Integer()},
      {"iu28", schemac::Type::Integer()},
      {"iu29", schemac::Type::Integer()},
      {"iu30", schemac::Type::Integer()},
      {"iu31", schemac::Type::Integer()},
      {"iu32", schemac::Type::Integer()},
      {"iu33", schemac::Type::Integer()},
      {"iu34", schemac::Type::Integer()},
      {"iu35", schemac::Type::Integer()},
      {"iu36", schemac::Type::Integer()},
      {"iu37", schemac::Type::Integer()},
      {"iu38", schemac::Type::Integer()},
      {"iu39", schemac::Type::Integer()},
      {"iu40", schemac::Type::Integer()},
      {"iu41", schemac::Type::Integer()},
      {"iu42", schemac::Type::Integer()},
      {"iu43", schemac::Type::Integer()},
      {"iu44", schemac::Type::Integer()},
      {"iu45", schemac::Type::Integer()},
      {"iu46", schemac::Type::Integer()},
      {"iu47", schemac::Type::Integer()},
      {"iu48", schemac::Type::Integer()},
      {"iu49", schemac::Type::Integer()},
      {"iu50", schemac::Type::Integer()},
      {"iu51", schemac::Type::Integer()},
      {"iu52", schemac::Type::Integer()},
      {"iu53", schemac::Type::Integer()},
      {"iu54", schemac::Type::Integer()},
      {"iu55", schemac::Type::Integer()},
      {"iu56", schemac::Type::Integer()},
      {"iu57", schemac::Type::Integer()},
      {"iu58", schemac::Type::Integer()},
      {"iu59", schemac::Type::Integer()},
      {"iu60", schemac::Type::Integer()},
      {"iu61", schemac::Type::Integer()},
      {"iu62", schemac::Type::Integer()},
      {"iu63", schemac::Type::Integer()},
      {"iu64", schemac::Type::Integer()},
      {"iu65", schemac::Type::Integer()},
      {"iu66", schemac::Type::Integer()},
      {"iu67", schemac::Type::Integer()},
      {"iu68", schemac::Type::Integer()},
      {"iu69", schemac::Type::Integer()},
      {"iu70", schemac::Type::Integer()},
      {"iu71", schemac::Type::Integer()},
      {"iu72", schemac::Type::Integer()},
      {"iu73", schemac::Type::Integer()},
      {"iu74", schemac::Type::Integer()},
      {"iu75", schemac::Type::Integer()},
      {"iu76", schemac::Type::Integer()},
      {"iu77", schemac::Type::Integer()},
      {"iu78", schemac::Type::Integer()},
      {"iu79", schemac::Type::Integer()},
      {"iu80", schemac::Type::Integer()},
      {"iu81", schemac::Type::Integer()},
      {"iu82", schemac::Type::Integer()},
      {"iu83", schemac::Type::Integer()},
      {"iu84", schemac::Type::Integer()},
      {"iu85", schemac::Type::Integer()},
      {"iu86", schemac::Type::Integer()},
      {"iu87", schemac::Type::Integer()},
      {"iu88", schemac::Type::Integer()},
      {"iu89", schemac::Type::Integer()},
      {"iu90", schemac::Type::Integer()},
      {"iu91", schemac::Type::Integer()},
      {"iu92", schemac::Type::Integer()},
      {"iu93", schemac::Type::Integer()},
      {"iu94", schemac::Type::Integer()},
      {"iu95", schemac::Type::Integer()},
      {"iu96", schemac::Type::Integer()},
      {"iu97", schemac::Type::Integer()},
      {"iu98", schemac::Type::Integer()},
      {"iu99", schemac::Type::Integer()}};
   return {412, "KRelationTestMany", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_KRelationTestMany() { return new KRelationTestMany(); }
/*---------------------------------------------------------------------------*/
void destroy_KRelationTestMany(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
