// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/list/tbb/ConcurrentVector.hpp"
#include "factDB/infra/list/tbb/InlineWrapper.hpp"
#include "tbb/concurrent_vector.h"
#include <gtest/gtest.h>
// ---------------------------------------------------------------------------------------------------
using namespace factDB::infra::list;
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_list_inlineList, Empty) {
   using InlineListType = InlineWrapper<size_t, factDB::infra::list::tbb::concurrent_vector<size_t>>;
   {
      InlineListType theList;

      ASSERT_TRUE(theList.empty());
      theList.emplace_back(1);
      ASSERT_FALSE(theList.empty());
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_list_inlineList, EmplaceBack) {
   using InlineListType = InlineWrapper<size_t, factDB::infra::list::tbb::concurrent_vector<size_t>>;
   {
      InlineListType theList;

      ASSERT_EQ(theList.size(), 0);
      for (size_t i = 0; i != 10; ++i) {
         theList.emplace_back(i);
         ASSERT_EQ(theList.size(), i + 1);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_list_inlineList, Values) {
   using InlineListType = InlineWrapper<size_t, factDB::infra::list::tbb::concurrent_vector<size_t>>;
   for (size_t curSize = 1; curSize <= 20; ++curSize) {
      InlineListType theList;
      ASSERT_TRUE(theList.empty());

      for (size_t i = 0; i < curSize; ++i) {
         theList.emplace_back(i);
      }

      ASSERT_EQ(theList.size(), curSize);

      size_t value = 0;
      for (auto iter = theList.begin(); iter != theList.end(); ++iter) {
         ASSERT_LT(value, curSize);
         ASSERT_EQ(*iter, value++);
      }
      ASSERT_EQ(value, curSize);
   }
}
// ---------------------------------------------------------------------------------------------------