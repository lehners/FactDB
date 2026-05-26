// ---------------------------------------------------------------------------------------------------
#include "factDB/parser/AST.hpp"
#include "factDB/parser/SchemaParseContext.hpp"
#include "gtest/gtest.h"
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
using namespace std;
// ---------------------------------------------------------------------------------------------------
TEST(factDB_parser_SchemaParser, CREATE_TABLE) {
   factDB::parser::SchemaParseContext parser;

   {
      std::stringstream ss;
      ss << "create table dblp11 (s int not null, d int not null, primary key(s, d));" << std::endl;
      parser.parse(ss);
      ASSERT_EQ(parser.parsedTreesSize(), 1);
      factDB::parser::AST* tree = parser.getParsedTrees(0);
      ASSERT_NE(tree, nullptr);
      ASSERT_EQ(tree->getASTType(), parser::AST::CreateTableType);

      auto castedAST = parser::CreateTable::dynCast(tree);
      ASSERT_NE(castedAST, nullptr);
      auto& tbl = castedAST->getTable();
      ASSERT_EQ(tbl.name, "dblp11");

      ASSERT_EQ(tbl.columns.size(), 2);
      ASSERT_EQ(tbl.columns[0].name, "s");
      ASSERT_EQ(tbl.columns[0].type, schemac::Type::Integer());
      ASSERT_EQ(tbl.columns[1].name, "d");
      ASSERT_EQ(tbl.columns[0].type, schemac::Type::Integer());
      ASSERT_EQ(tbl.primary_key.size(), 2);
   }
}
// ---------------------------------------------------------------------------------------------------
