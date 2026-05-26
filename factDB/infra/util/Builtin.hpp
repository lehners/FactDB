#ifndef H_infra_util_Builtin
#define H_infra_util_Builtin
// ---------------------------------------------------------------------------------------------------
#include "gtest/gtest.h"
#include <unordered_set>
// ---------------------------------------------------------------------------------------------------
namespace factDB::assert {
// ---------------------------------------------------------------------------------------------------
[[maybe_unused]] inline void assert_true(bool b) {
   EXPECT_TRUE(b);
   if (!b) throw std::runtime_error("values are not equal");
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
[[maybe_unused]] inline void assert_eq(T a, T b) {
   EXPECT_EQ(a, b);
   if (a != b) throw std::runtime_error("values are not equal");
}
// ---------------------------------------------------------------------------------------------------
[[maybe_unused]] inline void assert_eq(int a, int b) {
   EXPECT_EQ(a, b);
   if (a != b) throw std::runtime_error("values are not equal");
}
// ---------------------------------------------------------------------------------------------------
template <typename tuple_type>
[[maybe_unused]] inline void assert_exists(const std::vector<tuple_type>& vec, tuple_type tuple) {
   auto found = std::find(vec.begin(), vec.end(), tuple) != vec.end();
   EXPECT_TRUE(found);
   if (!found) throw std::runtime_error("values was not found");
}
// ---------------------------------------------------------------------------------------------------
template <typename tuple_type>
[[maybe_unused]] inline void assert_not_exists(const std::vector<tuple_type>& vec, tuple_type tuple) {
   auto found = std::find(vec.begin(), vec.end(), tuple) != vec.end();
   EXPECT_FALSE(found);
   if (found) throw std::runtime_error("values was found");
}
// ---------------------------------------------------------------------------------------------------
template <typename tuple_type, typename hasher>
[[maybe_unused]] inline void assert_exists(const std::unordered_set<tuple_type, hasher>& us, tuple_type tuple) {
   auto found = us.contains(tuple);
   EXPECT_TRUE(found);
   if (!found) throw std::runtime_error("values was not found");
}
// ---------------------------------------------------------------------------------------------------
template <typename tuple_type, typename hasher>
[[maybe_unused]] inline void assert_not_exists(const std::unordered_set<tuple_type, hasher>& us, tuple_type tuple) {
   auto found = us.contains(tuple);
   EXPECT_FALSE(found);
   if (found) throw std::runtime_error("values was found");
}
// ---------------------------------------------------------------------------------------------------
template <typename T>
[[maybe_unused]] inline void remove_duplicates(std::vector<T>& vec) {
   sort(vec.begin(), vec.end());
   vec.erase(unique(vec.begin(), vec.end()), vec.end());
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::assert
// ---------------------------------------------------------------------------------------------------
#endif // H_infra_util_Builtin
