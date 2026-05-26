#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/umbra/UmbraConnector.hpp"
#include "factDB/algebra/visitors/umbra/UmbraServerConnector.hpp"
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/queryc/writer/FileWriter.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
class UmbraConnector;
// ---------------------------------------------------------------------------------------------------
class UmbraVisitor : public AlgebraVisitorDB {
   protected:
   FileWriter out;

   public:
   UmbraVisitor(const Database& db, std::ostream& writer);

   // std::string prepareProduce(Print& print) const;

   void visitProduce(Print& print) override = 0;
   void visitProduce(Count& count) override = 0;

   void visitProduce(const Print&) override { unreachable(); }
   void visitProduce(const Count&) override { unreachable(); }
   void visitProduce(const InnerJoin&) override { unreachable(); }
   void visitProduce(const Selection&) override { unreachable(); }
   void visitProduce(const TableScan&) override { unreachable(); }
   void visitProduce(const CrossProduct&) override { unreachable(); }
};
// ---------------------------------------------------------------------------------------------------
class UmbraVisitorServer : public UmbraVisitor {
   UmbraServerConnector& umbraExecutor;

   public:
   UmbraVisitorServer(const Database& db, std::ostream& writer);

   void visitProduce(Print& print) override;
   void visitProduce(Count& count) override;
};
// ---------------------------------------------------------------------------------------------------
class UmbraVisitorLocal : public UmbraVisitor {
   UmbraConnector& umbraExecutor;

   public:
   UmbraVisitorLocal(const Database& db, std::ostream& writer);

   void visitProduce(Print& print) override;
   void visitProduce(Count& count) override;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
