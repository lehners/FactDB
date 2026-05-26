#include "factDB/infra/DoNotDestruct.hpp"
#include <unordered_set>
#include <gtest/gtest.h>
// ---------------------------------------------------------------------------------------------------
using namespace factDB::infra;
// ---------------------------------------------------------------------------------------------------
struct A {
   int doSth() { return 123; }
   int doSth() const { return 456; }
   void destruct() { ASSERT_FALSE(true); }
   ~A() { destruct(); }
};
// ---------------------------------------------------------------------------------------------------
TEST(factDB_infra_DoNotDestruct, Simple) {
   DoNotDestruct<A> a;
   auto& av = a.value();
   const auto& avc = a.value();
   ASSERT_EQ(av.doSth(), 123);
   ASSERT_EQ(avc.doSth(), 456);
}
// ---------------------------------------------------------------------------------------------------
