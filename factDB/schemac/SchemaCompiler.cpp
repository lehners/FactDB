// ---------------------------------------------------------------------------------------------------
#include "factDB/schemac/SchemaCompiler.hpp"
#include "factDB/Database.hpp"
#include "factDB/config.h"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <ranges>
// ---------------------------------------------------------------------------------------------------
namespace factDB::schemac {
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<bool> schemacColumnLayout("schemac.ColumnStore", !true);
// ---------------------------------------------------------------------------------------------------
std::string outputPath = CURRENT_SRC_DIR "/factDB/gen/tables/table";
// ---------------------------------------------------------------------------------------------------
std::string SchemaCompiler::getSrcPath() {
   return outputPath /*+ std::to_string(tableId)*/ + ".cpp";
}
// ---------------------------------------------------------------------------------------------------
std::string SchemaCompiler::getHeaderPath() {
   return outputPath + ".hpp";
}
// ---------------------------------------------------------------------------------------------------
std::string SchemaCompiler::getLibraryPath() {
   return outputPath + std::to_string(tableId) + ".cpp.so";
}
// ---------------------------------------------------------------------------------------------------
std::string fixPath(std::string libraryPath) {
   if (libraryPath.starts_with("dblp")) return "dblp";
   if (libraryPath.starts_with("epinions")) return "epinions";
   if (libraryPath.starts_with("hetio")) return "hetio";
   if (libraryPath.starts_with("imdb")) return "imdb";
   if (libraryPath.starts_with("watdiv")) return "watdiv";
   if (libraryPath.starts_with("yago")) return "yago";
   if (libraryPath.starts_with("WINF")) return "WINF";
   return libraryPath;
}
// ---------------------------------------------------------------------------------------------------
std::string SchemaCompiler::getSrcPath(const std::string& libraryPath) {
   return outputPath + "_" + fixPath(libraryPath) + ".cpp";
}
// ---------------------------------------------------------------------------------------------------
std::string SchemaCompiler::getHeaderIncludePath(const std::string& libraryPath) {
   return "factDB/gen/tables/table_" + fixPath(libraryPath) + ".hpp";
}
// ---------------------------------------------------------------------------------------------------
std::string SchemaCompiler::getHeaderPath(const std::string& libraryPath) {
   return outputPath + "_" + fixPath(libraryPath) + ".hpp";
}
// ---------------------------------------------------------------------------------------------------
std::string SchemaCompiler::getOutputFolder() {
   return std::filesystem::path(outputPath).parent_path().string();
}
// ---------------------------------------------------------------------------------------------------
SchemaCompiler::SchemaCompiler(const std::vector<std::string>& additionalHeaders, bool headerFullPath)
   : SchemaCompiler("", "", additionalHeaders, headerFullPath) {
}
// ---------------------------------------------------------------------------------------------------
SchemaCompiler::SchemaCompiler(const std::string& header_path_param, const std::string& source_path_param, const std::vector<std::string>& additional_headers, bool headerFullPath)
   : header(headerStream, FileWriter::Silent), source(sourceStream, FileWriter::Silent), tableId(++*SettingBase::getSetting<size_t>("globalTableIdx")) {
   constexpr auto prefixLength = std::string(CURRENT_SRC_DIR "/").size();
   std::string header_path = header_path_param.empty() ? getHeaderPath() : header_path_param;
   std::string source_path = source_path_param.empty() ? getSrcPath() : source_path_param;

   if (auto outFolder = getOutputFolder(); !std::filesystem::is_directory(outFolder))
      std::filesystem::create_directories(outFolder);

   headerStream = std::ofstream(header_path, std::ofstream::trunc);
   sourceStream = std::ofstream(source_path, std::ofstream::trunc);

   if (!headerStream.is_open()) {
      std::cerr << "Failed to open header file at \"" << header_path << "\"" << std::endl;
      assert(false);
   }

   if (!sourceStream.is_open()) {
      std::cerr << "Failed to open source file at \"" << source_path << "\"" << std::endl;
      assert(false);
   }

   std::cout << "successfully opened files at \"" << header_path << "\" and \"" << source_path << "\"" << std::endl;
   startFiles(header_path.substr(headerFullPath ? 0 : prefixLength), additional_headers);
}
// ---------------------------------------------------------------------------------------------------
SchemaCompiler::~SchemaCompiler() {
   finishFiles();
   headerStream.close();
   sourceStream.close();
}
// ---------------------------------------------------------------------------------------------------
FWContainer SchemaCompiler::headerGuard(const std::string& header_path) {
   auto cleaned_header_path = header_path;

   std::replace(cleaned_header_path.begin(), cleaned_header_path.end(), '/', '_');
   std::replace(cleaned_header_path.begin(), cleaned_header_path.end(), '.', '_');
   std::replace(cleaned_header_path.begin(), cleaned_header_path.end(), '-', '_');

   return fw::lc("H_FACTDB_FACT_DB_TABLES_H_", cleaned_header_path);
}
// ---------------------------------------------------------------------------------------------------
void SchemaCompiler::startFiles(const std::string& header_path, const std::vector<std::string>& additionalSrcHeaders) {
   header << "#pragma once" << fw::endl()
          << fw::context_switch() << fw::endl()
          << "#include \"factDB/schemac/BaseTable.hpp\"" << fw::endl()
          << "#include \"factDB/schemac/Table.hpp\"" << fw::endl()
          << "#include \"factDB/newftree/SingleNodeTree.hpp\"" << fw::endl()
          << fw::context_switch() << fw::endl()
          << "namespace factDB::tables {" << fw::endl()
          << fw::context_switch() << fw::endl();
   source << fw::fmt("#include \"{}\"", header_path) << fw::endl()
          << "#include \"factDB/infra/iu.hpp\"" << fw::endl()
          << "#include \"factDB/infra/types/RuntimeValue.hpp\"" << fw::endl()
          << "#include \"factDB/infra/Config.hpp\"" << fw::endl()
          << "#include <fstream>" << fw::endl()
          << "#include <filesystem>" << fw::endl();
   for (auto& additional_header : additionalSrcHeaders)
      source << fw::fmt("#include \"{}\"", additional_header) << fw::endl();
   source << fw::context_switch() << fw::endl()
          << "namespace factDB::tables {" << fw::endl()
          << fw::context_switch() << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void SchemaCompiler::finishFiles() {
   header << "} // namespace factDB::tables" << fw::endl()
          << fw::context_switch() << fw::endl();
   source << "} // namespace factDB::tables" << fw::endl()
          << fw::context_switch() << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void SchemaCompiler::generateTable(const factDB::schemac::Table& schema) {
   if constexpr (debugMode) {
      // check that no table is generated twice
      assert(!generatedTables.contains(schema.name));
      generatedTables.insert(schema.name);
   }

   header << fw::fmt("struct {} final : public BaseTable {{", schema.name) << fw::endl();
   generateColumnsEnum(schema);
   header << fw::endl();
   generateDataVectors(schema);

   header << fw::endl()
          << fw::fmt("~{}() override = default;", schema.name) << fw::endl()
          << fw::fmt("std::string_view name() const override  {{ return \"{}\"; }};", schema.name) << fw::endl();
   generateLoadRelation(schema);
   generateUnloadRelation(schema);
   generateSize(schema);
   generateGet(schema);
   generateTblFiles(schema);
   header // << "template <Columns col> auto& getValue(size_t idx);" << fw::endl()
      << fw::endl();
   generateTableInfos(schema);
   header << "};" << fw::endl()
          << fw::endl();
   // generateGetValueImpls(schema);
   header << fw::endl()
          << fw::fmt("factDB::BaseTable* {}();", getCreateFunName(schema), schema.name) << fw::endl()
          << fw::fmt("void destroy_{0}(factDB::BaseTable* ptr);", schema.name) << fw::endl()
          << fw::context_switch() << fw::endl();
   source << fw::fmt("factDB::BaseTable* {}() {{ return new {}(); }}", getCreateFunName(schema), schema.name) << fw::endl()
          << fw::context_switch() << fw::endl()
          << fw::fmt("void destroy_{}(factDB::BaseTable* ptr) {{ delete ptr; }}", schema.name) << fw::endl()
          << fw::context_switch() << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void SchemaCompiler::generateColumnsEnum(const factDB::schemac::Table& schema) {
   header << "enum Columns { " << fw::pushSeparator(", ");
   for (const auto& col : schema.columns)
      header << fw::separator() << col.name;
   header << "};" << fw::popSeparator() << fw::endl();
}
// --------------------------------------------------------------------------------------------------
void SchemaCompiler::generateDataVectors(const factDB::schemac::Table& schema) {
   if (schemacColumnLayout.get()) {
      for (const auto& col : schema.columns)
         header << fw::fmt("std::vector<{}> {}Data;", col.type, col.name) << fw::endl();
   } else {
      header << "struct Container {" << fw::endl();
      for (const auto& col : schema.columns)
         header << col.type << " " << col.name << ";" << fw::endl();

      auto iuTypes = fw::iter(schema.columns | std::views::transform([](const schemac::Column& c) { return c.type; }));
      header << fw::endl()
             << fw::fmt("using SingleNodeTreeType = SingleNodeTree<{}>;", iuTypes) << fw::endl()
             << "  const auto& asTuple() const { return SingleNodeTreeType::asTuple(*this); }" << fw::endl()
             << "  auto toFIterator() const { return SingleNodeTreeType::DefaultIterator(this->asTuple()); }" << fw::endl()
             << "};" << fw::endl()
             << fw::endl()
             << "std::vector<Container> data;" << fw::endl();
   }
}
// ---------------------------------------------------------------------------------------------------
void SchemaCompiler::generateSize(const factDB::schemac::Table& schema) {
   if (schemacColumnLayout.get()) {
      header << fw::fmt("size_t size() const override {{ return {}Data.size(); }}", schema.columns.front().name) << fw::endl();
   } else {
      header << "size_t size() const override { return data.size(); }" << fw::endl();
   }
}
// ---------------------------------------------------------------------------------------------------
void SchemaCompiler::generateLoadRelation(const factDB::schemac::Table& schema) {
   header << "void loadRelation(const std::string& in, char separator) override;" << fw::endl();

   source << fw::fmt("void {}::loadRelation(const std::string& path, char separator) {{", schema.name) << fw::endl()
          << fw::fmt("assert(!loaded);") << fw::endl()
          << fw::fmt("if (auto tblPath = path + \".tbl\"; std::filesystem::exists(tblPath)) {{ // load .tbl file if exists") << fw::endl()
          << fw::fmt("readTblFile<Container>(tblPath, data);") << fw::endl()
          << fw::fmt("}} else {{ ") << fw::endl()
          << fw::fmt("std::ifstream in(path);") << fw::endl()
          << fw::fmt("while (in.peek() != EOF) {{") << fw::endl();
   if (schemacColumnLayout.get()) {
      for (const auto& col : schema.columns)
         source << fw::fmt("updateElem({}Data.emplace_back(), in, separator, false);", col.name) << fw::endl();
   } else {
      source << "auto& newData = data.emplace_back();" << fw::endl();
      for (size_t idx = 0; const auto& col : schema.columns)
         source << fw::fmt("updateElem(newData.{}, in, separator, {});", col.name, ++idx == schema.columns.size() ? "true" : "false") << fw::endl();
   }
   source << fw::fmt("   }}") << fw::endl()
          << fw::fmt("   in.close();") << fw::endl()
          << fw::fmt("    }}") << fw::endl()
          << fw::fmt("   loaded = true;") << fw::endl()
          << fw::fmt("}}") << fw::endl()
          << fw::context_switch() << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void SchemaCompiler::generateUnloadRelation(const schemac::Table& schema) {
   header << "void unloadRelation() override;" << fw::endl();

   source << fw::fmt("void {}::unloadRelation() {{", schema.name) << fw::endl();
   if (schemacColumnLayout.get()) {
      for (const auto& col : schema.columns)
         source << fw::fmt("{}Data.clear();", col.type, col.name) << fw::endl();
   } else {
      source << "data.clear();" << fw::endl();
   }
   source << "loaded = false;" << fw::endl()
          << "}" << fw::endl()
          << fw::context_switch() << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void SchemaCompiler::generateGetValueImpls(const factDB::schemac::Table& schema) {
   if (schemacColumnLayout.get()) {
      for (const auto& col : schema.columns)
         header << fw::fmt("template <> INLINE auto& {0}::getValue<{0}::Columns::{1}>(size_t idx_) {{ return {1}Data[idx_]; }}", schema.name, col.name) << fw::endl();
   } else {
      for (const auto& col : schema.columns)
         header << fw::fmt("template <> INLINE auto& {0}::getValue<{0}::Columns::{1}>(size_t idx_) {{ return data[idx_].{1}; }}", schema.name, col.name) << fw::endl();
   }
}
// ---------------------------------------------------------------------------------------------------
void SchemaCompiler::generateTblFiles(const schemac::Table& schema) {
   header << fw::fmt("void genTblFiles(const std::string& tblPath, char separator) const override;") << fw::endl();
   source << fw::fmt("void {0}::genTblFiles(const std::string& tblPath, char separator) const  {{", schema.name) << fw::endl()
          << fw::fmt("  genTblFileImpl<{0}, Container>(tblPath, separator);", schema.name) << fw::endl()
          << fw::fmt("}}") << fw::endl()
          << fw::context_switch() << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void SchemaCompiler::generateGet(const schemac::Table& schema) {
   header << fw::fmt("RuntimeValue get(size_t idx, const IU& iu) const override;") << fw::endl();
   source << fw::fmt("RuntimeValue {}::get(size_t idx, const IU& iu) const {{", schema.name) << fw::endl();
   if (schemacColumnLayout.get()) {
      for (auto& col : schema.columns) {
         source << fw::fmt("if (\"{}\" == iu.column) {{", col.name) << fw::endl()
                << fw::fmt("  return {1}Data[idx].toRTV();", col.name) << fw::endl()
                << fw::fmt("}} else ");
      }
      source << "{ unreachable; }" << fw::endl();
   } else {
      for (size_t idx = 0; auto& col : schema.columns) {
         source << fw::fmt("if (\"{}\" == iu.column) {{", col.name) << fw::endl()
                << fw::fmt("  return data[idx].{}.toRTV();", col.name, idx++) << fw::endl()
                << fw::fmt("}} else ");
      }
      source << "{ unreachable(); }" << fw::endl();
   }
   source << "}" << fw::endl()
          << fw::context_switch() << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void SchemaCompiler::generateTableInfos(const schemac::Table& schema) {
   header << "static schemac::Table genSchema();" << fw::endl();

   source << fw::fmt("schemac::Table {}::genSchema() {{", schema.name) << fw::endl()
          << "std::vector<factDB::schemac::Column> cols = {" << fw::endl()
          << fw::pushSeparator(", ", true) << fw::checkQuotes(false);
   for (auto& col : schema.columns)
      source << fw::separator() << "{\"" << col.name << "\", schemac::Type::" << col.type.toConstructor() << "}";
   source << fw::endl()
          << "}" << fw::popSeparator() << fw::checkQuotes(true) << fw::sendl()
          << fw::fmt("return {{ {}, \"{}\", std::move(cols) }};", schema.id, schema.name) << fw::endl()
          << "}" << fw::endl()
          << fw::context_switch() << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void SchemaCompiler::generateGenTableInfos(const std::vector<schemac::Table>& schemas) {
   header << "std::vector<std::unique_ptr<BaseTable>> genTableInfos();" << fw::endl()
          << "std::vector<schemac::Table> genSchemas();" << fw::endl()
          << fw::context_switch() << fw::endl();

   source << "std::vector<std::unique_ptr<BaseTable>> genTableInfos() {" << fw::endl()
          << "  std::vector<std::unique_ptr<BaseTable>> tbls;" << fw::endl();
   for (const auto& tbl : schemas)
      source << fw::fmt("tbls.emplace_back(create_{}());", tbl.name) << fw::endl();
   source << "  return tbls;" << fw::endl()
          << "}" << fw::endl()
          << fw::context_switch() << fw::endl();

   source << "std::vector<schemac::Table> genSchemas() {" << fw::endl()
          << "  std::vector<schemac::Table> tables;" << fw::endl();
   for (const auto& schema : schemas)
      source << fw::fmt("tables.emplace_back({}::genSchema());", schema.name) << fw::endl();
   source << "  return tables;" << fw::endl()
          << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
std::string SchemaCompiler::getCreateFunName(const schemac::Table& schema) {
   return "create_" + schema.name;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::schemac
// ---------------------------------------------------------------------------------------------------