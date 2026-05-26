#ifndef H_FACTDB_ALGEBRAVISITOR_HPP
#define H_FACTDB_ALGEBRAVISITOR_HPP
// ---------------------------------------------------------------------------------------------------
#include <cstdint>
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB { class Database; } // namespace factDB
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
class Count;
class CrossProduct;
class InnerJoin;
class Operator;
class Print;
class Selection;
class TableScan;
// ---------------------------------------------------------------------------------------------------
namespace visitors {
// ---------------------------------------------------------------------------------------------------
enum class PrepareStage : uint8_t { BeforeCall,
                                    AfterCall };
// ---------------------------------------------------------------------------------------------------
enum class ConsumeStage : uint8_t { Prepare,
                                    TLSPrepare,
                                    BeforeLoop,
                                    InLoop,
                                    AfterLoop };
// ---------------------------------------------------------------------------------------------------
class AlgebraVisitor {
   protected:
   void setConsumer(Operator& child, Operator* parent);

   public:
   virtual ~AlgebraVisitor() = default;

   using Stage = visitors::PrepareStage;
   using ConsumeStage = visitors::ConsumeStage;

   virtual void visitPrepare(const Stage, const InnerJoin&) {}
   virtual void visitPrepare(const Stage, const Print&) {}
   virtual void visitPrepare(const Stage, const Selection&) {}
   virtual void visitPrepare(const Stage, const TableScan&) {}
   virtual void visitPrepare(const Stage, const Count&) {}
   virtual void visitPrepare(const Stage, const CrossProduct&) {}

   virtual void visitProduce(const InnerJoin& join) = 0;
   virtual void visitProduce(const Print& print) = 0;
   virtual void visitProduce(const Selection& selection) = 0;
   virtual void visitProduce(const TableScan& tableScan) = 0;
   virtual void visitProduce(const Count& tableScan) = 0;
   virtual void visitProduce(const CrossProduct& crossProduct) = 0;

   virtual void visitProduce(InnerJoin& innerJoin) { visitProduce(const_cast<const InnerJoin&>(innerJoin)); }
   virtual void visitProduce(Print& print) { visitProduce(const_cast<const Print&>(print)); }
   virtual void visitProduce(Selection& selection) { visitProduce(const_cast<const Selection&>(selection)); }
   virtual void visitProduce(TableScan& tableScan) { visitProduce(const_cast<const TableScan&>(tableScan)); }
   virtual void visitProduce(Count& count) { visitProduce(const_cast<const Count&>(count)); }
   virtual void visitProduce(CrossProduct& crossProduct) { visitProduce(const_cast<const CrossProduct&>(crossProduct)); }

   virtual void visitConsume(const ConsumeStage, const InnerJoin& /*join*/, const Operator* /*caller*/) {}
   virtual void visitConsume(const ConsumeStage, const Print& /*print*/, const Operator* /*caller*/) {}
   virtual void visitConsume(const ConsumeStage, const Count& /*count*/, const Operator* /*caller*/) {}
   virtual void visitConsume(const ConsumeStage, const Selection& /*selection*/, const Operator* /*caller*/) {}
   virtual void visitConsume(const ConsumeStage, const CrossProduct& /*crossProduct*/, const Operator* /*caller*/) {}

   virtual void open() {}
   virtual void close() {}
};
// ---------------------------------------------------------------------------------------------------
class AlgebraVisitorDB : public AlgebraVisitor {
   protected:
   const Database& database;
   AlgebraVisitorDB(const Database& db) : database(db){};
};
// ---------------------------------------------------------------------------------------------------
class DefaultVisitor : public AlgebraVisitor {
   void visitProduce(const Count&) override {}
   void visitProduce(const CrossProduct&) override {}
   void visitProduce(const InnerJoin&) override {}
   void visitProduce(const Print&) override {}
   void visitProduce(const Selection&) override {}
   void visitProduce(const TableScan&) override {}
};
// ---------------------------------------------------------------------------------------------------
} // namespace visitors
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_ALGEBRAVISITOR_HPP
