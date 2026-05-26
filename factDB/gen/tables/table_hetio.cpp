#include "factDB/gen/tables/table_hetio.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::tables {
// ---------------------------------------------------------------------------------------------------
void HetioBase::loadRelation(const std::string& path, char separator) {
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
RuntimeValue HetioBase::get(size_t idx, const IU& iu) const {
   if ("s" == iu.column) {
      return data[idx].s().toRTV();
   } else if ("d" == iu.column) {
      return data[idx].d().toRTV();
   } else {
      unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
void HetioBase::unloadRelation() {
   data.clear();
   loaded = false;
}
// ---------------------------------------------------------------------------------------------------
schemac::Table HetioBase::genSchema(uint32_t id, std::string name) {
   std::vector<factDB::schemac::Column> cols = {
      {"s", schemac::Type::Integer()},
      {"d", schemac::Type::Integer()}};
   return {id, name, std::move(cols)};
}
// ---------------------------------------------------------------------------------------------------
schemac::Table hetio45159::genSchema() { return HetioBase::genSchema(81, "hetio45159"); }
schemac::Table hetio45160::genSchema() { return HetioBase::genSchema(82, "hetio45160"); }
schemac::Table hetio45161::genSchema() { return HetioBase::genSchema(83, "hetio45161"); }
schemac::Table hetio45162::genSchema() { return HetioBase::genSchema(84, "hetio45162"); }
schemac::Table hetio45163::genSchema() { return HetioBase::genSchema(85, "hetio45163"); }
schemac::Table hetio45164::genSchema() { return HetioBase::genSchema(86, "hetio45164"); }
schemac::Table hetio45165::genSchema() { return HetioBase::genSchema(87, "hetio45165"); }
schemac::Table hetio45166::genSchema() { return HetioBase::genSchema(88, "hetio45166"); }
schemac::Table hetio45167::genSchema() { return HetioBase::genSchema(89, "hetio45167"); }
schemac::Table hetio45168::genSchema() { return HetioBase::genSchema(90, "hetio45168"); }
schemac::Table hetio45169::genSchema() { return HetioBase::genSchema(91, "hetio45169"); }
schemac::Table hetio45170::genSchema() { return HetioBase::genSchema(92, "hetio45170"); }
schemac::Table hetio45171::genSchema() { return HetioBase::genSchema(93, "hetio45171"); }
schemac::Table hetio45172::genSchema() { return HetioBase::genSchema(94, "hetio45172"); }
schemac::Table hetio45173::genSchema() { return HetioBase::genSchema(95, "hetio45173"); }
schemac::Table hetio45174::genSchema() { return HetioBase::genSchema(96, "hetio45174"); }
schemac::Table hetio45175::genSchema() { return HetioBase::genSchema(97, "hetio45175"); }
schemac::Table hetio45176::genSchema() { return HetioBase::genSchema(98, "hetio45176"); }
schemac::Table hetio45177::genSchema() { return HetioBase::genSchema(99, "hetio45177"); }
schemac::Table hetio45178::genSchema() { return HetioBase::genSchema(100, "hetio45178"); }
schemac::Table hetio45179::genSchema() { return HetioBase::genSchema(101, "hetio45179"); }
schemac::Table hetio45180::genSchema() { return HetioBase::genSchema(102, "hetio45180"); }
schemac::Table hetio45181::genSchema() { return HetioBase::genSchema(103, "hetio45181"); }
schemac::Table hetio45182::genSchema() { return HetioBase::genSchema(104, "hetio45182"); }
// ---------------------------------------------------------------------------------------------------
void hetio45159::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45159, Container>(tblPath, separator); }
void hetio45160::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45160, Container>(tblPath, separator); }
void hetio45161::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45161, Container>(tblPath, separator); }
void hetio45162::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45162, Container>(tblPath, separator); }
void hetio45163::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45163, Container>(tblPath, separator); }
void hetio45164::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45164, Container>(tblPath, separator); }
void hetio45165::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45165, Container>(tblPath, separator); }
void hetio45166::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45166, Container>(tblPath, separator); }
void hetio45167::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45167, Container>(tblPath, separator); }
void hetio45168::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45168, Container>(tblPath, separator); }
void hetio45169::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45169, Container>(tblPath, separator); }
void hetio45170::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45170, Container>(tblPath, separator); }
void hetio45171::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45171, Container>(tblPath, separator); }
void hetio45172::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45172, Container>(tblPath, separator); }
void hetio45173::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45173, Container>(tblPath, separator); }
void hetio45174::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45174, Container>(tblPath, separator); }
void hetio45175::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45175, Container>(tblPath, separator); }
void hetio45176::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45176, Container>(tblPath, separator); }
void hetio45177::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45177, Container>(tblPath, separator); }
void hetio45178::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45178, Container>(tblPath, separator); }
void hetio45179::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45179, Container>(tblPath, separator); }
void hetio45180::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45180, Container>(tblPath, separator); }
void hetio45181::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45181, Container>(tblPath, separator); }
void hetio45182::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<hetio45182, Container>(tblPath, separator); }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::tables
// ---------------------------------------------------------------------------------------------------
