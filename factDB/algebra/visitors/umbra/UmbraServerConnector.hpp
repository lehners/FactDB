#pragma once
// ---------------------------------------------------------------------------------------------------
#include <factDB/infra/Config.hpp>
#include <string>
#include <libpq-fe.h>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
class UmbraServerConnector {
   static constexpr char const* conninfo = "host=/tmp port=5434 user=postgres";

   PGconn* connection = nullptr;

   public:
   UmbraServerConnector();
   ~UmbraServerConnector();

   void version();
   std::string sendCmd(const std::string& /*query*/, bool /*needOutput*/) { not_implemented(); }
   size_t sendCountStar(const std::string& query);

   void close(PGresult* result = nullptr);
   void open();

   size_t size() const { not_implemented(); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------