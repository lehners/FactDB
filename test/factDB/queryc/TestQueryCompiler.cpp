#include "factDB/Database.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/queryc/NewFileWriter.hpp"
#include "factDB/queryc/QueryCompiler.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "gtest/gtest.h"

using namespace factDB;

std::string exampleOutput =
   "class T {\n"
   "   private:\n"
   "   void foo() {\n"
   "      return;\n"
   "   }\n"
   "\n"
   "};\n";

TEST(factDB_queryc_QueryCompiler, FileWriterFormat) {
   std::stringstream ss;
   FileWriter out(ss, FileWriter::Silent);
   auto newLine = fw::endl();

   out << std::string("class T {") << fw::endl();
   out << "private:" << fw::endl();
   out << "void foo() {" << fw::endl();
   out << "  return;" << fw::endl();
   out << "}" << fw::endl();
   out << fw::endl();
   out << "};" << fw::endl();
   // auto e = fw::lc("ein");

   ASSERT_EQ(ss.str(), exampleOutput);
}

TEST(factDB_queryc_QueryCompiler, FileWriterFmtString) {
   std::stringstream ss;
   FileWriter out(ss, FileWriter::Silent);
   auto newLine = fw::endl();

   out << fw::fmt("hello {} and {}.", "others", "world") << fw::endl();
   auto world = fw::lc("world");
   auto others = fw::lc("others");

   out << fw::fmt("hello {1} and {0}.", others, world) << fw::endl();

   ASSERT_EQ(ss.str(), "hello others and world.\nhello world and others.\n");

   const std::string& hello = "hello";
   fw::lc(hello, fw::endl());
}

TEST(factDB_queryc_QueryCompiler, IUEqual) {
   Database db = DatabaseLoadUtil::genTestDB();
   auto cols = db.getColumns(db.getRelationID(testdb::KRelationTest), "alias");
   std::stringstream ss1;
   FileWriter out1(ss1, FileWriter::Silent);
   out1 << cols[0].get();

   std::stringstream ss2;
   FileWriter out2(ss2, FileWriter::Silent);
   out2 << *cols[0].get();

   ASSERT_EQ(ss1.str(), ss2.str());
}