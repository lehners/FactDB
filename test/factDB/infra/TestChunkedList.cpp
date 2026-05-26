#include "factDB/infra/Config.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/list/ListBase.hpp"
#include <gtest/gtest.h>
// ---------------------------------------------------------------------------------------------------
using namespace factDB::infra::list;
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
template <ListTypeEnum lte>
struct ListTypeWrapper {
   static constexpr ListTypeEnum val = lte;
};
// ---------------------------------------------------------------------------------------------------
template <typename lte>
struct factDB_infra_ListDispatcher : public testing::Test {
   using OwningIntList = OwningListBase<int, lte::val>;
   using NonOwningIntList = NonOwningListBase<int, lte::val>;
   static constexpr ListTypeEnum type = lte::val;
};
// ---------------------------------------------------------------------------------------------------
using TestTypes = testing::Types<
   ListTypeWrapper<ListTypeEnum::ChunkedList>,
   ListTypeWrapper<ListTypeEnum::LinkedList>,
   ListTypeWrapper<ListTypeEnum::TbbConcurrentVector>,
   ListTypeWrapper<ListTypeEnum::InlineConcurrentVector>>;
// ---------------------------------------------------------------------------------------------------
class MyTypeNames {
   public:
   template <typename T>
   static std::string GetName(int) {
      if (std::is_same<T, ListTypeWrapper<ListTypeEnum::ChunkedList>>()) return "ChunkedList";
      if (std::is_same<T, ListTypeWrapper<ListTypeEnum::LinkedList>>()) return "LinkedList";
      if (std::is_same<T, ListTypeWrapper<ListTypeEnum::TbbConcurrentVector>>()) return "TbbConcurrentVector";
      if (std::is_same<T, ListTypeWrapper<ListTypeEnum::InlineConcurrentVector>>()) return "InlineConcurrentVector";
      factDB::unreachable();
   }
};
// ---------------------------------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------------------------------
TYPED_TEST_SUITE(factDB_infra_ListDispatcher, TestTypes, MyTypeNames);
// ---------------------------------------------------------------------------------------------------
TYPED_TEST(factDB_infra_ListDispatcher, Simple) {
   typename TestFixture::OwningIntList cl;
   ASSERT_TRUE(cl.empty());
   cl.push_back(1);
   ASSERT_FALSE(cl.empty());
   ASSERT_EQ(cl.size(), 1);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
   ASSERT_EQ(cl[0], 1);
#pragma GCC diagnostic pop
}
// ---------------------------------------------------------------------------------------------------
TYPED_TEST(factDB_infra_ListDispatcher, IsLast) {
   typename TestFixture::OwningIntList cl;
   ASSERT_TRUE(cl.begin().isLast());
   cl.push_back(1);
   ASSERT_FALSE(cl.begin().isLast());
   ASSERT_TRUE((++cl.begin()).isLast());
   ASSERT_TRUE(cl.end().isLast());
}
// ---------------------------------------------------------------------------------------------------
TYPED_TEST(factDB_infra_ListDispatcher, MultiInsert) {
   typename TestFixture::OwningIntList cl;
   ASSERT_TRUE(cl.empty());
   for (int i = 0; i < 100; i++) {
      cl.push_back(i);
   }
   ASSERT_FALSE(cl.empty());
   ASSERT_EQ(cl.size(), 100);
   for (size_t i = 0; i < 100; i++) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
      ASSERT_EQ(cl[i], i);
#pragma GCC diagnostic pop
   }
}
// ---------------------------------------------------------------------------------------------------
TYPED_TEST(factDB_infra_ListDispatcher, IterateEmptyList) {
   typename TestFixture::OwningIntList cl;
   ASSERT_TRUE(cl.empty());
   typename TestFixture::NonOwningIntList cl_no;
   ASSERT_TRUE(cl_no.empty());

   for ([[maybe_unused]] auto a : cl) {
      ASSERT_FALSE(true);
   }

   for ([[maybe_unused]] auto a : cl_no) {
      ASSERT_FALSE(true);
   }
}
// ---------------------------------------------------------------------------------------------------
TYPED_TEST(factDB_infra_ListDispatcher, Iterator) {
   typename TestFixture::OwningIntList cl;
   ASSERT_TRUE(cl.empty());
   for (int i = 0; i < 100; i++) {
      cl.push_back(i);
   }
   ASSERT_FALSE(cl.empty());
   ASSERT_EQ(cl.size(), 100);
   std::vector<int> results;
   size_t idx = 0;
   for (const auto& t : cl) {
      results.push_back(t);
   }
   std::sort(results.begin(), results.end());
   for (auto r : results) {
      ASSERT_EQ(r, idx++);
   }
   ASSERT_EQ(idx, 100);
}
// ---------------------------------------------------------------------------------------------------
TYPED_TEST(factDB_infra_ListDispatcher, IteratorDecrement) {
   if (TestFixture::type == LinkedList) GTEST_SKIP();

   typename TestFixture::OwningIntList cl;
   ASSERT_TRUE(cl.empty());
   for (int i = 0; i < 100; i++) {
      cl.push_back(i);
   }
   ASSERT_FALSE(cl.empty());
   ASSERT_EQ(cl.size(), 100);
   size_t idx = 100;
   for (auto iter = cl.end(); iter != cl.begin();) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
      ASSERT_EQ(*(--iter), --idx);
#pragma GCC diagnostic pop
   }
   ASSERT_EQ(idx, 0);
}
// ---------------------------------------------------------------------------------------------------
TYPED_TEST(factDB_infra_ListDispatcher, Contains) {
   if (TestFixture::type == ChunkedList) GTEST_SKIP();
   if (TestFixture::type == TbbConcurrentVector) GTEST_SKIP();
   if (TestFixture::type == InlineConcurrentVector) GTEST_SKIP();

   std::vector<typename TestFixture::OwningIntList::ElementType*> elements;
   typename TestFixture::OwningIntList cl;
   ASSERT_TRUE(cl.empty());
   elements.reserve(100);
   for (int i = 0; i < 100; i++) {
      elements.push_back(&*cl.emplace_back(i));
   }
   ASSERT_FALSE(cl.empty());
   ASSERT_EQ(cl.size(), 100);
   int idx = 100;

   for (auto& elem : elements) {
      ASSERT_FALSE(cl.containsObject(--idx));
      ASSERT_TRUE(cl.containsObject(*elem));
   }

   ASSERT_EQ(idx, 0);
}
// ---------------------------------------------------------------------------------------------------
namespace {
template <typename TestFixture>
void testMergeLD(bool compactFirst, bool compactSecond) {
   auto do_compacts = factDB::SettingBase::getSetting<bool>("infra.ChunkedList.CompactDuringMerge")->setTemporary(true);
   typename TestFixture::OwningIntList cl1, cl2;
   typename TestFixture::NonOwningIntList cl3;
   std::vector<int> should;
   for (int i = 0; i < 50; i++) {
      int val = 10 + i;
      cl1.emplace_back(val);
      should.push_back(val);
   }
   for (int i = 0; i < 50; i++) {
      int val = 100 + i;
      cl2.emplace_back(val);
      should.push_back(val);
   }
   if (compactFirst) cl1.compactLastChunk();
   if (compactSecond) cl1.compactLastChunk();

   cl3.merge(cl1);
   cl3.merge(cl2);

   std::vector<int> res;
   for (auto s : cl3)
      res.push_back(s);
   std::sort(res.begin(), res.end());
   std::sort(should.begin(), should.end());

   ASSERT_EQ(should.size(), res.size());
   for (size_t i = 0; i < should.size(); i++) {
      ASSERT_EQ(should.size(), res.size());
   }
}
} // namespace
// ---------------------------------------------------------------------------------------------------
TYPED_TEST(factDB_infra_ListDispatcher, Compact) {
   typename TestFixture::OwningIntList cl;
   std::vector<int> should;
   for (int i = 0; i < 21; i++) {
      int val = 10 + i;
      cl.push_back(val);
      should.push_back(val);
   }
   cl.compactLastChunk();
   std::vector<int> res;
   for (auto s : cl)
      res.push_back(s);
   std::sort(res.begin(), res.end());
   std::sort(should.begin(), should.end());

   ASSERT_EQ(should.size(), res.size());
   for (size_t i = 0; i < should.size(); i++) {
      ASSERT_EQ(should.size(), res.size());
   }
}
// ---------------------------------------------------------------------------------------------------
TYPED_TEST(factDB_infra_ListDispatcher, Merge) {
   testMergeLD<TestFixture>(true, true);
   testMergeLD<TestFixture>(true, false);
   testMergeLD<TestFixture>(false, false);
   testMergeLD<TestFixture>(false, true);
}
// ---------------------------------------------------------------------------------------------------
TYPED_TEST(factDB_infra_ListDispatcher, CopyOperator) {
   std::vector<int> vals = {1, 2, 3};
   std::vector<int> actually;
   typename TestFixture::OwningIntList list1;
   for (const auto& i : vals) {
      list1.push_back(i);
   }
   typename TestFixture::NonOwningIntList list2;
   list2 << list1;

   for (auto& val : list1) {
      actually.push_back(val);
   }
   size_t idx = 0;
   for (auto& val : list2) {
      ASSERT_EQ(actually[idx++], val);
   }

   std::sort(vals.begin(), vals.end());
   std::sort(actually.begin(), actually.end());

   ASSERT_EQ(vals.size(), actually.size());
   for (idx = 0; idx < vals.size(); idx++) {
      ASSERT_EQ(vals[idx], actually[idx]);
   }
}
// ---------------------------------------------------------------------------------------------------
