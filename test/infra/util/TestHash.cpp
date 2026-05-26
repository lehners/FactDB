#include "factDB/infra/util/Hash.hpp"
#include <gtest/gtest.h>

namespace factDB {

// Demonstrate some basic assertions.
TEST(Hash, Hash) {
   const char text[] = "0123456789012345678901234567890123456789012345678901234567890012345678901234567890123456789012345678901234567890123456789012"; // NOLINT
   ASSERT_EQ(Hash::hash(text, 122, 0), 0xd74bb00ee64ee6c9);
}

} // namespace factDB