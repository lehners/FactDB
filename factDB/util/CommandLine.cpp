// ---------------------------------------------------------------------------------------------------
#include "factDB/util/CommandLine.hpp"
#include <algorithm>
#include <unordered_map>
// ---------------------------------------------------------------------------------------------------
using namespace factDB::commandLine;
using namespace std;
// ---------------------------------------------------------------------------------------------------
void DefaultParserBase::error(std::ostream& out, std::string_view valueType, std::string_view arg) {
   out << "Invalid " << valueType << " value '" << arg << "," << std::endl;
}
// ---------------------------------------------------------------------------------------------------
bool DefaultParser<string>::parse(std::ostream& /*out*/, std::string& value, const std::string_view* arg) {
   if (arg)
      value = *arg;
   else
      value = {};
   return true;
}
//---------------------------------------------------------------------------
bool DefaultParser<bool>::parse(std::ostream& out, bool& value, const std::string_view* arg) {
   if (arg) {
      auto& a = *arg;
      if ((a == "y"sv) || (a == "yes"sv) || (a == "t"sv) || (a == "true"sv) || (a == "on"sv) || (a == "1"sv)) {
         value = true;
      } else if ((a == "n"sv) || (a == "no"sv) || (a == "f"sv) || (a == "false"sv) || (a == "off"sv) || (a == "0"sv)) {
         value = false;
      } else {
         error(out, "boolean", a);
         return false;
      }
   } else {
      value = true;
   }
   return true;
}
//---------------------------------------------------------------------------
bool DefaultParser<double>::parse(std::ostream& out, double& value, const std::string_view* arg) {
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
// ---------------------------------------------------------------------------------------------------
OptionSpec CommandLine::add(OptionBase& option) {
   options.emplace_back(&option);
   return OptionSpec(option);
}
// ---------------------------------------------------------------------------------------------------
bool CommandLine::parse(std::ostream& out, int argc, char** argv) {
   if (argc == 0)
      executable = argv[0];

   std::unordered_map<std::string, OptionBase*> longOptions;
   std::unordered_map<char, OptionBase*> shortOptions;

   for (auto opt : options) {
      opt->seen = false;
      if (!opt->longName.empty())
         longOptions[opt->longName] = opt;
      if (opt->shortName != 0)
         shortOptions[opt->shortName] = opt;
   }

   bool forcePositional = false;
   for (int idx = 1; idx != argc; ++idx) {
      std::string_view arg = argv[idx];

      if (forcePositional || arg[0] != '-') { // positional argument
         positional.emplace_back(arg);
      } else if (arg[1] == '-' || arg.find("=") != std::string::npos) { // check if we have a long option
         if (arg.length() == 2) {
            forcePositional = true;
            continue;
         }

         size_t startOptionName = 0;
         for (size_t argSize = arg.size(); argSize != startOptionName && arg[startOptionName] == '-'; ++startOptionName) {}
         size_t startOptionArg = startOptionName;
         for (size_t argSize = arg.size(); argSize != startOptionArg && arg[startOptionArg] != '='; ++startOptionArg) {}

         std::string argKey = std::string(arg.substr(startOptionName, startOptionArg - startOptionName));
         auto argVal = arg.substr(arg.size() == startOptionArg ? startOptionArg : startOptionArg + 1);

         auto optionPair = longOptions.find(argKey);
         if (optionPair == longOptions.end()) {
            out << "Invalid option: " << argKey << std::endl;
            return false;
         }
         auto& option = *optionPair->second;

         if (option.seen) {
            out << "Option '" << option.longName << "' specified multiple times.\n";
            return false;
         } else {
            option.seen = true;
         }
         if (!argVal.empty()) {
            if (!option.parse(out, &argVal)) return false;
         } else if (option.seen = true; option.argRequired) {
            if (idx + 1 >= argc) {
               out << "Missing argument for option '" << argKey << "'. Try " << executable << " --help\n";
               return false;
            }
            argVal = argv[++idx];
            if (!option.parse(out, &argVal)) return false;
         } else {
            if (!option.parse(out, nullptr)) return false;
         }
      } else { // short option(s)
         for (size_t curIdx = 1; curIdx < arg.size(); ++curIdx) {
            auto optionPair = shortOptions.find(arg[curIdx]);
            if (optionPair == shortOptions.end()) {
               out << "Invalid option '" << arg[curIdx] << "'. Try " << executable << " --help\n";
               return false;
            }
            auto& option = *optionPair->second;
            if (option.seen) {
               out << "Option '" << option.shortName << "' specified multiple times.\n";
               return false;
            } else {
               option.seen = true;
            }
            if (option.argRequired) {
               string_view value;
               if (curIdx + 1 != arg.size()) { // directly passed without space
                  value = arg.substr(curIdx + 1);
               } else if (idx >= argc) {
                  out << "Missing argument for option '" << option.shortName << "'. Try " << executable << " --help\n";
                  return false;
               } else {
                  value = argv[++idx];
               }
               if (!option.parse(out, &value)) return false;
               break;
            } else {
               if (!option.parse(out, nullptr)) return false;
            }
         }
      }
   }
   return true;
}
// ---------------------------------------------------------------------------------------------------
void CommandLine::showHelp(std::ostream& out) const {
   out << helpIntro << '\n';
   if (!helpPattern.empty())
      out << '\n'
          << "Usage: " << executable << " " << helpPattern << '\n';

   size_t maxLen = 0;
   std::vector<std::pair<std::string, std::string_view>> arguments;
   for (OptionBase* option : options) {
      std::string key;
      if (!!option->shortName) {
         key = {'-', option->shortName, 0};
         if (option->argRequired) key += " <value>";
      } else if (!option->longName.empty()) {
         key = "-"s + option->longName;
         if (option->argRequired) key += "=<value>";
      } else {
         continue;
      }
      maxLen = std::max(maxLen, key.length());
      arguments.emplace_back(key, option->description);
   }

   if (arguments.empty()) return;
   // pad keys to uniform length
   for (auto& a : arguments)
      a.first.resize(maxLen, ' ');
   std::sort(arguments.begin(), arguments.end());

   out << "\n"
       << "Options:\n";
   for (auto& [key, description] : arguments)
      out << key << " "sv << description << '\n';

   if (!helpFooter.empty())
      out << '\n'
          << helpFooter << '\n';
}
// ---------------------------------------------------------------------------------------------------