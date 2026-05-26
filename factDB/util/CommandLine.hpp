#pragma once
// ---------------------------------------------------------------------------------------------------
#include <charconv>
#include <ostream>
#include <string>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::commandLine {
// ---------------------------------------------------------------------------------------------------
struct DefaultParserBase {
   protected:
   static void error(std::ostream& out, std::string_view valueType, std::string_view arg);
};
// ---------------------------------------------------------------------------------------------------
template <class T>
struct DefaultParser {
};
// ---------------------------------------------------------------------------------------------------
template <>
struct DefaultParser<std::string> : public DefaultParserBase {
   static constexpr bool argRequired = true;
   static bool parse(std::ostream& out, std::string& value, const std::string_view* arg);
};
// ---------------------------------------------------------------------------------------------------
template <>
struct DefaultParser<bool> : public DefaultParserBase {
   static constexpr bool argRequired = false;
   static bool parse(std::ostream& out, bool& value, const std::string_view* arg);
};
// ---------------------------------------------------------------------------------------------------
template <>
struct DefaultParser<double> : public DefaultParserBase {
   static constexpr bool argRequired = true;
   static bool parse(std::ostream& out, double& value, const std::string_view* arg);
};
// ---------------------------------------------------------------------------------------------------
template <typename T>
   requires(std::is_arithmetic_v<T>)
struct DefaultParser<T> : public DefaultParserBase {
   static constexpr bool argRequired = true;
   static bool parse(std::ostream& out, T& value, const std::string_view* arg) {
      if (arg) {
         auto [ptr, ec] = std::from_chars(arg->data(), arg->data() + arg->size(), value);
         if ((ec != std::errc()) || (ptr != (arg->data() + arg->size()))) {
            error(out, "numerical", *arg);
            return false;
         }
      } else {
         value = {}; // currently unreachable, could change in the future
      }
      return true;
   }
};
// ---------------------------------------------------------------------------------------------------
class OptionBase {
   std::string longName;
   std::string description;
   char shortName = 0;
   bool argRequired;
   bool hasDefaultValue = false;
   bool seen;

   virtual bool parse(std::ostream& out, const std::string_view* arg) = 0;

   protected:
   explicit OptionBase(bool argRequired_) : argRequired(argRequired_) {}
   ~OptionBase() = default;

   friend class OptionSpec;
   friend class CommandLine;

   public:
   /// Not set?
   bool operator!() const { return (!seen) && (!hasDefaultValue); }
};
// ---------------------------------------------------------------------------------------------------
class OptionSpec {
   /// The option
   OptionBase& option;

   /// Constructor
   explicit OptionSpec(OptionBase& option_) : option(option_) {}

   friend class CommandLine;

   public:
   /// Set the short name
   OptionSpec& shortName(char n) {
      option.shortName = n;
      return *this;
   }
   /// Set the long name
   OptionSpec& longName(std::string n) {
      option.longName = std::move(n);
      return *this;
   }
   /// Set the description
   OptionSpec& description(std::string n) {
      option.description = std::move(n);
      return *this;
   }
   /// Mark as having a default value
   OptionSpec& hasDefaultValue() {
      option.hasDefaultValue = true;
      return *this;
   }
};
// ---------------------------------------------------------------------------------------------------
template <class T, class Parser = DefaultParser<T>>
class Option : public OptionBase {
   T value;

   bool parse(std::ostream& out, const std::string_view* arg) override { return Parser::parse(out, value, arg); }

   public:
   explicit Option(T value_ = {}) : OptionBase(Parser::argRequired), value(std::move(value_)) {}
   const T& get() const { return value; }
};
// ---------------------------------------------------------------------------------------------------
class CommandLine {
   /// The help text
   std::string helpIntro, helpPattern, helpFooter;
   /// All options
   std::vector<OptionBase*> options;
   /// The executable name
   std::string executable;
   /// The positional arguments
   std::vector<std::string> positional;

   protected:
   OptionSpec add(OptionBase& option);

   public:
   explicit CommandLine(std::string helpIntro_, std::string helpPattern_ = {}, std::string helpFooter_ = {}) : helpIntro(std::move(helpIntro_)), helpPattern(std::move(helpPattern_)), helpFooter(std::move(helpFooter_)) {}
   ~CommandLine() = default;

   /// Parse the command line options
   bool parse(std::ostream& out, int argc, char** argv);
   /// Show a help screen
   void showHelp(std::ostream& out) const;
   /// Get the positional arguments
   auto& getPositional() const { return positional; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::commandLine
// ---------------------------------------------------------------------------------------------------