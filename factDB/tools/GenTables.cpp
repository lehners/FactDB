// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include "factDB/SQLExecution.hpp"
#include <filesystem>
#include <iostream>
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
// ---------------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
   if (argc != 2) {
      std::cerr << "Usage: " << argv[0] << " <path>" << std::endl;
      return 1;
   }
   std::string path = argv[1];
   if (!std::filesystem::exists(path)) {
      std::cout << "The path is not valid." << std::endl;
      return 1;
   }

   auto db = DatabaseCreationHelper::loadDatabase();
   SQLExecutor sqlExecutor(db);

   std::fstream schema(path, std::ios_base::in);
   if (!schema) {
      std::cerr << "Schema SQL file was not opened succesfully" << std::endl;
      return 1;
   }
   sqlExecutor.executeSQL(schema);

   // db.genTblFiles();
}
// ---------------------------------------------------------------------------------------------------
