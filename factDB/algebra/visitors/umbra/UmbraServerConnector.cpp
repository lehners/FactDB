// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/umbra/UmbraServerConnector.hpp"
#include "factDB/infra/RuntimeException.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/util/DoOnDestruction.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <fcntl.h>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
UmbraServerConnector::UmbraServerConnector() {
   try {
      open();
   } catch (std::runtime_error& err) {
      std::cerr << err.what() << std::endl;
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(1s);
   }
}
// ---------------------------------------------------------------------------------------------------
UmbraServerConnector::~UmbraServerConnector() {
   close();
}
// ---------------------------------------------------------------------------------------------------
void UmbraServerConnector::open() {
   // Establish a connection to the database
   connection = PQconnectdb(conninfo);

   // Check if the connection was successful
   if (PQstatus(connection) != CONNECTION_OK) {
      close();
      throw std::runtime_error("Unable to start connection.");
   }
   std::cout << "----- Umbra Startup Successful -----" << std::endl;
}
// ---------------------------------------------------------------------------------------------------
void UmbraServerConnector::close(PGresult* result) {
   if (result != nullptr)
      PQclear(result);
   if (connection != nullptr) {
      PQfinish(connection); // Close connection
      connection = nullptr;
   }
}
// ---------------------------------------------------------------------------------------------------
size_t UmbraServerConnector::sendCountStar(const std::string& query) {
   auto genEstimates = SettingBase::getSetting<bool>("opt.oracle.genEstimates")->get();
   while (connection == nullptr) {
      try {
         open();
      } catch (std::runtime_error& err) {
         std::cerr << err.what() << std::endl;
         if (!genEstimates)
            throw factDB::RuntimeException(ErrorCode::UmbraTimout, "Unable to start connection.");
         using namespace std::chrono_literals;
         std::this_thread::sleep_for(1s);
      }
   }

   PGresult* res = PQexec(connection, query.c_str());
   DoOnDestruction dod([&]() { close(res); });

   if (PQresultStatus(res) != PGRES_TUPLES_OK) {
      if (res == nullptr) {
         throw factDB::RuntimeException(ErrorCode::UmbraTimout, "Umbra hit timeout (maybe check if it's really a timeout in server)");
      } else if (auto errMsg = PQerrorMessage(connection); errMsg == std::string("ERROR:  unable to allocate memory\n")) {
         throw factDB::RuntimeException(ErrorCode::UmbraOoM, "Umbra went out of memory.");
      } else {
         std::cerr << "SELECT failed: " << errMsg << std::endl;
         throw std::runtime_error("error during execution of count(*) query");
      }
   } else if (PQntuples(res) != 1) {
      throw std::runtime_error("expected exactly one result for count(*) query result");
   } else {
      auto val = PQgetvalue(res, 0, 0);
      assert(val != nullptr);
      size_t count = std::atoi(val);
      return count;
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
