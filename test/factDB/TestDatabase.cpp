#include "factDB/Database.hpp"
#include "factDB/gen/tables/table_KRelationTest.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/schemac/BaseTable.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "gtest/gtest.h"
// ---------------------------------------------------------------------------------------------------
namespace factDB::test {
// ---------------------------------------------------------------------------------------------------
TEST(factDB_Database, LoadExample) {
   auto db = DatabaseLoadUtil::genTestDB();

   ASSERT_EQ(db.getTable(testdb::KRelationTest).size(), 5);
   auto& iter = static_cast<tables::KRelationTest&>(db.getTable(tables::KRelationTest{}.name()));
   ASSERT_EQ(iter.size(), 5);

   auto checkValue = [&](size_t idx, int a, int b) { // NOLINT
      ASSERT_EQ(iter.data[idx].a(), Integer(a));
      ASSERT_EQ(iter.data[idx].b(), Integer(b));
   };
   checkValue(0, 1, 2);
   checkValue(1, 1, 2);
   checkValue(2, 2, 3);
   checkValue(3, 2, 4);
   checkValue(4, 3, 5);
}
// ---------------------------------------------------------------------------------------------------
TEST(factDB_Database, GetIUs) {
   factDB::Database db = DatabaseLoadUtil::genTestDB();
   auto columns = db.getColumns(db.getRelationID(testdb::KRelationTest), "");

   ASSERT_EQ(columns.size(), 2);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::test