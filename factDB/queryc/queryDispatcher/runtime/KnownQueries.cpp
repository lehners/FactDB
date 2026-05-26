// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/queryDispatcher/runtime/KnownQueries.hpp"
#include "factDB/config.h"
#include "factDB/infra/Setting.hpp"
#include "factDB/util/DoOnDestruction.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ranges>
// ---------------------------------------------------------------------------------------------------
using namespace factDB::queryc;
// ---------------------------------------------------------------------------------------------------
std::string getQueriesListFilename() {
   return CURRENT_SRC_DIR "/factDB/gen/queries.txt";
}
// ---------------------------------------------------------------------------------------------------
std::string QueryProperties::genModeString() const {
   std::string modes = has(QueryFlag::Naive) ? "n" : "";
   modes += has(QueryFlag::Parallel) ? "p" : "";
   modes += has(QueryFlag::Inlined) ? "i" : "";
   modes += has(QueryFlag::CacheCountStar) ? "c" : "";
   modes += has(QueryFlag::BottomInserts) ? "b" : "";
   return modes;
}
// ---------------------------------------------------------------------------------------------------
QueryFlag QueryProperties::fromModeString(const std::string& str) {
   QueryFlag flags = QueryFlag::None;

   for (char c : str) {
      switch (c) {
         case 'n': flags |= QueryFlag::Naive; break;
         case 'p': flags |= QueryFlag::Parallel; break;
         case 'i': flags |= QueryFlag::Inlined; break;
         case 'c': flags |= QueryFlag::CacheCountStar; break;
         case 'b': flags |= QueryFlag::BottomInserts; break;
         default: break;
      }
   }
   return flags;
}
// ---------------------------------------------------------------------------------------------------
QueryProperties QueryProperties::fromCurrentSettings() {
   auto& queryname = SettingBase::getSetting<std::string>("queryname")->get();
   bool naiveMerge = SettingBase::getSetting<bool>("codegen.factorized.naiveMerge")->get();
   bool parallel = SettingBase::getSetting<bool>("codegen.parallelFor")->get();
   bool inlined = SettingBase::getSetting<bool>("codegen.factorized.inline")->get();
   bool cacheCountStar = SettingBase::getSetting<bool>("codegen.cacheCountStar")->get();
   bool bottomInserts = SettingBase::getSetting<bool>("opt.doBottomInserts")->get();
   ExecutionMode execMode = SettingBase::getSetting<ExecutionMode>("codegen.executionMode")->get();

   QueryFlag flag = QueryFlag::None;
   if (naiveMerge) flag |= QueryFlag::Naive;
   if (parallel) flag |= QueryFlag::Parallel;
   if (inlined) flag |= QueryFlag::Inlined;
   if (cacheCountStar) flag |= QueryFlag::CacheCountStar;
   if (bottomInserts) flag |= QueryFlag::BottomInserts;

   return {execMode, queryname, flag};
}
// ---------------------------------------------------------------------------------------------------
KnownQueries::KnownQueries() {
   fromQueryFile();
}
// ---------------------------------------------------------------------------------------------------
KnownQueries::~KnownQueries() {
   writeQueries();
}
// ---------------------------------------------------------------------------------------------------
void KnownQueries::addQuery(const QueryProperties& qp) {
   generatedQueries.emplace_back(qp);
}
// ---------------------------------------------------------------------------------------------------
void KnownQueries::fromQueryFile() {
   auto queriesListFilename = getQueriesListFilename();
   if (std::filesystem::exists(queriesListFilename)) {
      std::ifstream inFile(queriesListFilename);
      DoOnDestruction dod([&]() { inFile.close(); });
      if (!inFile) throw std::runtime_error("Error opening query list file");
      std::string line;
      while (std::getline(inFile, line)) {
         size_t firstPos = line.find(", ");
         size_t secondPos = line.find(", ", firstPos + 2);

         std::string a = line.substr(0, firstPos);
         std::string b = line.substr(firstPos + 2, secondPos - (firstPos + 2));
         std::string c = line.substr(secondPos + 2);

         queryc::ExecutionMode em;
         bool parse = queryc::executionModeParser.parse(em, a);
         if (!parse) throw std::runtime_error("unable to parse execution mode");
         generatedQueries.emplace_back(em, b, QueryProperties::fromModeString(c));
      }
   }
}
// ---------------------------------------------------------------------------------------------------
void KnownQueries::writeQueries() const {
   auto queriesListFilename = getQueriesListFilename();
   std::ofstream queryListFile(queriesListFilename, std::ofstream::trunc);
   DoOnDestruction dod([&]() { queryListFile.close(); });
   for (auto& qp : generatedQueries)
      queryListFile << queryc::executionModeParser.outputDescription(qp.mode) << ", " << qp.queryname << ", " << qp.genModeString() << std::endl;
}
// ---------------------------------------------------------------------------------------------------
void KnownQueries::sort() {
   std::sort(generatedQueries.begin(), generatedQueries.end());
}
// ---------------------------------------------------------------------------------------------------
void KnownQueries::eliminateDuplicates() {
   sort();
   auto last = std::unique(generatedQueries.begin(), generatedQueries.end());
   generatedQueries.erase(last, generatedQueries.end());
}
// ---------------------------------------------------------------------------------------------------
std::unordered_set<std::string_view> KnownQueries::querynames() const {
   std::unordered_set<std::string_view> names;
   for (auto& qp : generatedQueries)
      names.insert(qp.queryname);
   return names;
}
// ---------------------------------------------------------------------------------------------------
