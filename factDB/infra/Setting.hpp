#ifndef H_FACTDB_INFRA_SETTING
#define H_FACTDB_INFRA_SETTING
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/SettingParser.hpp"
#include <cassert>
#include <string_view>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
template <typename T>
class Setting;

template <typename T>
struct ScopeSetter {
   Setting<T>& setting;
   T beforeValue;
   ScopeSetter(Setting<T>& settingParam, T val) : setting(settingParam), beforeValue(settingParam.get()) { setting.set(val); }
   ~ScopeSetter() { setting.set(beforeValue); }
};
// ---------------------------------------------------------------------------------------------------
class SettingBase {
   std::string_view name;

   public:
   SettingBase(std::string_view nameParam) noexcept : name(nameParam) { registerSetting(); }
   ~SettingBase();

   void registerSetting() noexcept;
   void unregisterSetting() noexcept;

   [[nodiscard]] inline bool operator==(const SettingBase& other) const { return name == other.name; }
   [[nodiscard]] inline std::string_view getName() const noexcept { return name; }

   [[nodiscard]] virtual bool interpret(std::string_view value) noexcept = 0;
   [[nodiscard]] virtual std::string getOutput() noexcept = 0;
   /// Get parser description
   [[nodiscard]] virtual std::string getParserDescription() const noexcept = 0;

   ScopeSetter<bool> setTemporaryBool(bool val);
   template <typename T>
   ScopeSetter<T> setTemporary(T val);

   static std::vector<std::reference_wrapper<SettingBase>> getAllSettings() noexcept;
   static void printAllSettings(std::ostream& stream) noexcept;
   static SettingBase* getSetting(std::string_view name) noexcept;
   template <class SettingType>
   static Setting<SettingType>* getSetting(std::string_view name, [[maybe_unused]] bool doAssert = false) noexcept {
      auto* s = static_cast<Setting<SettingType>*>(getSetting(name));
      assert(!doAssert || s != nullptr);
      return s;
   }
};
// ---------------------------------------------------------------------------------------------------
template <typename T>
class Setting : public SettingBase {
   T value;

   const SettingParser<T>* parser;

   public:
   [[nodiscard]] bool interpret(std::string_view nv) noexcept override { return parser->parse(value, nv); }
   [[nodiscard]] std::string getOutput() noexcept override { return parser->output(value); };
   [[nodiscard]] std::string getParserDescription() const noexcept override { return parser->parserDescription(); }

   Setting(std::string_view nameParam, T val, const SettingParser<T>* parser_ = &settinghelper::defaultParser<T>) : SettingBase(nameParam), value(std::move(val)), parser(parser_) {}
   void set(const T& val) { value = val; }
   ScopeSetter<T> setTemporary(const T& val) { return {*this, val}; }
   [[nodiscard]] const T& get() const { return value; }

   T operator++(int) noexcept {
      if constexpr (std::is_same_v<T, size_t>) {
         return value++;
      } else {
         static_assert(std::is_same_v<T, size_t>, "increment is only available for Settings with type size_t");
      }
   }
   T operator++() noexcept {
      if constexpr (std::is_same_v<T, size_t>) {
         return ++value;
      } else {
         static_assert(std::is_same_v<T, size_t>, "increment is only available for Settings with type size_t");
      }
   }
};
// ---------------------------------------------------------------------------------------------------
template <typename T>
ScopeSetter<T> SettingBase::setTemporary(T val) {
   auto* s = static_cast<Setting<T>*>(this);
   return {*s, val};
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_INFRA_SETTING