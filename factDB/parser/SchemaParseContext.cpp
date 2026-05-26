// ---------------------------------------------------------------------------------------------------
// adapted from imlab
// ---------------------------------------------------------------------------------------------------
#include "factDB/parser/SchemaParseContext.hpp"
#include "factDB/parser/AST.hpp"
#include "factDB/parser/gen/parser.h"
#include "factDB/util/DoOnDestruction.hpp"
#include <sstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::parser {
// ---------------------------------------------------------------------------------------------------
SchemaParseContext::SchemaParseContext(bool trace_scanning, bool trace_parsing) : trace_scanning_(trace_scanning), trace_parsing_(trace_parsing) {
}
// ---------------------------------------------------------------------------------------------------
SchemaParseContext::~SchemaParseContext() = default;
// ---------------------------------------------------------------------------------------------------
void SchemaParseContext::parse(std::istream& in) {
   beginScan(in);
   DoOnDestruction doLater([&]() { endScan(); });
   factDB::parser::SchemaParser parser(*this);
   parser.set_debug_level(trace_parsing_);
   parser.parse();
   endScan();
}
// ---------------------------------------------------------------------------------------------------
void SchemaParseContext::Error(const std::string& m) {
   throw SchemaCompilationError(m);
}
// ---------------------------------------------------------------------------------------------------
void SchemaParseContext::Error(uint32_t line, uint32_t column, const std::string& err) {
   std::stringstream ss;
   ss << "[ l=" << line << " c=" << column << " ] " << err << std::endl;
   throw SchemaCompilationError(ss.str());
}
// ---------------------------------------------------------------------------------------------------
void SchemaParseContext::registerAST(std::vector<std::unique_ptr<AST>>&& trees) {
   parsedTrees.reserve(parsedTrees.size() + trees.size());
   for (auto& tree : trees) {
      if (tree) parsedTrees.push_back(std::move(tree));
   }
}
// ---------------------------------------------------------------------------------------------------
std::vector<AST*> SchemaParseContext::getParsedTrees() const {
   std::vector<AST*> res;
   res.reserve(parsedTrees.size());
   for (auto& tree : parsedTrees)
      res.emplace_back(tree.get());
   return res;
}
// ---------------------------------------------------------------------------------------------------
AST* SchemaParseContext::getParsedTrees(size_t idx) const {
   assert(idx < parsedTrees.size());
   return parsedTrees[idx].get();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::parser
// ---------------------------------------------------------------------------------------------------