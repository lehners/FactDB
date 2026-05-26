#pragma once
// ---------------------------------------------------------------------------------------------------
#include <ostream>
#include <sstream>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class LineSortingStream : public std::ostream {
   class LineSortingBuffer : public std::stringbuf {
      friend class LineSortingStream;

      private:
      std::vector<std::string> lines;

      public:
      LineSortingBuffer() = default;
      int sync() override;
   };

   private:
   LineSortingBuffer buffer;

   public:
   LineSortingStream() : std::ostream(&buffer), buffer() {}

   void sortLines();
   void printSortedLines(std::ostream& outputStream) const;
   void printSortedLines() const;
   std::string concatLines();
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------