#pragma once
// ---------------------------------------------------------------------------------------------------
// Umbra
// (c) 2017 Thomas Neumann
// ---------------------------------------------------------------------------------------------------
#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
template <class T>
class SettingParser {
   public:
   /// Parse given string to value
   [[nodiscard]] virtual bool parse(T& v, std::string_view nv) const noexcept = 0;
   /// Description for parser
   [[nodiscard]] virtual std::string parserDescription() const noexcept = 0;
   /// Generate readable output string from value
   [[nodiscard]] virtual std::string output(const T& v) const noexcept = 0;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
namespace factDB::settinghelper {
// ---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------
/// Helper class for parsing enums
template <typename T, std::size_t N>
class EnumParser final : public SettingParser<T> {
   public:
   // An element consists of (value, description, input character)
   using Element = std::tuple<T, std::string_view, char>;
   /// The elements
   std::array<Element, N> elements;

   /// Constructor
   constexpr EnumParser(std::array<Element, N> elements_) : elements(std::move(elements_)) {}

   /// Parse value
   [[nodiscard]] bool parse(T& v, std::string_view nv) const noexcept override {
      if (nv.empty()) return false;
      for (const auto& [e, d, c] : elements) {
         if (nv[0] == c) {
            v = e;
            return true;
         }
         if (nv == d) {
            v = e;
            return true;
         }
      }
      return false;
   }
   /// Parser description
   [[nodiscard]] std::string parserDescription() const noexcept override {
      std::string description;
      bool first = true;
      for (const auto& [e, d, c] : elements) {
         if (first)
            first = false;
         else
            description.append(", ");
         description += d;
         description.append(":'");
         description += c;
         description.append("'");
      }
      return description;
   }

   /// Get output character
   [[nodiscard]] std::string outputDescription(const T& v) const noexcept {
      for (const auto& [e, d, c] : elements) {
         if (v == e) {
            return std::string(d);
         }
      }
      return "?"; // unreachable, unless value is corrupted
   }
   /// Get output string
   [[nodiscard]] std::string output(const T& v) const noexcept override { return outputDescription(v); }
};
//---------------------------------------------------------------------------
template <class... Ts>
concept all_same =
   sizeof
...(Ts) < 2 ||
   std::conjunction_v<
      std::is_same<std::tuple_element_t<0, std::tuple<Ts...>>, Ts>...>;
//---------------------------------------------------------------------------
template <typename... Elements>
   requires all_same<Elements...>
constexpr auto makeEnumParser(Elements... elements) noexcept
// Build a parser for enums, takes tuples of form (T value, string desc, char inputCharacter)
{
   using T = typename std::tuple_element<0, typename std::tuple_element<0, std::tuple<Elements...>>::type>::type;
   constexpr auto N = sizeof...(Elements);
   using Element = typename EnumParser<T, N>::Element;
   // static_assert(all_same<Elements...>);
   return EnumParser<T, N>(std::array<Element, N>{elements...});
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
class DefaultParserFunctions {};
template <>
class DefaultParserFunctions<bool> {
   public:
   [[nodiscard]] static bool parse(bool& v, std::string_view nv) noexcept;
   [[nodiscard]] static std::string parserDescription() noexcept;
   [[nodiscard]] static std::string output(const bool& v) noexcept;
};
template <>
class DefaultParserFunctions<unsigned> {
   public:
   [[nodiscard]] static bool parse(unsigned& v, std::string_view nv) noexcept;
   [[nodiscard]] static std::string parserDescription() noexcept;
   [[nodiscard]] static std::string output(const unsigned& v) noexcept;
};
template <>
class DefaultParserFunctions<int> {
   public:
   [[nodiscard]] static bool parse(int& v, std::string_view nv) noexcept;
   [[nodiscard]] static std::string parserDescription() noexcept;
   [[nodiscard]] static std::string output(const int& v) noexcept;
};
template <>
class DefaultParserFunctions<uint64_t> {
   public:
   [[nodiscard]] static bool parse(uint64_t& v, std::string_view nv) noexcept;
   [[nodiscard]] static std::string parserDescription() noexcept;
   [[nodiscard]] static std::string output(const uint64_t& v) noexcept;

   /// Parse a byte specification with units. Default unit is MB
   [[nodiscard]] static bool parseWithUnits(uint64_t& v, std::string_view nv) noexcept;
};
#if __APPLE__
template <>
class DefaultParserFunctions<unsigned long> {
   public:
   [[nodiscard]] static bool parse(unsigned long& v, std::string_view nv) noexcept;
   [[nodiscard]] static std::string parserDescription() noexcept;
   [[nodiscard]] static std::string output(const unsigned long& v) noexcept;

   /// Parse a byte specification with units. Default unit is MB
   [[nodiscard]] static bool parseWithUnits(unsigned long& v, std::string_view nv) noexcept;
};
#endif
template <>
class DefaultParserFunctions<std::string> {
   public:
   [[nodiscard]] static bool parse(std::string& v, std::string_view nv) noexcept;
   [[nodiscard]] static std::string parserDescription() noexcept;
   [[nodiscard]] static std::string output(const std::string& v) noexcept;
};
template <typename T>
class DefaultParser final : public SettingParser<T> {
   public:
   [[nodiscard]] bool parse(T& v, std::string_view nv) const noexcept override { return DefaultParserFunctions<T>::parse(v, nv); }
   [[nodiscard]] std::string parserDescription() const noexcept override { return DefaultParserFunctions<T>::parserDescription(); }
   [[nodiscard]] std::string output(const T& v) const noexcept override { return DefaultParserFunctions<T>::output(v); }
};
template <typename T>
static constexpr DefaultParser<T> defaultParser;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::settinghelper
// ---------------------------------------------------------------------------------------------------