#include "factDB/infra/UnionFind.hpp"
#include <bitset>
#include <gtest/gtest.h>
// ---------------------------------------------------------------------------------------------------
using namespace factDB::infra;
// ---------------------------------------------------------------------------------------------------
// adapted from Umbra
// (c) 2017 Thomas Neumann
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_UnionFind, BaseSimple) {
   {
      UnionFindBase uf(3);
      ASSERT_EQ(uf.find(0), 0);
      ASSERT_EQ(uf.find(1), 1);
      ASSERT_EQ(uf.unionSets(0, 0), 0);
      ASSERT_EQ(uf.unionSets(0, 1), 0);
      ASSERT_EQ(uf.find(0), 0);
      ASSERT_EQ(uf.find(1), 0);
      ASSERT_EQ(uf.unionSets(0, 2), 0);
      ASSERT_EQ(uf.find(2), 0);
   }
   {
      UnionFindBase uf(3);
      ASSERT_EQ(uf.unionSets(0, 1), 0);
      ASSERT_EQ(uf.unionSets(2, 0), 0);
      ASSERT_EQ(uf.find(2), 0);
   }
   {
      UnionFindBase uf(4);
      ASSERT_EQ(uf.unionSets(0, 1), 0);
      ASSERT_EQ(uf.unionSets(2, 3), 2);
      ASSERT_EQ(uf.unionSets(0, 2), 0);
      ASSERT_EQ(uf.find(3), 0);
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_UnionFind, BaseOptimize) {
   UnionFindBase uf(4);
   ASSERT_EQ(uf.unionSets(0, 1), 0);
   ASSERT_EQ(uf.unionSets(2, 3), 2);
   ASSERT_EQ(uf.unionSets(0, 2), 0);
   uf.optimizeAccessPaths();

   ASSERT_EQ(uf.find(0), 0);
   ASSERT_EQ(uf.find(1), 0);
   ASSERT_EQ(uf.find(2), 0);
   ASSERT_EQ(uf.find(3), 0);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_UnionFind, BaseisSingleSet) {
   UnionFindBase uf(4);
   ASSERT_EQ(uf.unionSets(0, 1), 0);
   ASSERT_EQ(uf.unionSets(2, 3), 2);
   ASSERT_FALSE(uf.isSingleSet());

   ASSERT_EQ(uf.unionSets(0, 2), 0);
   ASSERT_TRUE(uf.isSingleSet());
}
// ---------------------------------------------------------------------------------------------------
std::vector<std::string> strs = {"zero", "one", "2", "three", "four"};
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_UnionFind, Simple) {
   if (false) {
      UnionFind<std::string> uf;
      ASSERT_EQ(uf.find(strs[0]), 0);
      ASSERT_EQ(uf.find(strs[1]), 1);
      ASSERT_EQ(uf.unionSets(strs[0], strs[0]), 0);
      ASSERT_EQ(uf.unionSets(strs[0], strs[1]), 0);
      ASSERT_EQ(uf.find(strs[0]), 0);
      ASSERT_EQ(uf.find(strs[1]), 0);
      ASSERT_EQ(uf.unionSets(strs[0], strs[2]), 0);
      ASSERT_EQ(uf.find(strs[2]), 0);
   }
   {
      UnionFind<std::string> uf;
      ASSERT_EQ(uf.unionSets(strs[0], strs[1]), 0);
      ASSERT_EQ(uf.unionSets(strs[2], strs[0]), 0);
      ASSERT_EQ(uf.find(strs[2]), 0);
   }
   {
      UnionFind<std::string> uf;
      ASSERT_EQ(uf.unionSets(strs[0], strs[1]), 0);
      ASSERT_EQ(uf.unionSets(strs[2], strs[3]), 2);
      ASSERT_EQ(uf.unionSets(strs[0], strs[2]), 0);
      ASSERT_EQ(uf.find(strs[3]), 0);
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_UnionFind, Optimize) {
   UnionFind<std::string> uf;
   ASSERT_EQ(uf.unionSets(strs[0], strs[1]), 0);
   ASSERT_EQ(uf.unionSets(strs[2], strs[3]), 2);
   ASSERT_EQ(uf.unionSets(strs[0], strs[2]), 0);
   uf.optimizeAccessPaths();

   ASSERT_EQ(uf.find(strs[0]), 0);
   ASSERT_EQ(uf.find(strs[1]), 0);
   ASSERT_EQ(uf.find(strs[2]), 0);
   ASSERT_EQ(uf.find(strs[3]), 0);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_UnionFind, isSingleSet) {
   UnionFind<std::string> uf;
   ASSERT_EQ(uf.unionSets(strs[0], strs[1]), 0);
   ASSERT_EQ(uf.unionSets(strs[2], strs[3]), 2);
   ASSERT_FALSE(uf.isSingleSet());

   ASSERT_EQ(uf.unionSets(strs[0], strs[2]), 0);
   ASSERT_TRUE(uf.isSingleSet());

   ASSERT_EQ(uf.addElement(strs[4]), 4);
   ASSERT_FALSE(uf.isSingleSet());
}
// ---------------------------------------------------------------------------------------------------
