#ifndef H_factdb_algebra_visitors_QueryPlanWriter
#define H_factdb_algebra_visitors_QueryPlanWriter

#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include <nlohmann/json.hpp>

namespace factDB::algebra::visitors {

class QueryPlanWriter : public AlgebraVisitorDB {
   private:
   std::ostream& out;
   nlohmann::json cur_level;

   public:
   explicit QueryPlanWriter(const Database& db, std::ostream& writer);
   virtual ~QueryPlanWriter() override = default;

   void visitProduce(const InnerJoin& join) override;
   void visitProduce(const Print& print) override;
   void visitProduce(const Selection& selection) override;
   void visitProduce(const TableScan& tableScan) override;
   void visitProduce(const Count& tableScan) override;
   void visitProduce(const CrossProduct& tableScan) override;

   void close() override;
};

class QueryPlanReader {
   public:
   static std::unique_ptr<Operator> read_operator(const Database& db, nlohmann::json::reference& json);
};

} // namespace factDB::algebra::visitors

#endif // H_factdb_algebra_visitors_QueryPlanWriter
