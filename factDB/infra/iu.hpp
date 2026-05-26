#ifndef H_FACTDB_FACT_DB_INFRA_IU_HPP
#define H_FACTDB_FACT_DB_INFRA_IU_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/schemac/Type.hpp"
#include <string>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class IU {
   public:
   const std::string_view table;
   const std::string column;
   const factDB::schemac::Type type;

   // Anonymous IU constructor
   constexpr IU(const std::string& columnParam, const schemac::Type typParam) // NOLINT
      : table(), column(std::move(columnParam)), type(typParam) {}
   // Table IU constructor
   constexpr IU(const std::string_view tableParam, const std::string& columnParam, const schemac::Type typeParam)
      : table(tableParam), column(std::move(columnParam)), type(typeParam) {}

   bool operator==(const IU& other) const { return table == other.table && column == other.column; }
   bool operator!=(const IU& other) const { return !this->operator==(other); };
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // FACTDB_IU_HPP
