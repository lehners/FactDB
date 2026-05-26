#ifndef H_FACTDB_FACTDB_UTIL_STRINGUTILS_HPP
#define H_FACTDB_FACTDB_UTIL_STRINGUTILS_HPP
// ---------------------------------------------------------------------------------------------------
#include <algorithm>
#include <string>
// ---------------------------------------------------------------------------------------------------
namespace factDB::util {
// ---------------------------------------------------------------------------------------------------
inline void tolower(std::string& data) {
   std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) { return std::tolower(c); });
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::util
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_UTIL_QUERYFILE_HPP
