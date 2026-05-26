#include "factDB/gen/tables/table_epinions.hpp"
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
void EpinionsBase::loadRelation(const std::string& path, char separator) {
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
RuntimeValue EpinionsBase::get(size_t idx, const IU& iu) const {
   if ("s" == iu.column) {
      return data[idx].s().toRTV();
   } else if ("d" == iu.column) {
      return data[idx].d().toRTV();
   } else {
      unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
void EpinionsBase::unloadRelation() {
   data.clear();
   loaded = false;
}
// ---------------------------------------------------------------------------------------------------
schemac::Table EpinionsBase::genSchema(uint32_t id, std::string name) {
   std::vector<factDB::schemac::Column> cols = {
      {"s", schemac::Type::Integer()},
      {"d", schemac::Type::Integer()}};
   return {id, name, std::move(cols)};
}
// ---------------------------------------------------------------------------------------------------
schemac::Table epinions75888::genSchema() { return EpinionsBase::genSchema(31, "epinions75888"); }
schemac::Table epinions75889::genSchema() { return EpinionsBase::genSchema(32, "epinions75889"); }
schemac::Table epinions75890::genSchema() { return EpinionsBase::genSchema(33, "epinions75890"); }
schemac::Table epinions75891::genSchema() { return EpinionsBase::genSchema(34, "epinions75891"); }
schemac::Table epinions75892::genSchema() { return EpinionsBase::genSchema(35, "epinions75892"); }
schemac::Table epinions75893::genSchema() { return EpinionsBase::genSchema(36, "epinions75893"); }
schemac::Table epinions75894::genSchema() { return EpinionsBase::genSchema(37, "epinions75894"); }
schemac::Table epinions75895::genSchema() { return EpinionsBase::genSchema(38, "epinions75895"); }
schemac::Table epinions75896::genSchema() { return EpinionsBase::genSchema(39, "epinions75896"); }
schemac::Table epinions75897::genSchema() { return EpinionsBase::genSchema(40, "epinions75897"); }
schemac::Table epinions75898::genSchema() { return EpinionsBase::genSchema(41, "epinions75898"); }
schemac::Table epinions75899::genSchema() { return EpinionsBase::genSchema(42, "epinions75899"); }
schemac::Table epinions75900::genSchema() { return EpinionsBase::genSchema(43, "epinions75900"); }
schemac::Table epinions75901::genSchema() { return EpinionsBase::genSchema(44, "epinions75901"); }
schemac::Table epinions75902::genSchema() { return EpinionsBase::genSchema(45, "epinions75902"); }
schemac::Table epinions75903::genSchema() { return EpinionsBase::genSchema(46, "epinions75903"); }
schemac::Table epinions75904::genSchema() { return EpinionsBase::genSchema(47, "epinions75904"); }
schemac::Table epinions75905::genSchema() { return EpinionsBase::genSchema(48, "epinions75905"); }
schemac::Table epinions75906::genSchema() { return EpinionsBase::genSchema(49, "epinions75906"); }
schemac::Table epinions75907::genSchema() { return EpinionsBase::genSchema(50, "epinions75907"); }
schemac::Table epinions75908::genSchema() { return EpinionsBase::genSchema(51, "epinions75908"); }
schemac::Table epinions75909::genSchema() { return EpinionsBase::genSchema(52, "epinions75909"); }
schemac::Table epinions75910::genSchema() { return EpinionsBase::genSchema(53, "epinions75910"); }
schemac::Table epinions75911::genSchema() { return EpinionsBase::genSchema(54, "epinions75911"); }
schemac::Table epinions75912::genSchema() { return EpinionsBase::genSchema(55, "epinions75912"); }
schemac::Table epinions75913::genSchema() { return EpinionsBase::genSchema(56, "epinions75913"); }
schemac::Table epinions75914::genSchema() { return EpinionsBase::genSchema(57, "epinions75914"); }
schemac::Table epinions75915::genSchema() { return EpinionsBase::genSchema(58, "epinions75915"); }
schemac::Table epinions75916::genSchema() { return EpinionsBase::genSchema(59, "epinions75916"); }
schemac::Table epinions75917::genSchema() { return EpinionsBase::genSchema(60, "epinions75917"); }
schemac::Table epinions75918::genSchema() { return EpinionsBase::genSchema(61, "epinions75918"); }
schemac::Table epinions75919::genSchema() { return EpinionsBase::genSchema(62, "epinions75919"); }
schemac::Table epinions75920::genSchema() { return EpinionsBase::genSchema(63, "epinions75920"); }
schemac::Table epinions75921::genSchema() { return EpinionsBase::genSchema(64, "epinions75921"); }
schemac::Table epinions75922::genSchema() { return EpinionsBase::genSchema(65, "epinions75922"); }
schemac::Table epinions75923::genSchema() { return EpinionsBase::genSchema(66, "epinions75923"); }
schemac::Table epinions75924::genSchema() { return EpinionsBase::genSchema(67, "epinions75924"); }
schemac::Table epinions75925::genSchema() { return EpinionsBase::genSchema(68, "epinions75925"); }
schemac::Table epinions75926::genSchema() { return EpinionsBase::genSchema(69, "epinions75926"); }
schemac::Table epinions75927::genSchema() { return EpinionsBase::genSchema(70, "epinions75927"); }
schemac::Table epinions75928::genSchema() { return EpinionsBase::genSchema(71, "epinions75928"); }
schemac::Table epinions75929::genSchema() { return EpinionsBase::genSchema(72, "epinions75929"); }
schemac::Table epinions75930::genSchema() { return EpinionsBase::genSchema(73, "epinions75930"); }
schemac::Table epinions75931::genSchema() { return EpinionsBase::genSchema(74, "epinions75931"); }
schemac::Table epinions75932::genSchema() { return EpinionsBase::genSchema(75, "epinions75932"); }
schemac::Table epinions75933::genSchema() { return EpinionsBase::genSchema(76, "epinions75933"); }
schemac::Table epinions75934::genSchema() { return EpinionsBase::genSchema(77, "epinions75934"); }
schemac::Table epinions75935::genSchema() { return EpinionsBase::genSchema(78, "epinions75935"); }
schemac::Table epinions75936::genSchema() { return EpinionsBase::genSchema(79, "epinions75936"); }
schemac::Table epinions75937::genSchema() { return EpinionsBase::genSchema(80, "epinions75937"); }
// ---------------------------------------------------------------------------------------------------
void epinions75888::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75888, Container>(tblPath, separator); }
void epinions75889::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75889, Container>(tblPath, separator); }
void epinions75890::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75890, Container>(tblPath, separator); }
void epinions75891::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75891, Container>(tblPath, separator); }
void epinions75892::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75892, Container>(tblPath, separator); }
void epinions75893::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75893, Container>(tblPath, separator); }
void epinions75894::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75894, Container>(tblPath, separator); }
void epinions75895::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75895, Container>(tblPath, separator); }
void epinions75896::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75896, Container>(tblPath, separator); }
void epinions75897::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75897, Container>(tblPath, separator); }
void epinions75898::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75898, Container>(tblPath, separator); }
void epinions75899::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75899, Container>(tblPath, separator); }
void epinions75900::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75900, Container>(tblPath, separator); }
void epinions75901::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75901, Container>(tblPath, separator); }
void epinions75902::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75902, Container>(tblPath, separator); }
void epinions75903::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75903, Container>(tblPath, separator); }
void epinions75904::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75904, Container>(tblPath, separator); }
void epinions75905::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75905, Container>(tblPath, separator); }
void epinions75906::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75906, Container>(tblPath, separator); }
void epinions75907::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75907, Container>(tblPath, separator); }
void epinions75908::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75908, Container>(tblPath, separator); }
void epinions75909::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75909, Container>(tblPath, separator); }
void epinions75910::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75910, Container>(tblPath, separator); }
void epinions75911::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75911, Container>(tblPath, separator); }
void epinions75912::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75912, Container>(tblPath, separator); }
void epinions75913::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75913, Container>(tblPath, separator); }
void epinions75914::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75914, Container>(tblPath, separator); }
void epinions75915::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75915, Container>(tblPath, separator); }
void epinions75916::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75916, Container>(tblPath, separator); }
void epinions75917::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75917, Container>(tblPath, separator); }
void epinions75918::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75918, Container>(tblPath, separator); }
void epinions75919::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75919, Container>(tblPath, separator); }
void epinions75920::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75920, Container>(tblPath, separator); }
void epinions75921::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75921, Container>(tblPath, separator); }
void epinions75922::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75922, Container>(tblPath, separator); }
void epinions75923::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75923, Container>(tblPath, separator); }
void epinions75924::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75924, Container>(tblPath, separator); }
void epinions75925::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75925, Container>(tblPath, separator); }
void epinions75926::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75926, Container>(tblPath, separator); }
void epinions75927::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75927, Container>(tblPath, separator); }
void epinions75928::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75928, Container>(tblPath, separator); }
void epinions75929::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75929, Container>(tblPath, separator); }
void epinions75930::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75930, Container>(tblPath, separator); }
void epinions75931::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75931, Container>(tblPath, separator); }
void epinions75932::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75932, Container>(tblPath, separator); }
void epinions75933::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75933, Container>(tblPath, separator); }
void epinions75934::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75934, Container>(tblPath, separator); }
void epinions75935::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75935, Container>(tblPath, separator); }
void epinions75936::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75936, Container>(tblPath, separator); }
void epinions75937::genTblFiles(const std::string& tblPath, char separator) const { genTblFileImpl<epinions75937, Container>(tblPath, separator); }
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::tables
// ---------------------------------------------------------------------------------------------------
