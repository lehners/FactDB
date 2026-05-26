#include "factDB/infra/Setting.hpp"
#include "factDB/infra/SettingParser.hpp"
#include "fmt/format.h"
#include <cassert>
#include <iostream>
#include <mutex>
#include <unordered_map>
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<bool> doCompactDuringMerge("infra.ChunkedList.CompactDuringMerge", false);
// ---------------------------------------------------------------------------------------------------
using namespace std;
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct SettingMap {
   std::mutex mutex;
   std::unordered_map<string_view, SettingBase*> settings;
};
// ---------------------------------------------------------------------------------------------------
static SettingMap& getSettingMap() noexcept {
   static SettingMap sm;
   return sm;
}
// ---------------------------------------------------------------------------------------------------
void SettingBase::registerSetting() noexcept {
   auto& settingMap = getSettingMap();
   lock_guard<mutex> guard(settingMap.mutex);

   assert(!settingMap.settings.contains(name) && "Setting already in use");
   settingMap.settings[name] = this;
}
// ---------------------------------------------------------------------------------------------------
void SettingBase::unregisterSetting() noexcept {
   auto& settingMap = getSettingMap();
   lock_guard<mutex> guard(settingMap.mutex);

   assert(settingMap.settings.contains(name) && "Setting does not exist.");
   settingMap.settings.erase(name);
}
// ---------------------------------------------------------------------------------------------------
ScopeSetter<bool> SettingBase::setTemporaryBool(bool val) {
   auto* s = static_cast<Setting<bool>*>(this);
   return {*s, val};
}
// ---------------------------------------------------------------------------------------------------
SettingBase::~SettingBase() {
   unregisterSetting();
}
// ---------------------------------------------------------------------------------------------------
std::vector<reference_wrapper<SettingBase>> SettingBase::getAllSettings() noexcept {
   auto& settingMap = getSettingMap();
   lock_guard<mutex> guard(settingMap.mutex);

   vector<reference_wrapper<SettingBase>> returnValues;
   returnValues.reserve(settingMap.settings.size());
   for (const auto& [key, value] : settingMap.settings)
      returnValues.emplace_back(*value);
   return returnValues;
}
// ---------------------------------------------------------------------------------------------------
void SettingBase::printAllSettings(std::ostream& stream) noexcept {
   auto& settingMap = getSettingMap();
   lock_guard<mutex> guard(settingMap.mutex);

   vector<reference_wrapper<SettingBase>> returnValues;
   returnValues.reserve(settingMap.settings.size());
   for (const auto& [key, value] : settingMap.settings)
      stream << "\'" << key << "\'"
             << ": " << value << std::endl;
}
// ---------------------------------------------------------------------------------------------------
SettingBase* SettingBase::getSetting(std::string_view name) noexcept {
   auto& settingMap = getSettingMap();
   lock_guard<mutex> guard(settingMap.mutex);

   auto it = settingMap.settings.find(name);
   if (it == settingMap.settings.end()) return nullptr;
   return it->second;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------