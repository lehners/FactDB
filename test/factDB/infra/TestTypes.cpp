#include "factDB/infra/Types.hpp"
#include "gtest/gtest.h"
#include <string>

using namespace factDB;
using namespace std;

TEST(factDB_infra_Types, fromInt) {
   ASSERT_EQ(Integer(123).value, 123);
   ASSERT_EQ(Integer(0).value, 0);
   ASSERT_EQ(Integer(-1).value, -1);
}

TEST(factDB_infra_Types, fromString) {
   auto checkValue = [](const std::string& s, int should) {
      auto i = Integer::castString(s.c_str(), s.size());
      ASSERT_EQ(i, Integer(should));
   };
   auto checkThrow = [](const std::string& s) {
      ASSERT_ANY_THROW(Integer::castString(s.c_str(), s.size()));
   };

   checkValue("0", 0);
   checkValue("123", 123);
   checkValue("+123", 123);
   checkValue("-456", -456);
   checkValue("-456.234", -456);

   checkThrow("12345678901");
   checkThrow("1b1");
   checkThrow("");
}

TEST(factDB_infra_Types, stream) {
   auto checkValue = [](int in, const std::string& should) {
      std::stringstream ss;
      ss << Integer(in);
      ASSERT_EQ(ss.str(), should);
   };

   checkValue(0, "0");
   checkValue(123, "123");
   checkValue(-456, "-456");
}

TEST(factDB_infra_Types, calc) {
   ASSERT_EQ(Integer(123) + Integer(123), Integer(246));
   ASSERT_EQ(Integer(123) + Integer(0), Integer(123));
   ASSERT_EQ(Integer(123) + Integer(-1), Integer(122));

   ASSERT_EQ(Integer(123) - Integer(123), Integer(0));
   ASSERT_EQ(Integer(123) - Integer(124), Integer(-1));
   ASSERT_EQ(Integer(123) - Integer(0), Integer(123));
   ASSERT_EQ(Integer(123) - Integer(-1), Integer(124));

   ASSERT_EQ(Integer(123) * Integer(10), Integer(1230));
   ASSERT_EQ(Integer(123) * Integer(1), Integer(123));
   ASSERT_EQ(Integer(123) * Integer(-1), Integer(-123));
   ASSERT_EQ(Integer(-123) * Integer(1), Integer(-123));
   ASSERT_EQ(Integer(-123) * Integer(-1), Integer(123));
}

TEST(factDB_infra_Types, compare) {
   ASSERT_FALSE(Integer(1) == Integer(0));
   ASSERT_FALSE(Integer(1) == Integer(-1));

   ASSERT_FALSE(Integer(1) < Integer(-1));
   ASSERT_FALSE(Integer(1) <= Integer(-1));

   ASSERT_FALSE(Integer(1) < Integer(1));
   ASSERT_TRUE(Integer(1) <= Integer(1));
   ASSERT_FALSE(Integer(1) > Integer(1));
   ASSERT_TRUE(Integer(1) >= Integer(1));
   ASSERT_TRUE(Integer(1) == Integer(1));
}

TEST(factDB_infra_Types_UInt64, fromInt) {
   ASSERT_EQ(UInt64(123).value, 123);
   ASSERT_EQ(UInt64(0).value, 0);
}

TEST(factDB_infra_Types_UInt64, fromString) {
   auto checkValue = [](const std::string& s, int should) {
      auto i = UInt64::castString(s.c_str(), s.size());
      ASSERT_EQ(i, UInt64(should));
   };
   auto checkThrow = [](const std::string& s) {
      ASSERT_ANY_THROW(UInt64::castString(s.c_str(), s.size()));
   };

   checkValue("0", 0);
   checkValue("123", 123);
   checkValue("123", 123);
   checkValue("456.234", 456);

   checkThrow("-456");
   checkThrow("123456789012345678901");
   checkThrow("1b1");
   checkThrow("");
}

TEST(factDB_infra_Types_UInt64, stream) {
   auto checkValue = [](int in, const std::string& should) {
      std::stringstream ss;
      ss << UInt64(in);
      ASSERT_EQ(ss.str(), should);
   };

   checkValue(0, "0");
   checkValue(123, "123");
   checkValue(456, "456");
}

TEST(factDB_infra_Types_UInt64, calc) {
   ASSERT_EQ(UInt64(123) + UInt64(123), UInt64(246));
   ASSERT_EQ(UInt64(123) + UInt64(0), UInt64(123));

   ASSERT_EQ(UInt64(123) - UInt64(123), UInt64(0));
   ASSERT_EQ(UInt64(123) - UInt64(124), UInt64(-1));
   ASSERT_EQ(UInt64(123) - UInt64(0), UInt64(123));

   ASSERT_EQ(UInt64(123) * UInt64(10), UInt64(1230));
   ASSERT_EQ(UInt64(123) * UInt64(1), UInt64(123));
}

TEST(factDB_infra_Types_UInt64, compare) {
   ASSERT_FALSE(UInt64(1) == UInt64(0));
   ASSERT_FALSE(UInt64(10) == UInt64(9));

   ASSERT_FALSE(UInt64(11) < UInt64(9));
   ASSERT_FALSE(UInt64(11) <= UInt64(9));

   ASSERT_FALSE(UInt64(1) < UInt64(1));
   ASSERT_TRUE(UInt64(1) <= UInt64(1));
   ASSERT_FALSE(UInt64(1) > UInt64(1));
   ASSERT_TRUE(UInt64(1) >= UInt64(1));
   ASSERT_TRUE(UInt64(1) == UInt64(1));
}

TEST(factDB_infra_Types_Timestamp, fromString) {
   auto checkConversion = [](std::string_view timestamp) {
      auto t = Timestamp::castString(timestamp);
      std::stringstream ss;
      ss << t;
      return ss.str() == timestamp;
   };

   ASSERT_TRUE(checkConversion("2012-01-26 12:26:41.020"));
   ASSERT_TRUE(checkConversion("1234-01-02 3:04:05.789"));
   ASSERT_TRUE(checkConversion("2012-01-26 12:26:41"));
   ASSERT_ANY_THROW(checkConversion("2012-01-26 12:26"));
   ASSERT_ANY_THROW(checkConversion("2012-01-26 12:"));
   ASSERT_ANY_THROW(checkConversion("2012-01-26 12"));
   ASSERT_ANY_THROW(checkConversion("2012-01-26 "));
   ASSERT_ANY_THROW(checkConversion("2012-01-26"));
   ASSERT_ANY_THROW(checkConversion("2012-01-2"));
   ASSERT_ANY_THROW(checkConversion("2012-01"));
   ASSERT_ANY_THROW(checkConversion("2012-"));
   ASSERT_ANY_THROW(checkConversion("2012"));

   ASSERT_ANY_THROW(checkConversion("2012-01-32 12:26:41"));
   ASSERT_ANY_THROW(checkConversion("2012-13-32 12:26:41"));
   ASSERT_ANY_THROW(checkConversion("2012-13-32 25:26:41"));
   ASSERT_ANY_THROW(checkConversion("2012-13-32 25:60:41"));
   ASSERT_ANY_THROW(checkConversion("2012-13-32 25:59:61"));
}

inline string_view trim(std::string_view in) {
   auto left = in.begin();
   for (;; ++left) {
      if (left == in.end())
         return {};
      if (!isspace(*left))
         break;
   }
   auto right = in.end() - 1;
   for (; right > left && isspace(*right); --right)
      ;
   return {left, static_cast<size_t>(std::distance(left, right) + 1)};
}

TEST(factDB_infra_Types_Date, fromString) {
   auto checkConversion = [](std::string_view timestamp) {
      auto t = Date::castString(timestamp);
      std::stringstream ss;
      ss << t;
      return ss.str() == trim(timestamp);
   };

   ASSERT_TRUE(checkConversion("2012-01-26"));
   ASSERT_TRUE(checkConversion("1234-01-02"));
   ASSERT_TRUE(checkConversion(" 2012-01-26 "));
   ASSERT_ANY_THROW(checkConversion("2012-01-2"));
   ASSERT_ANY_THROW(checkConversion("2012-01"));
   ASSERT_ANY_THROW(checkConversion("2012-"));
   ASSERT_ANY_THROW(checkConversion("2012"));

   ASSERT_ANY_THROW(checkConversion("2012-01-32 12:26:41"));
   ASSERT_ANY_THROW(checkConversion("2012-13-32 12:26:41"));
   ASSERT_ANY_THROW(checkConversion("2012-13-32 25:26:41"));
   ASSERT_ANY_THROW(checkConversion("2012-13-32 25:60:41"));
   ASSERT_ANY_THROW(checkConversion("2012-13-32 25:59:61"));
}

TEST(factDB_infra_Types_Bool, Simple) {
   Bool t1(true), t2(true), f1(false), f2(false);
   ASSERT_EQ(t1, t2);
   ASSERT_EQ(f1, f2);
   ASSERT_NE(t1, f1);

   ASSERT_TRUE(t1.toBool());
   ASSERT_FALSE(f1.toBool());

   ASSERT_EQ(Bool::castString("true"sv), Bool(true));
   ASSERT_EQ(Bool::castString("t"sv), Bool(true));
   ASSERT_EQ(Bool::castString("T"sv), Bool(true));
   ASSERT_EQ(Bool::castString("1"sv), Bool(true));

   ASSERT_EQ(Bool::castString("false"sv), Bool(false));
   ASSERT_EQ(Bool::castString("f"sv), Bool(false));
   ASSERT_EQ(Bool::castString("F"sv), Bool(false));
   ASSERT_EQ(Bool::castString("0"sv), Bool(false));

   ASSERT_EQ(Bool(true) && Bool(true), Bool(true));
   ASSERT_EQ(Bool(true) && Bool(false), Bool(false));
   ASSERT_EQ(Bool(false) && Bool(true), Bool(false));
   ASSERT_EQ(Bool(false) && Bool(false), Bool(false));

   ASSERT_EQ(Bool(true) || Bool(true), Bool(true));
   ASSERT_EQ(Bool(true) || Bool(false), Bool(true));
   ASSERT_EQ(Bool(false) || Bool(true), Bool(true));
   ASSERT_EQ(Bool(false) || Bool(false), Bool(false));

   {
      Bool t(true);
      t &= Bool(true);
      ASSERT_TRUE(t.toBool());
      t &= Bool(false);
      ASSERT_FALSE(t.toBool());
      t &= Bool(false);
      ASSERT_FALSE(t.toBool());
   }
   {
      Bool b(false);
      b &= Bool(true);
      ASSERT_FALSE(b.toBool());
   }

   {
      Bool b(false);
      b |= Bool(false);
      ASSERT_FALSE(b.toBool());
      b |= Bool(true);
      ASSERT_TRUE(b.toBool());
      b |= Bool(true);
      ASSERT_TRUE(b.toBool());
   }
   {
      Bool b(true);
      b |= Bool(false);
      ASSERT_TRUE(b.toBool());
   }
}