#include "factDB/infra/ChainingHashTable.hpp"
#include <gtest/gtest.h>
// ---------------------------------------------------------------------------------------------------
using namespace factDB;
using namespace std;
// ---------------------------------------------------------------------------------------------------
template <typename K, typename V>
using HashTable = factDB::ChainingHashTable<K, V>;
// ---------------------------------------------------------------------------------------------------
template <typename K, typename V>
static std::vector<int> getDataList(const HashTable<K, V>& ht, uint64_t hash) {
   std::vector<int> result;
   for (auto cur = ht.lookup(hash); cur; cur = ht.lookupNext(cur)) {
      result.push_back(cur->val);
   }
   return result;
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_ChainingHashTable, ConstructEmpty) {
   HashTable<int, int> ht;
   ASSERT_EQ(ht.size(), 0);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_ChainingHashTable, InsertContains) {
   HashTable<int, int> ht;
   ASSERT_EQ(ht.size(), 0);
   ht.insert(123, 456, 456);

   ht.finalizeSingleThreaded();
   ASSERT_TRUE(ht.couldContain(123));
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_ChainingHashTable, InsertContainsConst) {
   HashTable<int, int> ht;
   const HashTable<int, int>& constHt = ht;
   EXPECT_EQ(constHt.size(), 0);
   ht.insert(123, 456, 456);

   ht.finalizeSingleThreaded();
   EXPECT_TRUE(constHt.couldContain(123));
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_ChainingHashTable, InsertLookup) {
   HashTable<int, int> ht;
   EXPECT_EQ(ht.size(), 0);
   ht.insert(123, 234, 456);

   ht.finalizeSingleThreaded();
   EXPECT_EQ(ht.size(), 1);
   auto lookup = ht.lookup(123);
   EXPECT_NE(lookup, nullptr);
   EXPECT_EQ(lookup->key, 234);
   EXPECT_EQ(lookup->val, 456);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_ChainingHashTable, SubscriptInsert) {
   HashTable<int, int> ht;

   ht.insert(123, 456, 568);
   ht.insert(234, 567, 567);
   ht.insert(234, 567, 789);

   ht.finalizeSingleThreaded();
   EXPECT_TRUE(ht.contains(123));
   EXPECT_TRUE(ht.contains(234));

   auto resultVec = getDataList(ht, 234);
   EXPECT_EQ(resultVec.size(), 2);
   EXPECT_NE(find(resultVec.begin(), resultVec.end(), 789), resultVec.end());
   EXPECT_NE(find(resultVec.begin(), resultVec.end(), 567), resultVec.end());

   resultVec = getDataList(ht, 123);
   EXPECT_EQ(resultVec.size(), 1);
   EXPECT_NE(find(resultVec.begin(), resultVec.end(), 568), resultVec.end());
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_ChainingHashTable, InsertMany) {
   HashTable<int, int> ht;
   std::vector<int> values(1000);
   for (int i = 0; i < 1000; ++i) {
      // SCOPED_TRACE(i);
      values.push_back(i);
      uint64_t key = (i + 43) * 1327; // NOLINT
      ht.insert(key, i, int(i));
   }
   ht.finalizeSingleThreaded();
   for (int i = 0; i < 1000; ++i) {
      //SCOPED_TRACE(i);
      int key = (i + 43) * 1327;
      EXPECT_TRUE(ht.contains(key));
      EXPECT_EQ(ht.lookup(key)->val, i);
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_ChainingHashTable, Move) {
   HashTable<int, int> ht1;
   ht1.insert(123, 456, 456);
   ht1.insert(234, 567, 456);

   ht1.finalizeSingleThreaded();
   EXPECT_TRUE(ht1.contains(123));
   EXPECT_TRUE(ht1.contains(234));
}
// ---------------------------------------------------------------------------------------------------
