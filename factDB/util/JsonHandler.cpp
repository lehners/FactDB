#ifndef H_FACTDB_FACT_DB_UTIL_JSONHANDLER_HPP
#define H_FACTDB_UTIL_JSON_HANDLER

#include "factDB/util/JsonHandler.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/visitors/GraphvizQueryPlan.hpp"
#include "factDB/algebra/visitors/QueryPlanWriter.hpp"
#include "factDB/queryc/QueryCompiler.hpp"
#include <fstream>

namespace factDB::util::json {

void printJson(const Database& db, std::unique_ptr<algebra::Operator>& tree, const OrderedIUSet& requiredColumns, const std::string& outfile) {
   std::ofstream stream(outfile, std::ofstream::trunc);
   algebra::visitors::QueryPlanWriter visitor(db, stream);
   queryc::QueryCompiler compiler;
   compiler.compile(tree, requiredColumns, visitor);
   stream.close();
}

std::unique_ptr<algebra::Operator> readJson(const Database& db, const std::string& inPath) {
   std::ifstream inStream(inPath);
   nlohmann::json jf = nlohmann::json::parse(inStream);
   auto tree = algebra::visitors::QueryPlanReader::read_operator(db, jf);
   inStream.close();
   return tree;
}

void printGraphviz(Database& db, std::unique_ptr<algebra::Operator>& tree, const OrderedIUSet& requiredColumns) {
   std::ostream& stream = std::cout;
   algebra::visitors::GraphvizQueryPlan visitor(db, stream);
   queryc::QueryCompiler compiler;
   compiler.compile(tree, requiredColumns, visitor);
}

} // namespace factDB::util::json

#endif //  H_FACTDB_FACT_DB_UTIL_JSONHANDLER_HPP
