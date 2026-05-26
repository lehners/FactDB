#include "factDB/gen/tables/table_dblp.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::tables {
// ---------------------------------------------------------------------------------------------------
// namespace {
// ---------------------------------------------------------------------------------------------------
// template<typename Container>
void DBLPBase::loadRelation(const std::string& path, char separator) {
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
RuntimeValue DBLPBase::get(size_t idx, const IU& iu) const {
   if ("s" == iu.column) {
      return data[idx].s().toRTV();
   } else if ("d" == iu.column) {
      return data[idx].d().toRTV();
   } else {
      unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
void DBLPBase::unloadRelation() {
   data.clear();
   loaded = false;
}
// ---------------------------------------------------------------------------------------------------
schemac::Table DBLPBase::genSchema(uint32_t id, std::string name) {
   std::vector<factDB::schemac::Column> cols = {
      {"s", schemac::Type::Integer()},
      {"d", schemac::Type::Integer()}};
   return {id, std::move(name), std::move(cols)};
}
// ---------------------------------------------------------------------------------------------------
schemac::Table dblp1::genSchema() { return DBLPBase::genSchema(4, "dblp1"); }
schemac::Table dblp2::genSchema() { return DBLPBase::genSchema(5, "dblp2"); }
schemac::Table dblp3::genSchema() { return DBLPBase::genSchema(6, "dblp3"); }
schemac::Table dblp4::genSchema() { return DBLPBase::genSchema(7, "dblp4"); }
schemac::Table dblp5::genSchema() { return DBLPBase::genSchema(8, "dblp5"); }
schemac::Table dblp6::genSchema() { return DBLPBase::genSchema(9, "dblp6"); }
schemac::Table dblp7::genSchema() { return DBLPBase::genSchema(10, "dblp7"); }
schemac::Table dblp8::genSchema() { return DBLPBase::genSchema(11, "dblp8"); }
schemac::Table dblp9::genSchema() { return DBLPBase::genSchema(12, "dblp9"); }
schemac::Table dblp10::genSchema() { return DBLPBase::genSchema(13, "dblp10"); }
schemac::Table dblp11::genSchema() { return DBLPBase::genSchema(14, "dblp11"); }
schemac::Table dblp12::genSchema() { return DBLPBase::genSchema(15, "dblp12"); }
schemac::Table dblp13::genSchema() { return DBLPBase::genSchema(16, "dblp13"); }
schemac::Table dblp14::genSchema() { return DBLPBase::genSchema(17, "dblp14"); }
schemac::Table dblp15::genSchema() { return DBLPBase::genSchema(18, "dblp15"); }
schemac::Table dblp16::genSchema() { return DBLPBase::genSchema(19, "dblp16"); }
schemac::Table dblp17::genSchema() { return DBLPBase::genSchema(20, "dblp17"); }
schemac::Table dblp18::genSchema() { return DBLPBase::genSchema(21, "dblp18"); }
schemac::Table dblp19::genSchema() { return DBLPBase::genSchema(22, "dblp19"); }
schemac::Table dblp20::genSchema() { return DBLPBase::genSchema(23, "dblp20"); }
schemac::Table dblp21::genSchema() { return DBLPBase::genSchema(24, "dblp21"); }
schemac::Table dblp22::genSchema() { return DBLPBase::genSchema(25, "dblp22"); }
schemac::Table dblp23::genSchema() { return DBLPBase::genSchema(26, "dblp23"); }
schemac::Table dblp24::genSchema() { return DBLPBase::genSchema(27, "dblp24"); }
schemac::Table dblp25::genSchema() { return DBLPBase::genSchema(28, "dblp25"); }
schemac::Table dblp26::genSchema() { return DBLPBase::genSchema(29, "dblp26"); }
schemac::Table dblp27::genSchema() { return DBLPBase::genSchema(30, "dblp27"); }
// ---------------------------------------------------------------------------------------------------
void dblp1::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp1, Container>(tblPath, separator); }
void dblp2::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp2, Container>(tblPath, separator); }
void dblp3::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp3, Container>(tblPath, separator); }
void dblp4::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp4, Container>(tblPath, separator); }
void dblp5::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp5, Container>(tblPath, separator); }
void dblp6::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp6, Container>(tblPath, separator); }
void dblp7::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp7, Container>(tblPath, separator); }
void dblp8::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp8, Container>(tblPath, separator); }
void dblp9::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp9, Container>(tblPath, separator); }
void dblp10::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp10, Container>(tblPath, separator); }
void dblp11::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp11, Container>(tblPath, separator); }
void dblp12::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp12, Container>(tblPath, separator); }
void dblp13::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp13, Container>(tblPath, separator); }
void dblp14::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp14, Container>(tblPath, separator); }
void dblp15::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp15, Container>(tblPath, separator); }
void dblp16::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp16, Container>(tblPath, separator); }
void dblp17::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp17, Container>(tblPath, separator); }
void dblp18::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp18, Container>(tblPath, separator); }
void dblp19::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp19, Container>(tblPath, separator); }
void dblp20::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp20, Container>(tblPath, separator); }
void dblp21::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp21, Container>(tblPath, separator); }
void dblp22::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp22, Container>(tblPath, separator); }
void dblp23::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp23, Container>(tblPath, separator); }
void dblp24::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp24, Container>(tblPath, separator); }
void dblp25::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp25, Container>(tblPath, separator); }
void dblp26::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp26, Container>(tblPath, separator); }
void dblp27::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<dblp27, Container>(tblPath, separator); }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::tables
// ---------------------------------------------------------------------------------------------------
