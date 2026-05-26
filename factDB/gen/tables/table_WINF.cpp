#include "factDB/gen/tables/table_WINF.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include <filesystem>
#include <fstream>
/*---------------------------------------------------------------------------*/
namespace factDB::tables {
/*---------------------------------------------------------------------------*/
void WINF_A::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.k_a(), in, separator, false);
         updateElem(newData.a(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void WINF_A::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue WINF_A::get(size_t idx, const IU& iu) const {
   if ("k_a" == iu.column) {
      return data[idx].k_a().toRTV();
   } else if ("a" == iu.column) {
      return data[idx].a().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void WINF_A::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<WINF_A, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table WINF_A::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"k_a", schemac::Type::Integer()},
      {"a", schemac::Type::Integer()}};
   return {418, "WINF_A", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_WINF_A() { return new WINF_A(); }
/*---------------------------------------------------------------------------*/
void destroy_WINF_A(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
// ----------------- B -----------------
/*---------------------------------------------------------------------------*/
void WINF_B::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.k_b(), in, separator, false);
         updateElem(newData.b(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void WINF_B::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue WINF_B::get(size_t idx, const IU& iu) const {
   if ("k_b" == iu.column) {
      return data[idx].k_b().toRTV();
   } else if ("b" == iu.column) {
      return data[idx].b().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void WINF_B::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<WINF_B, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table WINF_B::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"k_b", schemac::Type::Integer()},
      {"b", schemac::Type::Integer()}};
   return {419, "WINF_B", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_WINF_B() { return new WINF_B(); }
/*---------------------------------------------------------------------------*/
void destroy_WINF_B(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
// ----------------- C -----------------
/*---------------------------------------------------------------------------*/
void WINF_C::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.k_c(), in, separator, false);
         updateElem(newData.c(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void WINF_C::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue WINF_C::get(size_t idx, const IU& iu) const {
   if ("k_c" == iu.column) {
      return data[idx].k_c().toRTV();
   } else if ("c" == iu.column) {
      return data[idx].c().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void WINF_C::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<WINF_C, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table WINF_C::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"k_c", schemac::Type::Integer()},
      {"c", schemac::Type::Integer()}};
   return {420, "WINF_C", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_WINF_C() { return new WINF_C(); }
/*---------------------------------------------------------------------------*/
void destroy_WINF_C(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
// ----------------- D -----------------
/*---------------------------------------------------------------------------*/
void WINF_D::loadRelation(const std::string& path, char separator) {
   assert(!loaded);
   if (auto tblPath = path + ".tbl"; std::filesystem::exists(tblPath)) { // load .tbl file if exists
      readTblFile<Container>(tblPath, data);
   } else {
      std::ifstream in(path);
      while (in.peek() != EOF) {
         auto& newData = data.emplace_back();
         updateElem(newData.k_d(), in, separator, false);
         updateElem(newData.d(), in, separator, true);
      }
      in.close();
   }
   loaded = true;
}
/*---------------------------------------------------------------------------*/
void WINF_D::unloadRelation() {
   data.clear();
   loaded = false;
}
/*---------------------------------------------------------------------------*/
RuntimeValue WINF_D::get(size_t idx, const IU& iu) const {
   if ("k_d" == iu.column) {
      return data[idx].k_d().toRTV();
   } else if ("d" == iu.column) {
      return data[idx].d().toRTV();
   } else {
      unreachable();
   }
}
/*---------------------------------------------------------------------------*/
void WINF_D::genTblFiles(const std::string& tblPath, char separator) const {
   genTblFileImpl<WINF_D, Container>(tblPath, separator);
}
/*---------------------------------------------------------------------------*/
schemac::Table WINF_D::genSchema() {
   std::vector<factDB::schemac::Column> cols = {
      {"k_d", schemac::Type::Integer()},
      {"d", schemac::Type::Integer()}};
   return {421, "WINF_D", std::move(cols)};
}
/*---------------------------------------------------------------------------*/
factDB::BaseTable* create_WINF_D() { return new WINF_D(); }
/*---------------------------------------------------------------------------*/
void destroy_WINF_D(factDB::BaseTable* ptr) { delete ptr; }
/*---------------------------------------------------------------------------*/
} // namespace factDB::tables
/*---------------------------------------------------------------------------*/
