#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "gtest/gtest.h"

using namespace factDB;

TEST(factDB_opt, EquivalenceClasses) {
   Database db = DatabaseLoadUtil::genTestDB();
   auto r1 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r1");
   auto r2 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r2");
   auto r3 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r3");
   auto r4 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r4");
   auto r5 = std::make_unique<algebra::TableScan>(db, testdb::KRelationTest, "r5");

   auto r1IUs = r1->collectIUs();
   auto r2IUs = r2->collectIUs();
   auto r3IUs = r3->collectIUs();
   auto r4IUs = r4->collectIUs();
   auto r5IUs = r5->collectIUs();

   auto jc1 = JoinCondition::create(r1IUs[0], r2IUs[0]);
   auto jc2 = JoinCondition::create(r1IUs[1], r4IUs[0]);
   auto jc3 = JoinCondition::create(r1IUs[1], r2IUs[0]);
   auto jc4 = JoinCondition::create(r2IUs[1], r3IUs[0]);
   auto jc5 = JoinCondition::create(r3IUs[1], r4IUs[1]);
   auto jc6 = JoinCondition::create(r4IUs[1], r5IUs[0]);

   auto jcl12 = JoinConditionList::create({std::move(jc1), std::move(jc3)});
   auto jcl123 = JoinConditionList::create({std::move(jc4)});
   auto jcl1234 = JoinConditionList::create({std::move(jc2), std::move(jc5)});
   auto jcl12345 = JoinConditionList::create({std::move(jc6)});

   auto j1 = std::make_unique<algebra::InnerJoin>(std::move(r1), std::move(r2), std::move(jcl12));
   auto j2 = std::make_unique<algebra::InnerJoin>(std::move(j1), std::move(r3), std::move(jcl123));
   auto j3 = std::make_unique<algebra::InnerJoin>(std::move(j2), std::move(r4), std::move(jcl1234));
   auto j4 = std::make_unique<algebra::InnerJoin>(std::move(j3), std::move(r5), std::move(jcl12345));

   auto allIUs = j4->collectIUs();
   std::unique_ptr<algebra::Operator> tree = std::make_unique<algebra::Count>(std::move(j4));

   std::vector<std::vector<const IU*>> should{
      {r1IUs[0], r1IUs[1], r4IUs[0], r2IUs[0]},
      {r2IUs[1], r3IUs[0]},
      {r3IUs[1], r4IUs[1], r5IUs[0]},
      {r5IUs[1]}};

   auto queryGraph = opt::QueryGraph::generateQueryGraph(db, *tree);

   std::unordered_map<const IU*, unsigned> iu2class;
   std::vector<unsigned> elementsPerClass(queryGraph.maxEquivalenceClasses(), 0);
   for (const IU* iu : tree->collectIUs()) {
      unsigned equivalenceClass = queryGraph.getEquivalenceClass(*iu);
      ASSERT_LT(equivalenceClass, elementsPerClass.size());
      iu2class[iu] = equivalenceClass;
      ++elementsPerClass[equivalenceClass];
   }

   ASSERT_EQ(iu2class.size(), 10);
   for (auto& equivalenceClass : should) {
      ASSERT_FALSE(equivalenceClass.empty());
      unsigned firstElementsClass = iu2class[equivalenceClass.front()];
      ASSERT_EQ(equivalenceClass.size(), elementsPerClass[firstElementsClass]);

      for (size_t innerIdx = 1; innerIdx < equivalenceClass.size(); ++innerIdx) {
         ASSERT_TRUE(iu2class.contains(equivalenceClass[innerIdx]));
         ASSERT_EQ(firstElementsClass, iu2class[equivalenceClass[innerIdx]]);
      }
   }
}
