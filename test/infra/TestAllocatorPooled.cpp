#include "factDB/infra/alloc/AllocatorPooled.hpp"
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <utility>
#include <gtest/gtest.h>
// ---------------------------------------------------------------------------------------------------
using namespace std;
// ---------------------------------------------------------------------------------------------------
namespace factDB::pool::test {
// ---------------------------------------------------------------------------------------------------
namespace impl {
struct AllocatorTestType {};
}; // namespace impl
// ---------------------------------------------------------------------------------------------------
/// Concept for an allocator
template <typename Allocator>
concept IsAllocator = requires(Allocator& a) {
   typename Allocator::value_type;
   typename Allocator::template rebind<typename Allocator::value_type>;
   typename Allocator::template rebind<impl::AllocatorTestType>;
   requires std::same_as<typename Allocator::template rebind<typename Allocator::value_type>, Allocator>;
   requires !std::same_as<typename Allocator::template rebind<impl::AllocatorTestType>, Allocator>;
   requires std::default_initializable<Allocator>;
   requires std::movable<Allocator>;
   { a.allocate() } -> std::same_as<typename Allocator::value_type*>;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::pool::test
// ---------------------------------------------------------------------------------------------------
using namespace factDB::pool;
// ---------------------------------------------------------------------------------------------------
TEST(TestPooledAllocator, Concept) {
   struct Foo {
      Foo(const Foo&) = delete;
      Foo(Foo&&) = delete;
      Foo& operator=(const Foo&) = delete;
      Foo& operator=(Foo&&) = delete;
   };
   // You can change EXPECT_TRUE to static_assert in the following lines to
   // get more detailed error messages that explain why the test fails.
   static_assert(test::IsAllocator<PooledAllocator<int>>);
   static_assert(test::IsAllocator<PooledAllocator<Foo>>);
}
// ---------------------------------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------------------------------
template <typename T>
class TestPooledAllocatorT : public ::testing::Test {
   public:
   T makeValue(unsigned char b) {
      T value;
      for (size_t i = 0; i < sizeof(T); ++i)
         reinterpret_cast<char*>(&value)[i] = b; // NOLINT
      return value;
   }

   void writeValue(T& value, unsigned char b) {
      value = makeValue(b);
   }

   void expectValue(const T& value, unsigned char b) {
      auto expectedValue = makeValue(b);
      EXPECT_EQ(value, expectedValue);
   }
};
// ---------------------------------------------------------------------------------------------------
struct LargeAlignType {
   alignas(std::max_align_t) std::array<unsigned char, sizeof(std::max_align_t)> data;

   bool operator==(const LargeAlignType&) const = default;
};
// ---------------------------------------------------------------------------------------------------
using AllocatorTestTypes = ::testing::Types<int, LargeAlignType, std::array<unsigned char, 1000>>;
// ---------------------------------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------------------------------
TYPED_TEST_SUITE(TestPooledAllocatorT, AllocatorTestTypes);
// ---------------------------------------------------------------------------------------------------
TYPED_TEST(TestPooledAllocatorT, Allocate) {
   PooledAllocator<TypeParam> a;
   auto* ptr1 = a.allocate();
   ASSERT_EQ(reinterpret_cast<uintptr_t>(ptr1) % alignof(TypeParam), 0);
   this->writeValue(*ptr1, 42);

   auto* ptr2 = a.allocate();
   ASSERT_EQ(reinterpret_cast<uintptr_t>(ptr2) % alignof(TypeParam), 0);
   auto* ptr3 = a.allocate();
   ASSERT_EQ(reinterpret_cast<uintptr_t>(ptr3) % alignof(TypeParam), 0);

   ASSERT_NE(ptr1, ptr2);
   ASSERT_NE(ptr1, ptr3);
   ASSERT_NE(ptr2, ptr3);

   this->writeValue(*ptr2, 13);
   this->writeValue(*ptr3, 37);

   this->expectValue(*ptr1, 42);
   this->expectValue(*ptr2, 13);
   this->expectValue(*ptr3, 37);
}
// ---------------------------------------------------------------------------------------------------
TYPED_TEST(TestPooledAllocatorT, AllocateMany) {
   PooledAllocator<TypeParam> a;
   unordered_set<TypeParam*> pointers;
   for (unsigned i = 0; i < 1000; ++i) {
      auto* p = a.allocate();
      this->writeValue(*p, i);
      ASSERT_EQ(pointers.count(p), 0);
      pointers.insert(p);
   }
}
// ---------------------------------------------------------------------------------------------------
TYPED_TEST(TestPooledAllocatorT, Move) {
   PooledAllocator<TypeParam> a;
   for (unsigned i = 0; i < 1000; ++i) {
      a.allocate();
   }
   PooledAllocator<TypeParam> b(std::move(a));
   PooledAllocator<TypeParam> c;
   c = std::move(b);
}
// ---------------------------------------------------------------------------------------------------
TYPED_TEST(TestPooledAllocatorT, Iterator) {
   PooledAllocator<TypeParam> a;
   ASSERT_EQ(a.begin(), a.end());

   auto* ptr1 = a.allocate();
   ASSERT_EQ(reinterpret_cast<uintptr_t>(ptr1) % alignof(TypeParam), 0);
   this->writeValue(*ptr1, 42);

   auto* ptr2 = a.allocate();
   ASSERT_EQ(reinterpret_cast<uintptr_t>(ptr2) % alignof(TypeParam), 0);
   auto* ptr3 = a.allocate();
   ASSERT_EQ(reinterpret_cast<uintptr_t>(ptr3) % alignof(TypeParam), 0);

   ASSERT_NE(ptr1, ptr2);
   ASSERT_NE(ptr1, ptr3);
   ASSERT_NE(ptr2, ptr3);

   this->writeValue(*ptr2, 13);
   this->writeValue(*ptr3, 37);

   auto iter = a.begin();
   ASSERT_NE(iter, a.end());
   this->expectValue(*iter, 42);

   ASSERT_NE(++iter, a.end());
   this->expectValue(*iter, 13);

   ASSERT_NE(++iter, a.end());
   this->expectValue(*iter, 37);

   ASSERT_EQ(++iter, a.end());
}
