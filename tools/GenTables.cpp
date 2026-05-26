// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include "factDB/config.h"
#include "factDB/parser/AST.hpp"
#include "factDB/parser/SchemaParseContext.hpp"
#include "factDB/queryc/CxxWrapper.hpp"
#include "factDB/queryc/QueryParseContext.cpp"
#include "factDB/schemac/SchemaCompiler.hpp"
#include <cassert>
// ---------------------------------------------------------------------------------------------------
int main() {
   factDB::parser::SchemaParseContext parser;

   std::fstream schemaFile(CURRENT_SRC_DIR "/tools/schemas.sql");
   parser.parse(schemaFile);

   {
      factDB::schemac::SchemaCompiler schemaCompiler;
      for (auto& tree : parser.getParsedTrees()) {
         if (tree->getASTType() == factDB::parser::AST::CreateTableType) {
            auto* createTblAST = factDB::parser::CreateTable::dynCast(tree);
            assert(createTblAST != nullptr);
            schemaCompiler.generateTable(createTblAST->getTable());
         }
      }
   }

   /*{
      factDB::queryc::CxxCompiler compiler;
      auto handle = compiler.compile(factDB::schemac::SchemaCompiler::getSrcPath(), false);
      std::vector<factDB::BaseTable*> tbls;

      for (auto& tree : parser.getParsedTrees()) {
         const auto& tbl = factDB::parser::CreateTable::dynCast(tree)->getTable();
         std::string createFunName = "create_" + tbl.name;
         std::cout << "try to load: " << createFunName << std::endl;
         factDB::queryc::FunctorBuilder<factDB::BaseTable*> createFun(handle, createFunName);
         tbls.emplace_back(createFun());
      }

      for (auto& t : tbls) {
         std::cout << t->name() << std::endl;
      }

      for (auto& t : tbls) {
         // std::cout << t << std::endl;
         delete t;
      }
   }*/
}
// ---------------------------------------------------------------------------------------------------
