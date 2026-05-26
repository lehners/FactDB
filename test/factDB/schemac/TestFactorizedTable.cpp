#include "factDB/schemac/FactorizedTable.hpp"
#include "gtest/gtest.h"
#include <tuple>
#include <unordered_map>


TEST(factDB_schemac_FactorizedTable, SingleInsert) {
   using FactTable = factDB::schemac::FactorizedTable;

   FactTable ft;

   for (int i = 0; i < 4; i++) {
      ft.insert(5, i);
   }

   ASSERT_EQ(ft.size(), 4);
   int curIdx = 0;
   for (const auto& [valA, valB] : ft) {
      ASSERT_EQ(valA.value, 5);
      ASSERT_EQ(valB[0]->value, curIdx++);
   }
}

TEST(factDB_schemac_FactorizedTable, MultipleInsert) {
   using FactTable = factDB::schemac::FactorizedTable;

   FactTable ft;
   for (int k = 0; k < 10; k++) {
      for (int i = 0; i <= k; i++) {
         ft.insert(k, i);
      }
   }

   // ASSERT_EQ(ft.size(), 4851);
   int curK = 0;
   int curI = 0;
   for (const auto& [valA, valB] : ft) {
      ASSERT_EQ(valA.value, curK);
      ASSERT_EQ(valB[0]->value, curI);
      if (curI++ == curK) {
         ++curK;
         curI = 0;
      }
   }

}

TEST(factDB_schemac_FactorizedTable, NestedFactorization) {
   using FactTable = factDB::schemac::FactorizedTable;

   FactTable ft;
   for (int k = 0; k < 10; k++) {
      for (int i = 0; i <= k; i++) {
         ft.insert(k, i);
      }
   }

   int curK = 0;
   int curI = 0;
   for (const auto& [valA, valVec] : ft.iter({0,1})) {
      ASSERT_EQ(valA.value, curK);
      ASSERT_EQ(valVec.size(),1);
      ASSERT_EQ(valVec[0]->value, curI);
      if (curI++ == curK) {
         ++curK;
         curI = 0;
      }
   }

   using KeyType = std::tuple<int, int, int>;
   struct KeyHash {
      std::size_t operator()(const KeyType& k) const { return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k); }
   };
   std::unordered_map<KeyType, bool, KeyHash> should;

   ft.appendCol();
   for (int k = 0; k < 10; k++) {
      auto* handle = ft.findKey(factDB::Integer(k));
      ASSERT_NE(handle, nullptr);
      for (int i = 0; i <= k; i++) {
         ft.insert(handle, i, 1);

         for (auto i2 = 0; i2 <= k; i2++) {
            should[std::make_tuple(k, i, i2)] = false;
         }
      }
   }

   ASSERT_EQ(ft.size(), 385);

   for (const auto& [valA, valVec] : ft.iter({0,1,2})) {
      ASSERT_EQ(valVec.size(), 2);
      should[std::make_tuple(valA.value, valVec[0]->value, valVec[1]->value)] = true;
   }

   for (auto& a : should) {
      if (!a.second) {
         std::cout << "missing: (" << std::get<0>(a.first) << ", " << std::get<1>(a.first) << ", " << std::get<2>(a.first) << ")" << std::endl;
         ASSERT_FALSE(true);
      }
   }
}