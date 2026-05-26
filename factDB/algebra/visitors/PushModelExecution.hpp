#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/infra/IUSet.hpp"
#include "tbb/tbb.h"
#include <list>
#include <unordered_map>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class Expression;
class IU;
struct RuntimeValue;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
class PushModelExecution : public AlgebraVisitorDB {
   private:
   std::ostream& outStream;
   tbb::enumerable_thread_specific<size_t> tupleCounts;
   tbb::enumerable_thread_specific<std::vector<RuntimeValue>> iuRegister;
   std::unordered_map<const IU*, size_t, IUPointerHash, IUPointerEqual> ius;

   std::unordered_map<const Operator*, void*> opStorage;

   private:
   std::vector<RuntimeValue> createTuple(const factDB::OrderedIUSet& requiredIUs, const std::vector<RuntimeValue>& localRegister) const;
   void allocateRegister(const OrderedIUSet& requiredIUs);
   size_t getIUIdx(const IU* iu) const;

   public:
   explicit PushModelExecution(const Database& db, std::ostream& out);
   /// Destructor
   ~PushModelExecution() override = default;

   void visitProduce(const InnerJoin& join) override;
   void visitProduce(const Print& print) override;
   void visitProduce(const Selection& selection) override;
   void visitProduce(const TableScan& tableScan) override;
   void visitProduce(const Count& count) override;
   void visitProduce(const CrossProduct& count) override;

   void visitConsume(const ConsumeStage, const InnerJoin& join, const Operator* caller) override;
   void visitConsume(const ConsumeStage, const Print& print, const Operator* caller) override;
   void visitConsume(const ConsumeStage, const Count& count, const Operator* caller) override;
   void visitConsume(const ConsumeStage, const Selection& selection, const Operator* caller) override;
   void visitConsume(const ConsumeStage, const CrossProduct& crossProduct, const Operator* caller) override;

   void open() override{};
   void close() override{};
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
