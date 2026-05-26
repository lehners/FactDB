// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/list/FastDeque.hpp"
#include <gtest/gtest.h>
// ---------------------------------------------------------------------------------------------------
using namespace factDB::infra::list;
// ---------------------------------------------------------------------------------------------------

TEST(factDB_infra_list_FastDeque, Empty) {
   FastDeque<size_t> deque;
   ASSERT_TRUE(deque.empty());
   ASSERT_EQ(deque.size(), 0);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_list_FastDeque, Simple) {
   FastDeque<size_t> deque;

   deque.emplace_back(1);

   ASSERT_TRUE(!deque.empty());
   ASSERT_EQ(deque.size(), 1);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_list_FastDeque, Resize) {
   FastDeque<size_t> deque;
   deque.resize(10);
   ASSERT_TRUE(!deque.empty());
   ASSERT_EQ(deque.size(), 10);

   for (size_t i = 0; i != 10; ++i) {
      deque[i] = i;
   }

   ASSERT_TRUE(!deque.empty());
   ASSERT_EQ(deque.size(), 10);

   deque.emplace_back(1);

   ASSERT_TRUE(!deque.empty());
   ASSERT_EQ(deque.size(), 11);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_list_FastDeque, Iterate) {
   auto testDequeIter = [](size_t size) {
      FastDeque<size_t> deque;

      for (size_t i = 0; i != size; ++i) {
         deque.emplace_back(i);
      }

      ASSERT_EQ(deque.size(), size);
      size_t i = 0;
      for (auto& iter : deque)
         ASSERT_EQ(iter, i++);
      ASSERT_EQ(i, size);
   };
   testDequeIter(0);
   testDequeIter(1);
   testDequeIter(2);
   testDequeIter(1025);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_list_FastDeque, Iterator) {
   FastDeque<size_t> deque;
   for (size_t i = 0; i != 1025; ++i) {
      deque.emplace_back(i);
   }

   ASSERT_EQ(deque.size(), 1025);
   auto iterator = deque.begin();
   ASSERT_EQ(*iterator, 0);
   ++iterator;
   ASSERT_EQ(*iterator, 1);
   ++iterator;
   ASSERT_EQ(*iterator, 2);
   --iterator;
   ASSERT_EQ(*iterator, 1);
   iterator += 25;
   ASSERT_EQ(*iterator, 26);
   ASSERT_NE(iterator, deque.end());
   ASSERT_FALSE(iterator.isLast());
   iterator += 999;
   ASSERT_EQ(iterator, deque.end());
   ASSERT_TRUE(iterator.isLast());
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_list_FastDeque, ResizeInsertMany) {
   for (size_t i = 1; i != 1025; ++i) {
      FastDeque<size_t> deque;
      deque.resize(i - 1);
      deque.emplace_back(i);
      ASSERT_EQ(deque.size(), i);
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_list_FastDeque, MoveIter) {
   auto testDequeIter = [](size_t size) {
      FastDeque<size_t> deque;

      for (size_t i = 0; i != size; ++i) {
         deque.emplace_back(i);
      }

      FastDeque<size_t> otherDeque = std::move(deque);

      ASSERT_EQ(otherDeque.size(), size);
      size_t i = 0;
      for (auto& iter : otherDeque)
         ASSERT_EQ(iter, i++);
      ASSERT_EQ(i, size);
   };
   testDequeIter(0);
   testDequeIter(1);
   testDequeIter(2);
   testDequeIter(1025);
}
// ---------------------------------------------------------------------------------------------------
