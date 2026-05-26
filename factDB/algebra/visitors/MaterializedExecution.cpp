// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/MaterializedExecution.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/algebra/translator/ChainingHashtableTranslator.hpp"
#include "factDB/infra/ChainingHashTable.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "tbb/tbb.h"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
MaterializedExecution::MaterializedExecution(const factDB::Database& db, std::ostream& out) : AlgebraVisitorDB(db), outStream(out) {
}
// ---------------------------------------------------------------------------------------------------
void MaterializedExecution::visitProduce(const factDB::algebra::TableScan& tableScan) {
   const BaseTable& table = database.getTable(tableScan.getTable());
   result.reserve(database.getTable(tableScan.getTable()).size());
   std::mutex mutex;
   tbb::parallel_for(tbb::blocked_range<size_t>(0, table.size(), 100), [&](tbb::blocked_range<size_t> range) {
      for (auto idx = range.begin(); idx != range.end(); ++idx) {
         mutex.lock();
         auto& curTuple = result.emplace_back();
         mutex.unlock();
         curTuple.reserve(tableScan.getRequiredIus().size());
         for (auto& iu : tableScan.getRequiredIus()) {
            curTuple.emplace_back(table.get(idx, *iu));
         }
      }
   });
   for (size_t idx = 0; auto& curIU : tableScan.getRequiredIus()) {
      ius[curIU] = idx++;
   }
}
// ---------------------------------------------------------------------------------------------------
void MaterializedExecution::visitProduce(const factDB::algebra::InnerJoin& join) {
   join.getLeftChild()->produce(*this);
   // create hashtable

   std::unordered_map<const IU*, size_t, IUPointerHash, IUPointerEqual> leftRequiredIUMap;
   for (size_t idx = 0; auto& iu : join.getLeftRequired())
      leftRequiredIUMap[iu] = idx++;

   auto createTuple = [&]<typename T>(const T& requiredIUs, std::vector<RuntimeValue>& inputTuple) {
      std::vector<RuntimeValue> values;
      values.reserve(requiredIUs.size());
      for (auto& iu : requiredIUs) {
         assert(ius.contains(iu));
         auto iuIdx = ius[iu];
         values.emplace_back(inputTuple[iuIdx]);
      }
      return values;
   };

   auto hashKey = [](std::vector<RuntimeValue>& keys) {
      uint64_t keyHash = 0;
      for (auto& k : keys)
         keyHash |= k.hash();
      return keyHash;
   };

   factDB::ChainingHashTable<std::vector<RuntimeValue>, std::vector<RuntimeValue>> hashMap;
   tbb::parallel_for(tbb::blocked_range<size_t>(0, result.size(), 100), [&](tbb::blocked_range<size_t> range) {
      for (auto idx = range.begin(); idx != range.end(); ++idx) {
         auto& val = result[idx];
         auto keys = createTuple(algebra::HashTableTranslator::getLeftKeys(join), val);
         auto values = createTuple(join.getLeftRequired(), val);
         auto hash = hashKey(keys);

         hashMap.insert(hash, std::move(keys), std::move(values));
      }
   });

   hashMap.finalize<true>();

   // right child
   ius.clear();
   result.clear();
   join.getRightChild()->produce(*this);

   TupleList joinResult;
   auto producedIUs = join.getParentRequired().merge(join.getParentRequiredPipeline());

   tbb::enumerable_thread_specific<std::vector<std::vector<RuntimeValue>>> resultTLS;

   tbb::parallel_for(tbb::blocked_range<size_t>(0, result.size(), 100), [&](tbb::blocked_range<size_t> range) {
      for (auto idx = range.begin(); idx != range.end(); ++idx) {
         auto& val = result[idx];
         auto probeKeys = createTuple(algebra::HashTableTranslator::getRightKeys(join), val);

         auto iter = hashMap.lookup(hashKey(probeKeys));
         while (iter != nullptr) {
            if (iter->key == probeKeys) {
               auto& curTuple = resultTLS.local().emplace_back();
               curTuple.reserve(producedIUs.size());
               for (const IU* iu : producedIUs) {
                  if (join.getJoinType() != JoinType::RightSemi && join.getLeftRequired().contains(iu)) {
                     curTuple.emplace_back(iter->val[leftRequiredIUMap[iu]]);
                  } else {
                     assert(join.getRightRequired().contains(iu));
                     assert(ius.contains(iu));
                     curTuple.emplace_back(val[ius[iu]]);
                  }
               }
               if (join.getJoinType() == JoinType::RightSemi)
                  break; // stop after first match for right semi-join
            }
            iter = hashMap.lookupNext(iter);
         }
      }
   });

   size_t resultSize = 0;
   for (auto& tls : resultTLS)
      resultSize += tls.size();
   result.reserve(resultSize);

   result = resultTLS.combine(
      [](std::vector<std::vector<RuntimeValue>>& x, const std::vector<std::vector<RuntimeValue>>& y) {
         std::size_t n = x.size();
         x.resize(n + y.size());
         std::move(y.begin(), y.end(), x.begin() + n); // NOLINT(bugprone-narrowing-conversions)
         return x;
      });

   ius.clear();
   for (size_t idx = 0; const IU* iu : producedIUs) { // set iu mapping for next operator
      ius[iu] = idx++;
   }
}
// ---------------------------------------------------------------------------------------------------
void MaterializedExecution::visitProduce(const factDB::algebra::Print& print) {
   print.getChild()->produce(*this);
   for (auto& tuple : result) {
      for (bool first = true; auto& iu : print.getRequiredIus()) {
         assert(ius.contains(iu));
         outStream << (first ? "" : print.getSeperator()) << tuple[ius[iu]];
         first = false;
      }
      outStream << std::endl;
   }
}
// ---------------------------------------------------------------------------------------------------
void MaterializedExecution::visitProduce(const factDB::algebra::Count& count) {
   count.getChild()->produce(*this);
   outStream << result.size() << std::endl;
}
// ---------------------------------------------------------------------------------------------------
void MaterializedExecution::visitProduce(const CrossProduct&) {
   not_implemented();
}
// ---------------------------------------------------------------------------------------------------
void MaterializedExecution::visitProduce(const factDB::algebra::Selection& selection) {
   selection.getChild()->produce(*this);
   size_t writeIdx = 0;
   for (auto& cur : result) {
      result[writeIdx] = cur;
      bool inc = Bool::fromRTV(selection.getPredicate().evaluate(cur, ius)).toBool();
      writeIdx += inc;
   }
   result.resize(writeIdx);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------