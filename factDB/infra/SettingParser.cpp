//---------------------------------------------------------------------------
// Umbra
// (c) 2017 Thomas Neumann
//---------------------------------------------------------------------------
#include "factDB/infra/SettingParser.hpp"
//---------------------------------------------------------------------------
namespace factDB::settinghelper {
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
bool DefaultParserFunctions<bool>::parse(bool& v, std::string_view nv) noexcept
// Parse a boolean
{
   if (!nv.empty()) {
      if ((nv[0] == '1') || (nv[0] == 'y') || (nv[0] == 'Y') || (nv[0] == 't') || (nv[0] == 'T')) {
         v = true;
         return true;
      }
      if ((nv[0] == '0') || (nv[0] == 'n') || (nv[0] == 'N') || (nv[0] == 'f') || (nv[0] == 'F')) {
         v = false;
         return true;
      }
      if (((nv[0] == 'o') || (nv[0] == 'O')) && (nv.size() > 1)) {
         if ((nv[1] == 'n') || (nv[1] == 'N')) {
            v = true;
            return true;
         }
         if ((nv[1] == 'f') || (nv[1] == 'F')) {
            v = false;
            return true;
         }
      }
   }
   return false;
}
//---------------------------------------------------------------------------
std::string DefaultParserFunctions<bool>::parserDescription() noexcept {
   return "on/off"s;
}
//---------------------------------------------------------------------------
std::string DefaultParserFunctions<bool>::output(const bool& v) noexcept {
   return v ? "on"s : "off"s;
}
//---------------------------------------------------------------------------
bool DefaultParserFunctions<unsigned>::parse(unsigned& v, std::string_view nv) noexcept {
   if (!nv.empty()) {
      v = 0;
      for (char c : nv)
         if ((c >= '0') && (c <= '9'))
            v = 10 * v + c - '0';
         else
            return false;
      return true;
   }
   return false;
}
//---------------------------------------------------------------------------
std::string DefaultParserFunctions<unsigned>::output(const unsigned& v) noexcept {
   return to_string(v);
}
//---------------------------------------------------------------------------
std::string DefaultParserFunctions<unsigned>::parserDescription() noexcept {
   return "unsigned"s;
}
//---------------------------------------------------------------------------
bool DefaultParserFunctions<int>::parse(int& v, std::string_view nv) noexcept {
   if (!nv.empty()) {
      if (nv.front() == '-') {
         v = -1;
         nv = nv.substr(1);
      } else {
         v = 1;
      }
      unsigned res;
      if (!DefaultParserFunctions<unsigned>::parse(res, nv))
         return false;
      v *= res; // NOLINT(bugprone-narrowing-conversions)
      return true;
   }
   return false;
}
//---------------------------------------------------------------------------
std::string DefaultParserFunctions<int>::output(const int& v) noexcept {
   return to_string(v);
}
//---------------------------------------------------------------------------
std::string DefaultParserFunctions<int>::parserDescription() noexcept {
   return "int"s;
}
//---------------------------------------------------------------------------
bool DefaultParserFunctions<uint64_t>::parse(uint64_t& v, std::string_view nv) noexcept {
   if (!nv.empty()) {
      v = 0;
      for (char c : nv)
         if ((c >= '0') && (c <= '9'))
            v = 10 * v + c - '0';
         else
            return false;
      return true;
   }
   return false;
}
//---------------------------------------------------------------------------
std::string DefaultParserFunctions<uint64_t>::output(const uint64_t& v) noexcept {
   return to_string(v);
}
//---------------------------------------------------------------------------
std::string DefaultParserFunctions<uint64_t>::parserDescription() noexcept {
   return "uint64"s;
}
//---------------------------------------------------------------------------
bool DefaultParserFunctions<uint64_t>::parseWithUnits(uint64_t& v, std::string_view s) noexcept {
   // Interpret the units
   uint64_t units = 1ull << 20u;
   if (!s.empty()) {
      char c = s.back();
      if ((c == 'B') || (c == 'b')) {
         units = 1;
         s = s.substr(0, s.length() - 1);
      } else if ((c == 'K') || (c == 'k')) {
         units = 1ull << 10u;
         s = s.substr(0, s.length() - 1);
      } else if ((c == 'M') || (c == 'm')) {
         units = 1ull << 20u;
         s = s.substr(0, s.length() - 1);
      } else if ((c == 'G') || (c == 'g')) {
         units = 1ull << 30u;
         s = s.substr(0, s.length() - 1);
      }
   }

   // Parse the value
   uint64_t n = 0;
   for (char c : s) {
      if ((c >= '0') && (c <= '9')) {
         n = 10 * n + c - '0';
      } else {
         return false;
      }
   }
   n = n * units;
   if (n < (1ull << 10u)) n = 1ull << 10u;

   v = n;
   return true;
}
//---------------------------------------------------------------------------
#if __APPLE__
//---------------------------------------------------------------------------
bool DefaultParserFunctions<unsigned long>::parse(unsigned long& v, std::string_view nv) noexcept {
   return DefaultParserFunctions<size_t>::parse(v, nv);
}
//---------------------------------------------------------------------------
std::string DefaultParserFunctions<unsigned long>::output(const unsigned long& v) noexcept {
   return DefaultParserFunctions<size_t>::output(v);
}
//---------------------------------------------------------------------------
bool DefaultParserFunctions<unsigned long>::parseWithUnits(unsigned long& v, std::string_view nv) noexcept {
   return DefaultParserFunctions<size_t>::parseWithUnits(v, nv);
}
//---------------------------------------------------------------------------
std::string DefaultParserFunctions<unsigned long>::parserDescription() noexcept {
   return DefaultParserFunctions<size_t>::parserDescription();
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
bool DefaultParserFunctions<std::string>::parse(std::string& v, std::string_view nv) noexcept {
   v = nv;
   return true;
}
//---------------------------------------------------------------------------
std::string DefaultParserFunctions<std::string>::output(const std::string& v) noexcept {
   return v;
}
//---------------------------------------------------------------------------
std::string DefaultParserFunctions<std::string>::parserDescription() noexcept {
   return "string"s;
}
//---------------------------------------------------------------------------
} // namespace factDB::settinghelper
//---------------------------------------------------------------------------