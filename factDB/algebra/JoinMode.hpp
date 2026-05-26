#ifndef H_FACTDB_FACTDB_ALGEBRA_INNERJOIN_CPP_JOINMODE_HPP
#define H_FACTDB_FACTDB_ALGEBRA_INNERJOIN_CPP_JOINMODE_HPP
// ---------------------------------------------------------------------------------------------------
#include <cstdint>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
enum class JoinMode : std::uint8_t { TopInsert = 0,
                                     BottomInsert = 1
};
// ---------------------------------------------------------------------------------------------------
enum class JoinType : std::uint8_t { Inner = 0,
                                     RightSemi = 1
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_ALGEBRA_INNERJOIN_CPP_JOINMODE_HPP
