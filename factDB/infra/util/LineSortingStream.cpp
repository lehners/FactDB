// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/util/LineSortingStream.hpp"
#include <algorithm>
#include <iostream>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
int LineSortingStream::LineSortingBuffer::sync() {
   lines.push_back(str());
   str("");
   return 0;
}
// ---------------------------------------------------------------------------------------------------
void LineSortingStream::sortLines() {
   buffer.pubsync();
   std::sort(buffer.lines.begin(), buffer.lines.end());
}
// ---------------------------------------------------------------------------------------------------
void LineSortingStream::printSortedLines() const {
   return printSortedLines(std::cout);
}
// ---------------------------------------------------------------------------------------------------
void LineSortingStream::printSortedLines(std::ostream& outputStream) const {
   for (const auto& line : buffer.lines)
      outputStream << line;
}
// ---------------------------------------------------------------------------------------------------
std::string LineSortingStream::concatLines() {
   std::stringstream s;
   printSortedLines(s);
   return s.str();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
