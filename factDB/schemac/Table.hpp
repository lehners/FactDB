// ---------------------------------------------------------------------------------------------------
#ifndef H_FACTDB_PARSER_SCHEMA_H_
#define H_FACTDB_PARSER_SCHEMA_H_
// ---------------------------------------------------------------------------------------------------
#include "factDB/schemac/IndexType.hpp"
#include "factDB/schemac/Type.hpp"
#include <cstdint>
#include <string>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::schemac {
// ---------------------------------------------------------------------------------------------------
struct Column {
   // the name of the column
   std::string name;
   // type of the column
   schemac::Type type;
};
// ---------------------------------------------------------------------------------------------------
struct Table {
   /// the id of the table
   uint32_t id = 0;
   // the name of the table
   std::string name;
   // the columns of the table
   std::vector<Column> columns;
   // the primary key (ignored for now, todo)
   std::vector<Column> primary_key = {};
   // index type (ignored for now, todo)
   IndexType index_type = None;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::schemac
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_PARSER_SCHEMA_H_
// ---------------------------------------------------------------------------------------------------