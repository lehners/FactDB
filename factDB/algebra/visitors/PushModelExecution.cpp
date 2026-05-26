// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/PushModelExecution.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/algebra/translator/ChainingHashtableTranslator.hpp"
#include "factDB/infra/ChainingHashTable.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include <cassert>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
std::mutex outputMutex;
// ---------------------------------------------------------------------------------------------------
PushModelExecution::PushModelExecution(const factDB::Database& db, std::ostream& out) : AlgebraVisitorDB(db), outStream(out) {
}
// ---------------------------------------------------------------------------------------------------
size_t PushModelExecution::getIUIdx(const factDB::IU* iu) const {
   assert(ius.contains(iu));
   return ius.find(iu)->second;
}
// ---------------------------------------------------------------------------------------------------
void PushModelExecution::allocateRegister(const OrderedIUSet& requiredIUs) {
   for (auto& iu : requiredIUs) {
      if (!ius.contains(iu)) {
         auto iusSize = ius.size();
         ius[iu] = iusSize;
      }
   }
}
// ---------------------------------------------------------------------------------------------------
void PushModelExecution::visitProduce(const Count& count) {
   tupleCounts = 0;
   count.getChild()->produce(*this);

   size_t totalCount = 0;
   for (auto& tupleCo : tupleCounts)
      totalCount += tupleCo;

   outputMutex.lock();
   outStream << totalCount << std::endl;
   outputMutex.unlock();
}
// ---------------------------------------------------------------------------------------------------
void PushModelExecution::visitConsume([[maybe_unused]] const AlgebraVisitor::ConsumeStage stage, const Count& count, [[maybe_unused]] const Operator* caller) {
   assert(caller == count.getChild().get());
   assert(stage == ConsumeStage::InLoop);
   allocateRegister(count.getRequiredIus());
   tupleCounts.local()++;
}
// ---------------------------------------------------------------------------------------------------
void PushModelExecution::visitProduce(const Print& print) {
   allocateRegister(print.getRequiredIus());
   print.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void PushModelExecution::visitConsume([[maybe_unused]] const AlgebraVisitor::ConsumeStage stage, const Print& print, [[maybe_unused]] const Operator* caller) {
   assert(caller == print.getChild().get());
   assert(stage == ConsumeStage::InLoop);

   std::stringstream res;
   for (size_t idx = 0; [[maybe_unused]] auto& iu : print.getRequiredIus()) {
      auto iuIdx = getIUIdx(iu);
      res << (idx == 0 ? "" : print.getSeperator()) << iuRegister.local()[iuIdx];
      ++idx;
   }
   outputMutex.lock();
   outStream << res.str() << std::endl;
   outputMutex.unlock();
}
// ---------------------------------------------------------------------------------------------------
void PushModelExecution::visitProduce(const InnerJoin& join) {
   factDB::ChainingHashTable<std::vector<RuntimeValue>, std::vector<RuntimeValue>> ht;
   opStorage[&join] = &ht;

   auto pipelineRegister = std::move(iuRegister.local());
   auto pipelineIUs = std::move(ius);

   allocateRegister(join.getLeftRequired());
   join.getLeftChild()->produce(*this);

   iuRegister.local() = std::move(pipelineRegister);
   ius = std::move(pipelineIUs);

   allocateRegister(join.getRightRequiredPipeline().difference(join.getParentRequired()));

   ht.finalizeParallel();
   join.getRightChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
std::vector<RuntimeValue> PushModelExecution::createTuple(const factDB::OrderedIUSet& requiredIUs, const std::vector<RuntimeValue>& localRegister) const {
   std::vector<RuntimeValue> values;
   values.reserve(localRegister.size());
   for (auto& iu : requiredIUs) {
      auto iuIdx = getIUIdx(iu);
      values.emplace_back(localRegister[iuIdx]);
   }
   return values;
}
// ---------------------------------------------------------------------------------------------------
void PushModelExecution::visitConsume([[maybe_unused]] const AlgebraVisitor::ConsumeStage stage, const InnerJoin& join, const Operator* caller) {
   assert(stage == ConsumeStage::InLoop);
   assertAxiom(opStorage.contains(&join));

   auto& hashTable = *static_cast<factDB::ChainingHashTable<std::vector<RuntimeValue>, std::vector<RuntimeValue>>*>(opStorage[&join]);

   auto hashKey = [](std::vector<RuntimeValue>& keys) {
      uint64_t keyHash = 0;
      for (auto& k : keys)
         keyHash |= k.hash();
      return keyHash;
   };
   auto& localRegister = iuRegister.local();
   if (caller == join.getLeftChild().get()) {
      assert(!algebra::HashTableTranslator::getLeftKeys(join).empty() && "You want to execute a join without any join condition, i.e. a large cross product.");
      auto keys = createTuple(algebra::HashTableTranslator::getLeftKeys(join), localRegister);
      auto values = createTuple(join.getLeftRequired(), localRegister);
      auto hash = hashKey(keys);

      hashTable.insert(hash, std::move(keys), std::move(values));
   } else {
      assert(caller == join.getRightChild().get());

      auto probeKeys = createTuple(algebra::HashTableTranslator::getRightKeys(join), localRegister);

      auto producedIUs = join.getParentRequired().merge(join.getParentRequiredPipeline());
      std::unordered_map<const IU*, size_t, IUPointerHash, IUPointerEqual> leftRequiredIUMap;
      for (size_t idx = 0; auto& iu : join.getLeftRequired())
         leftRequiredIUMap[iu] = idx++;

      auto iter = hashTable.lookup(hashKey(probeKeys));
      while (iter != nullptr) {
         if (iter->key == probeKeys) {
            for (const IU* iu : producedIUs) {
               auto registerIdx = getIUIdx(iu);
               if (join.getJoinType() != JoinType::RightSemi && join.getLeftRequired().contains(iu)) {
                  localRegister[registerIdx] = iter->val[leftRequiredIUMap[iu]];
               } else {
                  assert(join.getRightRequired().contains(iu) || join.getRightRequiredPipeline().contains(iu));
                  assert(ius.contains(iu));
                  // do nothing since the value is already stored in its slot
               }
            }
            join.consumerConsume(ConsumeStage::InLoop, *this);

            if (join.getJoinType() == JoinType::RightSemi)
               break; // do not query whole chain for semi join
         }
         iter = hashTable.lookupNext(iter);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
void PushModelExecution::visitProduce(const factDB::algebra::TableScan& tableScan) {
   allocateRegister(tableScan.getRequiredIus());
   const BaseTable& table = database.getTable(tableScan.getTable());
   tbb::parallel_for(tbb::blocked_range<size_t>(0, table.size(), 100), [&](tbb::blocked_range<size_t> range) {
      auto& localRegister = iuRegister.local();
      if (localRegister.size() != ius.size()) {
         localRegister.resize(ius.size());
      }

      for (size_t idx = range.begin(); idx != range.end(); ++idx) {
         for (auto& iu : tableScan.getRequiredIus()) {
            assert(ius.contains(iu));
            auto registerIdx = getIUIdx(iu);
            localRegister[registerIdx] = table.get(idx, *iu);
         }
         tableScan.consumerConsume(ConsumeStage::InLoop, *this);
      }
   });
}
// ---------------------------------------------------------------------------------------------------
void PushModelExecution::visitProduce(const Selection& selection) {
   selection.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void PushModelExecution::visitConsume([[maybe_unused]] const ConsumeStage stage, const Selection& selection, [[maybe_unused]] const Operator* caller) {
   assert(caller == selection.getChild().get());
   assert(stage == ConsumeStage::InLoop);

   bool result = Bool::fromRTV(selection.getPredicate().evaluate(iuRegister.local(), ius)).toBool();
   if (result)
      selection.consumerConsume(ConsumeStage::InLoop, *this);
}
// ---------------------------------------------------------------------------------------------------
void PushModelExecution::visitProduce(const CrossProduct&) {
   not_implemented();
}
// ---------------------------------------------------------------------------------------------------
void PushModelExecution::visitConsume(const ConsumeStage, const CrossProduct&, const Operator*) {
   not_implemented();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------