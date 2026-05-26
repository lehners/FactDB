#pragma once
// ---------------------------------------------------------------------------------------------------
#include <fstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class Database;
// ---------------------------------------------------------------------------------------------------
struct SQLExecutor {
   private:
   enum ExecutorState {
      Running,
      Stop
   };

   ExecutorState state = Running;

   Database& database;

   void handleBackslashCommand(const std::string& line);
   void parseStream(std::istream& stream, std::ostream& outStream);

   public:
   SQLExecutor(Database& db);

   void executeSQL(const std::string& str, bool instantProcess = true);
   void executeSQL(std::istream& stream, bool instantProcess = true);
   void executeSQL(std::istream& stream, std::ostream& outStream, bool instantProcess = true);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------