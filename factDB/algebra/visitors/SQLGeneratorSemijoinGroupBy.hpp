#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/queryc/writer/FileWriter.hpp"
#include <unordered_set>
#include <vector>
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
class SQLGeneratorSemijoinGroupBy : public DefaultVisitor {
   private:
   enum SQLStage {
      Tables,
      Predicates
   };
   using TreeStructure = std::pair<std::unique_ptr<factDB::algebra::Operator>, factDB::OrderedIUSet>;
   using AliasMap = std::unordered_set<std::string_view>;

   std::vector<std::string> tables;
   std::vector<std::string> conditions;


   SQLStage stage = Tables;
   FileWriter out;
   const Database& database;
   AliasMap aliasMap;
   size_t semijoin_id = 0;

   private:
   static FileWriter& printBinaryExpression(FileWriter& writer, const Expression& expression, const AliasMap& map, std::string_view sep);
   static FWContainer printIU(const IU& iu, const AliasMap& map);
   static FileWriter& printExpression(FileWriter& writer, const Expression& expression,const AliasMap& map);

   public:
   explicit SQLGeneratorSemijoinGroupBy(std::ostream& writer, const Database& db) : out(writer), database(db) { out.useFinalNewline(false); }
   ~SQLGeneratorSemijoinGroupBy() override = default;

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
