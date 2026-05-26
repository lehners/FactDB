#include "factDB/infra/BitSet.hpp"
#include <unordered_set>
#include <gtest/gtest.h>
// ---------------------------------------------------------------------------------------------------
using namespace factDB::infra;
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_BitSet64, Empty) {
   {
      BitSet64 bitSet64;
      ASSERT_TRUE(bitSet64.empty());
      bitSet64.insert(1);
      ASSERT_FALSE(bitSet64.empty());
   }
   {
      BitSet64 bs({1, 2, 3});
      ASSERT_FALSE(bs.empty());
   }
   {
      BitSet64 bs({});
      ASSERT_TRUE(bs.empty());
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_BitSet64, contains) {
   BitSet64 bs({1, 5, 10, 50, 63});

   ASSERT_FALSE(bs.empty());
   for (unsigned i = 0; i < 100; ++i) {
      switch (i) {
         case 1:
         case 5:
         case 10:
         case 50:
         case 63:
            ASSERT_EQ(1, bs.count(i));
            ASSERT_TRUE(bs.contains(i));
            break;
         default:
            ASSERT_EQ(0, bs.count(i));
            ASSERT_FALSE(bs.contains(i));
            break;
      }
   }

   BitSet64 bs2(10, 15, factDB::infra::BitSet64::RangeEnum::Range);
   for (unsigned i = 0; i < 100; ++i) {
      ASSERT_EQ(10 <= i && i < 15, bs2.contains(i));
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_BitSet64, subset) {
   BitSet64 bs({1, 5, 10, 50, 63});

   BitSet64 bs41({5, 10, 50, 63});
   BitSet64 bs42({1, 10, 50, 63});
   BitSet64 bs43({1, 5, 50, 63});
   BitSet64 bs44({1, 5, 10, 63});
   BitSet64 bs45({1, 5, 10, 50});

   BitSet64 bs31({5, 50, 63});
   BitSet64 bs32({1, 10, 63});
   BitSet64 bs33({1, 5, 50});
   BitSet64 bs34({5, 10, 50});

   BitSet64 bs11({1});
   BitSet64 bs12({5});
   BitSet64 bs13({10});
   BitSet64 bs14({50});
   BitSet64 bs15({63});

   for (auto& i : {bs11, bs12, bs13, bs14, bs15, bs31, bs32, bs33, bs34})
      ASSERT_TRUE(i.isSubsetOf(bs));

   for (auto& i : {bs31, bs32, bs33, bs34})
      for (auto& j : {bs31, bs32, bs33, bs34})
         ASSERT_EQ(i == j, i.isSubsetOf(j));

   for (size_t idx = 0; auto& bitset : {bs41, bs42, bs43, bs44}) {
      for (size_t idx2 = 0; auto& singleSet : {bs11, bs12, bs13, bs14, bs15}) {
         ASSERT_EQ(idx2 == idx, !singleSet.isSubsetOf(bitset));
         ASSERT_FALSE(bitset.isSubsetOf(singleSet));
         ++idx2;
      }
      ++idx;
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_BitSet64, back) {
   BitSet64 bs0{0};
   BitSet64 bs1{1, 5, 10, 63};
   BitSet64 bs2{2, 10, 25, 60};
   BitSet64 bs3{1};

   ASSERT_EQ(bs0.back(), 0);
   ASSERT_EQ(bs1.back(), 63);
   ASSERT_EQ(bs2.back(), 60);
   ASSERT_EQ(bs3.back(), 1);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_BitSet64, swap) {
   BitSet64 bs1{1, 5, 10, 63};
   BitSet64 bs2{2, 10, 25, 60};

   auto bs1c = bs1, bs2c = bs2;

   bs1.swap(bs2);

   ASSERT_NE(bs1c, bs1);
   ASSERT_NE(bs2c, bs2);

   ASSERT_EQ(bs1c, bs2);
   ASSERT_EQ(bs2c, bs1);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_BitSet64, iterate) {
   std::initializer_list<unsigned int> init1 = {1, 5, 10, 63};
   std::initializer_list<unsigned int> init2 = {2, 10, 25, 60};

   {
      BitSet64 bs(init1);
      auto iter1 = init1.begin();
      for (auto v : bs) {
         ASSERT_EQ(v, *iter1);
         ++iter1;
      }
   }
   {
      BitSet64 bs(init2);
      auto iter1 = init2.begin();
      for (auto v : bs) {
         ASSERT_EQ(v, *iter1);
         ++iter1;
      }
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_BitSet64, intersect) {
   BitSet64 bsA({1, 5, 10, 50, 63});
   BitSet64 bsB({2, 11, 23, 58, 17});

   for (unsigned i = 0; i < 60; ++i) {
      BitSet64 bs2({i});
      ASSERT_EQ(bsA.contains(i), bs2.doesIntersectWith(bsA));
      ASSERT_EQ(bsB.contains(i), bs2.doesIntersectWith(bsB));
   }

   // with itself
   ASSERT_TRUE(bsA.doesIntersectWith(bsA));
   ASSERT_TRUE(bsB.doesIntersectWith(bsB));
   // with other
   ASSERT_FALSE(bsA.doesIntersectWith(bsB));

   BitSet64 bsC({1, 17, 24, 26});
   ASSERT_TRUE(bsA.doesIntersectWith(bsC));
   ASSERT_TRUE(bsB.doesIntersectWith(bsC));
   ASSERT_TRUE(bsC.doesIntersectWith(bsA));
   ASSERT_TRUE(bsC.doesIntersectWith(bsB));
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_BitSet64, setOperations) {
   BitSet64 bsA({1, 5, 10, 50, 63});
   BitSet64 bsB({2, 11, 23, 58, 17});

   auto sumOp = bsA + bsB;
   auto difOp = bsA - bsB;
   auto andOp = bsA & bsB;

   for (unsigned i = 0; i != sizeof(uint64_t) * 8; ++i) {
      ASSERT_EQ(bsA.contains(i) || bsB.contains(i), sumOp.contains(i));
      ASSERT_EQ(bsA.contains(i) && !bsB.contains(i), difOp.contains(i));
      ASSERT_EQ(bsA.contains(i) && bsB.contains(i), andOp.contains(i));
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_BitSet64, nextPermutation) {
   BitSet64 curSet;
   std::unordered_set<BitSet64, BitSet64::hasher> permutations;
   do {
      ASSERT_FALSE(permutations.contains(curSet));
      permutations.insert(curSet);
      curSet = curSet.nextPermutation(3);
   } while (!curSet.empty());

   std::vector<BitSet64> expected{
      BitSet64{0, 0, 0},
      BitSet64{0, 0, 1},
      BitSet64{0, 1, 0},
      BitSet64{1, 0, 0},
      BitSet64{1, 1, 0},
      BitSet64{1, 0, 1},
      BitSet64{0, 1, 1},
      BitSet64{1, 1, 1},
   };

   ASSERT_EQ(expected.size(), permutations.size());
   for (auto& cur : expected) {
      ASSERT_TRUE(permutations.contains(cur));
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_BitSetVar, Test) {
   constexpr size_t maxSize = 1024;
   {
      BitSetVar a(maxSize, {}), b(maxSize, {3, 128, 512});
      ASSERT_EQ(a - b, BitSetVar(maxSize, {}));
      ASSERT_EQ(b - a, b);
      ASSERT_EQ(b + a, b);
      ASSERT_EQ(a + b, b);
      a -= b;
      ASSERT_EQ(a, BitSetVar(maxSize, {}));
      a += b;
      ASSERT_EQ(a, BitSetVar(maxSize, {3, 128, 512}));
      a -= b;
      ASSERT_EQ(a, BitSetVar(maxSize, {}));
      b -= a;
      ASSERT_EQ(b, BitSetVar(maxSize, {3, 128, 512}));
   }
   {
      BitSetVar a(maxSize, {3, 255, 500}), b(maxSize, {4, 254, 244});
      ASSERT_EQ((a - b), a);
      ASSERT_EQ((b - a), b);
      ASSERT_EQ((a - a), BitSetVar(maxSize, {}));
      ASSERT_EQ((b - b), BitSetVar(maxSize, {}));
   }
   {
      BitSetVar a(maxSize, {3, 255, 256, 257, 260, 500}), b(maxSize, {4, 255, 257, 259, 260, 244});
      ASSERT_EQ((a & b), BitSetVar(maxSize, {255, 257, 260}));
      a &= b;
      b &= a;
      ASSERT_EQ(a, BitSetVar(maxSize, {255, 257, 260}));
      ASSERT_EQ(b, BitSetVar(maxSize, {255, 257, 260}));
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_BitSetVar, compareVal) {
   BitSetVar bs1(127, {1, 10, 100});
   BitSetVar bs2(127, {2});
   BitSetVar bs3(127, {100, 120});
   BitSetVar bs4(127, {99, 120});

   ASSERT_TRUE(bs1.compareNumeric(bs2));
   ASSERT_TRUE(bs1.compareNumeric(bs3));
   ASSERT_TRUE(bs1.compareNumeric(bs4));

   ASSERT_FALSE(bs2.compareNumeric(bs1));
   ASSERT_TRUE(bs2.compareNumeric(bs3));
   ASSERT_TRUE(bs2.compareNumeric(bs4));

   ASSERT_FALSE(bs3.compareNumeric(bs1));
   ASSERT_FALSE(bs3.compareNumeric(bs2));
   ASSERT_FALSE(bs3.compareNumeric(bs4));

   ASSERT_FALSE(bs4.compareNumeric(bs1));
   ASSERT_FALSE(bs4.compareNumeric(bs2));
   ASSERT_TRUE(bs4.compareNumeric(bs3));

   ASSERT_FALSE(bs1.compareNumeric(bs1));
   ASSERT_FALSE(bs2.compareNumeric(bs2));
   ASSERT_FALSE(bs3.compareNumeric(bs3));
   ASSERT_FALSE(bs4.compareNumeric(bs4));
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_BitSetVar, iterate) {
   std::initializer_list<unsigned int> init1 = {1, 5, 10, 63, 100, 128, 200, 1000};
   std::initializer_list<unsigned int> init2 = {102, 110, 125, 160, 200, 1000};

   {
      BitSetVar bs(1024, init1);
      auto iter1 = init1.begin();
      for (auto v : bs) {
         ASSERT_EQ(v, *iter1);
         ++iter1;
      }
   }
   {
      BitSetVar bs(1024, init2);
      auto iter1 = init2.begin();
      for (auto v : bs) {
         ASSERT_EQ(v, *iter1);
         ++iter1;
      }
   }
}
// ---------------------------------------------------------------------------------------------------
