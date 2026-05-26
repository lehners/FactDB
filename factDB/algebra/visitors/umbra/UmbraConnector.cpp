// ---------------------------------------------------------------------------------------------------
#include "factDB/config.h"
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/umbra/UmbraConnector.hpp"
#include "factDB/infra/Setting.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <sys/wait.h>
#include <unistd.h>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
static factDB::Setting<std::string> umbraBinary("visitor.umbra.bin", CURRENT_SRC_DIR "/umbra/bin/sql");
static factDB::Setting<std::string> umbraDatabase("visitor.umbra.db", CURRENT_SRC_DIR "/umbra/db/ce.db");
// ---------------------------------------------------------------------------------------------------
UmbraConnector::UmbraConnector(const std::string& cmdPath_, const std::vector<std::string>& args_) : cmdPath(cmdPath_), args(args_) {
   if (!std::filesystem::exists(cmdPath_))
      throw std::runtime_error("Invalid path to umbra, maybe symlink to umbra is missing?");
   if (pipe(pipeInput) == -1 || pipe(pipeOutput) == -1) {
      throw std::runtime_error("Failed to create pipes");
   }
   pid = fork();
   if (pid == -1) {
      throw std::runtime_error("Failed to fork process");
   }
   if (pid == 0) { // Child process
      // Redirect stdin and stdout to the pipes
      dup2(pipeInput[0], STDIN_FILENO);
      dup2(pipeOutput[1], STDOUT_FILENO);

      // Close unused pipe ends
      close(pipeInput[1]);
      close(pipeOutput[0]);

      // Prepare arguments for execv
      std::vector<char*> exec_args;
      exec_args.push_back(const_cast<char*>(cmdPath.c_str()));
      for (const auto& arg : args) {
         exec_args.push_back(const_cast<char*>(arg.c_str()));
      }
      exec_args.push_back(nullptr);

      // Execute sqlCMD
      execv(cmdPath.c_str(), exec_args.data());
      _exit(EXIT_FAILURE); // If execv fails
   } else { // Parent process
      // Close unused pipe ends
      close(pipeInput[0]);
      close(pipeOutput[1]);

      std::cout << "----- Umbra Startup Output: -----" << std::endl
                << readOutputTimeout(1) << std::endl
                << "----- Umbra Startup Output end -----" << std::endl;
   }
}
// ---------------------------------------------------------------------------------------------------
UmbraConnector::~UmbraConnector() {
   sendCmd("\\q", false);
   close(pipeInput[1]);
   close(pipeOutput[0]);
   waitpid(pid, nullptr, 0); // Wait for child process to finish
}
// ---------------------------------------------------------------------------------------------------
std::string UmbraConnector::sendCmd(const std::string& query, bool needOutput) {
   if (write(pipeInput[1], query.c_str(), query.length()) == -1) {
      throw std::runtime_error("Failed to write to sqlCMD");
   }

   // Send a newline to execute the command
   if (write(pipeInput[1], "\n", 1) == -1) {
      throw std::runtime_error("Failed to write newline to sqlCMD");
   }

   if (needOutput)
      return readOutput();
   else
      return "";
}
// ---------------------------------------------------------------------------------------------------
size_t UmbraConnector::sendCountStar(const std::string& query) {
   auto result = sendCmd(query, true);
   return parseCountStarResult(result);
}
// ---------------------------------------------------------------------------------------------------
std::string UmbraConnector::readOutputTimeout(size_t timeoutPeriod) const {
   char buffer[256];
   std::vector<char> output;
   ssize_t bytesRead;

   fd_set readfds;
   struct timeval timeout;

   // Initialize the timeout data structure
   timeout.tv_sec = timeoutPeriod; // 1 second timeout
   timeout.tv_usec = 0;

   FD_ZERO(&readfds);
   FD_SET(pipeOutput[0], &readfds);

   int retval = select(pipeOutput[0] + 1, &readfds, NULL, NULL, &timeout);

   if (retval == -1) {
      throw std::runtime_error("select() failed");
   } else if (retval > 0) {
      if (FD_ISSET(pipeOutput[0], &readfds)) {
         while ((bytesRead = read(pipeOutput[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            output.insert(output.end(), buffer, buffer + bytesRead);
            // Break the loop if we have read the expected output.
            if (buffer[bytesRead - 1] == '\n') break;
         }

         if (bytesRead == -1 && errno != EAGAIN) {
            throw std::runtime_error("Failed to read from sqlCMD");
         }
      }
   }

   return std::string(output.begin(), output.end());
}
// ---------------------------------------------------------------------------------------------------
std::string UmbraConnector::readOutput() const {
   char buffer[256];
   std::vector<char> output;
   ssize_t bytesRead;

   while ((bytesRead = read(pipeOutput[0], buffer, sizeof(buffer) - 1)) > 0) {
      buffer[bytesRead] = '\0';
      output.insert(output.end(), buffer, buffer + bytesRead);
      // Break the loop if we have read the expected output.
      if (buffer[bytesRead - 1] == '\n') break;
   }

   if (bytesRead == -1) {
      throw std::runtime_error("Failed to read from sqlCMD");
   }

   return std::string(output.begin(), output.end());
}
// ---------------------------------------------------------------------------------------------------
size_t UmbraConnector::parseCountStarResult(const std::string& input) const {
   constexpr std::string_view header = "count\n";
   if (!input.starts_with(header)) {
      auto currentQueryName = factDB::SettingBase::getSetting<std::string>("queryname");
      std::cout << "Problem in query <queryname>: " << currentQueryName->get() << std::endl;
      throw std::runtime_error("Unknown count(*) result, invalid header ( " + currentQueryName->get() + "): " + input);
   }
   size_t readSize = 0;
   for (auto idx = header.size(); idx != input.size() - 1; ++idx) {
      switch (input[idx]) {
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            readSize = readSize * 10 + (input[idx] - '0');
            break;
         case '\n':
            if (idx != input.size() - 1)
               throw std::runtime_error("Unknown count(*) result, more than 1 row: " + input);
            break;
         default:
            throw std::runtime_error("Unknown count(*) result, unexpected letter in first row: " + input);
            break;
      }
   }
   return readSize;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
