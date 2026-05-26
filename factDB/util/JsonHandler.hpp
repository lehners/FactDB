#ifndef H_FACTDB_FACT_DB_UTIL_JSONHANDLER_HPP
#define H_FACTDB_FACT_DB_UTIL_JSONHANDLER_HPP

#include "factDB/infra/IUSet.hpp"
#include <memory>

namespace factDB {
class Database;
} // namespace factDB

namespace factDB::algebra {
class Operator;
} // namespace factDB::algebra

namespace factDB::util::json {

void printJson(const Database& db, std::unique_ptr<algebra::Operator>& tree, const OrderedIUSet& requiredColumns, const std::string& outfile);
std::unique_ptr<algebra::Operator> readJson(const Database& db, const std::string& inPath);

void printGraphviz(Database& db, std::unique_ptr<algebra::Operator>& tree, const OrderedIUSet& requiredColumns);

} // namespace factDB::util::json

#endif //  H_FACTDB_FACT_DB_UTIL_JSONHANDLER_HPP
