#ifndef H_factdb_algebra_visitors_GraphvizQueryPlan
#define H_factdb_algebra_visitors_GraphvizQueryPlan
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/queryc/writer/FileWriter.hpp"
#include <iostream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
class GraphvizQueryPlan : public AlgebraVisitorDB {
   private:
   FileWriter out;

   public:
   explicit GraphvizQueryPlan(const Database& db, std::ostream& writer);
   virtual ~GraphvizQueryPlan() override = default;

   void open() override;
   void close() override;

   void visitProduce(const InnerJoin& join) override;
   void visitProduce(const Print& print) override;
   void visitProduce(const Selection& selection) override;
   void visitProduce(const TableScan& tableScan) override;
   void visitProduce(const Count& tableScan) override;
   void visitProduce(const CrossProduct& crossProduct) override;

   static void plotSubPlan(const Database& db, const Operator& op, std::ostream& out = std::cout);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_algebra_visitors_GraphvizQueryPlan
