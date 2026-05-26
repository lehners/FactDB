#ifndef H_FACTDB_SQLGenerator_HPP
#define H_FACTDB_SQLGenerator_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/queryc/writer/FileWriter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class Expression;
struct OrderedIUSet;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
class SQLGenerator : public DefaultVisitor {
   private:
   enum SQLStage {
      Tables,
      Predicates
   };
   using TreeStructure = std::pair<std::unique_ptr<factDB::algebra::Operator>, factDB::OrderedIUSet>;

   SQLStage stage = Tables;
   FileWriter out;
   const Database& database;

   private:
   FileWriter& printBinaryExpression(const Expression& expression, std::string_view sep);
   FileWriter& printIU(const IU& iu);
   FileWriter& printExpression(const Expression& expression, bool requiresSeparator = true);

   public:
   explicit SQLGenerator(std::ostream& writer, const Database& db) : out(writer), database(db) { out.useFinalNewline(false); }
   ~SQLGenerator() override = default;

   void visitProduce(const InnerJoin& join) override;
   void visitProduce(const Print& print) override;
   void visitProduce(const Count& count) override;
   void visitProduce(const Selection& selection) override;
   void visitProduce(const TableScan& tableScan) override;
   void visitProduce(const CrossProduct& crossProduct) override;

   // gen SQL for intermediate tree, primarily meant for Query Signature
   static void genSQL(algebra::Operator* tree, const Database& db, std::ostream& ostream);
   static void genSQL(TreeStructure&& tree, const Database& db, std::ostream& ostream);
   static void genSQL(std::unique_ptr<factDB::algebra::Operator>& tree, const Database& db, factDB::OrderedIUSet& requiredColumns, std::ostream& ostream);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_SQLGenerator_HPP
