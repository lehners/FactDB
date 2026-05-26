#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/util/DoOnDestruction.hpp"
#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
// ---------------------------------------------------------------------------------------------------
namespace factDB::util {
// ---------------------------------------------------------------------------------------------------
struct FileReader {
   char* ptr = nullptr;
   size_t size = 0;

   FileReader(const std::string& filename) {
      int fd = open(filename.c_str(), O_RDONLY);
      if (fd == -1) {
         std::cerr << "Error opening file: " << filename << std::endl;
         return;
      }

      DoOnDestruction doOnDestruction([&fd]() { close(fd); });

      // Get the file size
      struct stat fileStat;
      if (fstat(fd, &fileStat) == -1) {
         std::cerr << "Error getting file size" << std::endl;
         return;
      }

      char* fileContent = static_cast<char*>(mmap(nullptr, fileStat.st_size, PROT_READ, MAP_SHARED, fd, 0));
      if (fileContent == MAP_FAILED) {
         std::cerr << "Error mapping file into memory: " << filename << std::endl;
         ptr = nullptr;
      }

      ptr = fileContent;
      size = fileStat.st_size;
   }

   ~FileReader() {
      if (ptr != nullptr && munmap(ptr, size) == -1) {
         std::cerr << "Error unmapping file from memory" << std::endl;
         return;
      }
   }

   bool isValid() { return !!ptr; }

   char* operator*() { return ptr; }
   std::string str() { return {ptr}; }
   std::string_view sv() { return {ptr, size}; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::util
// ---------------------------------------------------------------------------------------------------
