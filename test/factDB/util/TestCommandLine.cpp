#include "factDB/util/CommandLine.hpp"
#include "gtest/gtest.h"
#include <sstream>
//---------------------------------------------------------------------------
// Umbra
// (c) 2017 Thomas Neumann
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace factDB::commandLine {
//---------------------------------------------------------------------------
namespace {
struct Options : public CommandLine {
   Option<bool> flag;
   Option<int> ival;
   Option<unsigned> uval;
   Option<string> sval;

   Options() : CommandLine("test"s, "[options] [args]"s) {
      add(flag).shortName('f').longName("flag"s).description("a flag"s);
      add(ival).shortName('i').longName("ival"s);
      add(uval).shortName('u').longName("uval"s);
      add(sval).longName("sval"s);
      //add(dummy).hidden().defaultValue();
   }
   using CommandLine::add;
};
//---------------------------------------------------------------------------
static bool doParse(Options& options, initializer_list<const char*> args)
// Helper to simplify calling parse
{
   vector<char*> a;
   for (auto v : args)
      a.push_back(const_cast<char*>(v));
   stringstream s;
   return options.parse(s, a.size(), a.data());
}
} // namespace
//---------------------------------------------------------------------------
TEST(infra_util_CommandLine, Test) {
   {
      Options o;
      ASSERT_TRUE(doParse(o, {"foo", "x", "y"}));
      ASSERT_TRUE(((!o.flag) && (!o.ival) && (!o.sval) && (o.getPositional().size() == 2) && (o.getPositional()[0] == "x"sv) && (o.getPositional()[1] == "y"sv)));
   }

   {
      Options o;
      ASSERT_FALSE(doParse(o, {"foo", "-v"}));
   }

   {
      Options o;
      ASSERT_TRUE(doParse(o, {"foo", "--", "-v"}));
      ASSERT_TRUE(((!o.flag) && (!o.ival) && (!o.sval) && (o.getPositional().size() == 1) && (o.getPositional()[0] == "-v"sv)));
   }

   {
      Options o;
      ASSERT_TRUE(doParse(o, {"foo", "x", "-f", "--", "-i9", "y"}));
      ASSERT_TRUE(((!!o.flag) && (!o.ival) && (!o.sval) && (o.getPositional().size() == 3) && (o.getPositional()[0] == "x"sv) && (o.getPositional()[1] == "-i9"sv) && (o.getPositional()[2] == "y"sv)));
   }

   {
      Options o;
      ASSERT_TRUE(doParse(o, {"foo", "--flag"}));
      ASSERT_TRUE(((!!o.flag) && (o.flag.get() == true)));
   }
   {
      Options o;
      ASSERT_TRUE(doParse(o, {"foo", "-flag=y"}));
      ASSERT_TRUE(((!!o.flag) && (o.flag.get() == true)));
   }
   {
      Options o;
      ASSERT_TRUE(doParse(o, {"foo", "-flag=no"}));
      ASSERT_TRUE(((!!o.flag) && (o.flag.get() == false)));
   }
   {
      Options o;
      ASSERT_FALSE(doParse(o, {"foo", "-flag=foo"}));
   }
   {
      Options o;
      ASSERT_FALSE(doParse(o, {"foo", "-flagx"}));
      ASSERT_FALSE(doParse(o, {"foo", "--flagx"}));
      ASSERT_FALSE(doParse(o, {"foo", "--flag", "--flag"}));
      ASSERT_FALSE(doParse(o, {"foo", "-x"}));
      ASSERT_FALSE(doParse(o, {"foo", "----"}));
   }

   {
      Options o;
      ASSERT_TRUE(doParse(o, {"foo", "--ival=6"}));
      ASSERT_TRUE(((!!o.ival) && (o.ival.get() == 6)));
   }
   {
      Options o;
      ASSERT_TRUE(doParse(o, {"foo", "-i6"}));
      ASSERT_TRUE(((!!o.ival) && (o.ival.get() == 6)));
      ASSERT_TRUE(doParse(o, {"foo", "-i", "7"}));
      ASSERT_TRUE(((!!o.ival) && (o.ival.get() == 7)));
      ASSERT_FALSE(doParse(o, {"foo", "-i6", "-i7"}));
      ASSERT_FALSE(doParse(o, {"foo", "-ixy"}));
   }
   {
      Options o;
      ASSERT_TRUE(doParse(o, {"foo", "--ival", "7"}));
      ASSERT_TRUE(((!!o.ival) && (o.ival.get() == 7)));
   }
   {
      Options o;
      ASSERT_FALSE(doParse(o, {"foo", "--ival"}));
      ASSERT_FALSE(doParse(o, {"foo", "--ival=xyz"}));
      ASSERT_FALSE(doParse(o, {"foo", "--ival", "xyz"}));
   }
   {
      Options o;
      ASSERT_TRUE(doParse(o, {"foo", "--sval=xyz"}));
      ASSERT_TRUE(((!!o.sval) && (o.sval.get() == "xyz")));

      ASSERT_FALSE(doParse(o, {"foo", "--sval"}));
      stringstream s;
      string x = "x"s;
      ASSERT_TRUE(DefaultParser<std::string>::parse(s, x, nullptr));
      ASSERT_TRUE(x == ""sv);
   }

   {
      Options o;
      stringstream s;
      o.showHelp(s);
      ASSERT_TRUE(s.str().find("Options"sv) != string::npos);
   }
   {
      CommandLine o("foo"s);
      Option<bool> flag;
      static_cast<Options&>(o).add(flag);
      stringstream s;
      o.showHelp(s);
      ASSERT_TRUE(s.str().find("Options"sv) == string::npos);
   }
   {
      Options o;
      ASSERT_TRUE(doParse(o, {"foo", "--uval=6"}));
      ASSERT_TRUE(((!!o.uval) && (o.uval.get() == 6)));
   }
   {
      Options o;
      ASSERT_FALSE(doParse(o, {"foo", "--uval=-6"}));
   }
}
//---------------------------------------------------------------------------
} // namespace factDB::commandLine
//---------------------------------------------------------------------------
