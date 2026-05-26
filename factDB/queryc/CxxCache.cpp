// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/CxxCache.hpp"
#include "factDB/config.h"
#include "factDB/infra/Setting.hpp"
#include "factDB/queryc/QueryParseContext.hpp"
#include "factDB/schemac/SchemaCompiler.hpp"
#include "factDB/util/FileReader.hpp"
#include "fmt/format.h"
#include <filesystem>
#include <iostream>
#include <string_view>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
// ---------------------------------------------------------------------------------------------------
using namespace factDB::queryc;
using namespace std;
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<bool> runRepeats("codegen.buffer.active"sv, false);
static factDB::Setting<size_t> globalQueryIdx("globalQueryIdx"sv, 0);
static factDB::Setting<size_t> globalTableIdx("globalTableIdx", 0);
// ---------------------------------------------------------------------------------------------------
std::tuple<size_t, bool> parseID(std::string_view prefix, const std::string& filename) {
   if (!filename.starts_with(prefix))
      return {0, false};
   size_t cur_val = 0;
   std::string_view sv_filename = std::string_view(filename).substr(prefix.size());
   for (char i : sv_filename) {
      if ('0' <= i && i <= '9')
         cur_val = 10 * cur_val + i - '0';
      else
         break;
   }
   return {cur_val, true};
}
// ---------------------------------------------------------------------------------------------------
void updateMax(std::string_view prefix, const std::string& filename, size_t& var) {
   auto [idx, success] = parseID(prefix, filename);
   var = (success && idx > var) ? idx : var;
}
// ---------------------------------------------------------------------------------------------------
} // anonymous namespace
// ---------------------------------------------------------------------------------------------------
std::tuple<size_t, size_t> CxxCache::loadFolder(const std::string_view& path) {
   size_t maxQuery = 0, maxTable = 0;
   if (!std::filesystem::exists(path))
      return {0, 0};
   for (auto& entry : std::filesystem::directory_iterator(path)) {
      if (entry.path().extension() == ".cpp") {
         bool added = add(entry.path());
         auto filename = entry.path().filename().string();
         if (added) {
            updateMax("query", filename, maxQuery);
            updateMax("table", filename, maxTable);
         }
      }
   }
   return {maxQuery, maxTable};
}
// ---------------------------------------------------------------------------------------------------
void CxxCache::load() {
   auto dst = fmt::format("{}/gen", CURRENT_BINARY_DIR);

   auto [maxQuery1, maxTable1] = loadFolder(QueryParseContext::getOutputFolder());
   auto [maxQuery2, maxTable2] = loadFolder(schemac::SchemaCompiler::getOutputFolder());

   SettingBase::getSetting<size_t>("globalQueryIdx")->set(std::max(maxQuery1, maxQuery2));
   SettingBase::getSetting<size_t>("globalTableIdx")->set(std::max(maxTable1, maxTable2));
}
// ---------------------------------------------------------------------------------------------------
bool CxxCache::add(const std::filesystem::path& srcFile) {
   auto compiledFile = std::string(srcFile) + ".so";
   return add(srcFile, compiledFile);
}
// ---------------------------------------------------------------------------------------------------
bool CxxCache::add(const std::string& srcFile, const std::string& soFile) {
   if (!std::filesystem::exists(srcFile) || !std::filesystem::exists(soFile))
      return false;

   auto soFileWrite = std::filesystem::last_write_time(soFile);
   auto srcFileWrite = std::filesystem::last_write_time(srcFile);

   if (srcFileWrite >= soFileWrite) { // .cpp was modified after .so => ignore this file
      return false;
   }

   util::FileReader cppContent(srcFile);
   if (!cxx2file.contains(*cppContent)) {
      cxx2file[cppContent.str()] = srcFile;
      file2so[srcFile] = {soFile, soFileWrite};
   }
   return true;
}
// ---------------------------------------------------------------------------------------------------
const std::string* CxxCache::operator[](const std::string& filename) {
   util::FileReader cppContent(filename);

   if (!cxx2file.contains(cppContent.sv())) // key was not contained
      return nullptr;
   auto cppFile = cxx2file.find(cppContent.sv())->second;

   if (!file2so.contains(cppFile))
      return nullptr;
   auto& [soFileName, soFileWrite] = file2so[cppFile];
   if (soFileWrite != std::filesystem::last_write_time(soFileName)) // file was written in the meantime
      return nullptr;
   return &soFileName;
}
// ---------------------------------------------------------------------------------------------------
static CxxCache cache;
// ---------------------------------------------------------------------------------------------------
CxxCache& CxxCache::getCache() {
   return cache;
}
// ---------------------------------------------------------------------------------------------------
