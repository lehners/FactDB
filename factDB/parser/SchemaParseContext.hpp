#ifndef H_FACTDB_PARSER_SCHEMAPARSECONTEXT_H_
#define H_FACTDB_PARSER_SCHEMAPARSECONTEXT_H_
// ---------------------------------------------------------------------------------------------------
// adapted from imlab
// ---------------------------------------------------------------------------------------------------
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB::parser {
// ---------------------------------------------------------------------------------------------------
class SchemaParser;
class AST;
// ---------------------------------------------------------------------------------------------------
struct SchemaCompilationError : std::exception {
   // Constructor
   explicit SchemaCompilationError(const char* what) : message_(what) {}
   // Constructor
   explicit SchemaCompilationError(const std::string& what) : message_(std::move(what)) {}
   // Destructor
   virtual ~SchemaCompilationError() noexcept = default;
   // Get error message
   [[nodiscard]] const char* what() const noexcept override { return message_.c_str(); }

   protected:
   // Error message
   std::string message_;
};
// ---------------------------------------------------------------------------------------------------
class SchemaParseContext {
   friend SchemaParser;

   private:
   // Trace the scanning
   bool trace_scanning_;
   // Trace the parsing
   bool trace_parsing_;
   // the parsed trees
   std::vector<std::unique_ptr<AST>> parsedTrees;

   // Begin a scan (implemented in scanner.l)
   void beginScan(std::istream& in);
   // End a scan (implemented in scanner.l)
   void endScan();

   // Register a parsed AST
   void registerAST(std::vector<std::unique_ptr<AST>>&& trees);

   public:
   // Constructor
   explicit SchemaParseContext(bool trace_scanning = false, bool trace_parsing = false);
   // Destructor
   ~SchemaParseContext();

   // Parse an istream
   void parse(std::istream& in);

   // Throw an error
   void Error(uint32_t line, uint32_t column, const std::string& err);
   // Throw an error
   void Error(const std::string& m);

   [[nodiscard]] std::vector<AST*> getParsedTrees() const;
   [[nodiscard]] AST* getParsedTrees(size_t idx) const;
   [[nodiscard]] size_t parsedTreesSize() const { return parsedTrees.size(); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::parser
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_PARSER_SCHEMAPARSECONTEXT_H_