#include "factDB/util/TaggedPointer.hpp"
#include "gtest/gtest.h"
// ---------------------------------------------------------------------------------------------------
using namespace std;
using namespace factDB::util;
// ---------------------------------------------------------------------------------------------------
namespace factDB::test {
// ---------------------------------------------------------------------------------------------------
TEST(factDB_util_TaggablePointer, Simple) {
   size_t a = 10;
   {
      TypedTaggedPointer<size_t> ptr(&a);
      ASSERT_EQ(ptr.getPointer(), &a);
   }

   {
      TypedTaggedPointer<size_t> ptr(&a, 123);
      ASSERT_EQ(ptr.getPointer(), &a);
      if constexpr (TaggedPointer::useTagging) {
         ASSERT_EQ(ptr.getTag(), 123);
      }

      ptr.updateTag(345);
      ASSERT_EQ(ptr.getPointer(), &a);
      if constexpr (TaggedPointer::useTagging) {
         ASSERT_EQ(ptr.getTag(), 345);
      }

      ptr.updateTag(65535);
      ASSERT_EQ(ptr.getPointer(), &a);
      if constexpr (TaggedPointer::useTagging) {
         ASSERT_EQ(ptr.getTag(), 65535);
      }
   }
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::test
// ---------------------------------------------------------------------------------------------------