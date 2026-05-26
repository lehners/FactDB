#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/config.h"
// ---------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
class UmbraConnector {
   private:
   std::string cmdPath;
   std::vector<std::string> args;
   int pipeInput[2];
   int pipeOutput[2];
   pid_t pid;

   private:
   std::string readOutput() const;
   std::string readOutputTimeout(size_t timeoutPeriod) const;
   size_t parseCountStarResult(const std::string& input) const;

   public:
   UmbraConnector(const std::string& cmdPath_, const std::vector<std::string>& args_);
   UmbraConnector(const UmbraConnector&) = delete;
   UmbraConnector(UmbraConnector&&) = default;
   UmbraConnector operator=(const UmbraConnector&) = delete;
   UmbraConnector& operator=(UmbraConnector&&) = default;
   ~UmbraConnector();

   std::string sendCmd(const std::string& query, bool readOutput);
   size_t sendCountStar(const std::string& query);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
