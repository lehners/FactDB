#ifndef H_FACTDB_TEST_FACT_DB_UTIL_TRANSLATIONTEST_HPP
#define H_FACTDB_TEST_FACT_DB_UTIL_TRANSLATIONTEST_HPP

#include "factDB/Database.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Operator.hpp"
#include <functional>
#include <iostream>
#include <sstream>
#include <vector>

namespace factDB::test {

/// tests a generated code snippet
std::string testGeneratedCode(factDB::Database& db, std::unique_ptr<factDB::algebra::Operator> printer, const factDB::OrderedIUSet& required, bool sortLines = true);
/// serializes and deserialized to json
std::unique_ptr<factDB::algebra::Operator> serializeAndDeserialize(const Database& db, std::unique_ptr<factDB::algebra::Operator>& printer, const factDB::OrderedIUSet& required);

namespace {
template <class TupType, size_t... I>
void print(std::stringstream& ss, const TupType& tup, std::index_sequence<I...>) {
   (..., (ss << (I == 0 ? "" : "|") << std::get<I>(tup)));
}

template <class... T>
void print(std::stringstream& ss, const std::tuple<T...>& tup) {
   print(ss, tup, std::make_index_sequence<sizeof...(T)>());
}
} // anonymous namespace

template <class... T>
std::string genTupleString(std::vector<std::tuple<T...>> tuples) {
   std::sort(tuples.begin(), tuples.end());
   std::stringstream ss;
   for (auto& t : tuples) {
      print(ss, t);
      ss << "\n";
   }
   return ss.str();
}

namespace defaultQueryTrees {

struct TestCase {
   using TreeStructure = std::pair<std::unique_ptr<factDB::algebra::Operator>, factDB::OrderedIUSet>;

   protected:
   [[nodiscard]] virtual TreeStructure getInnerTree(const Database& db) const = 0;

   public:
   [[nodiscard]] virtual TreeStructure getTreePrint(const Database& db) const;
   [[nodiscard]] virtual TreeStructure getTreeCount(const Database& db) const;
   [[nodiscard]] virtual std::string resultPrint() const = 0;
   [[nodiscard]] virtual std::string resultCount() const;

   static void performTest(const TestCase& tc, const std::function<void(const TestCase& tc)>& fun) { fun(tc); }
};

struct TestSimpleScan : public TestCase {
   [[nodiscard]] TreeStructure getInnerTree(const Database& db) const override;
   [[nodiscard]] std::string resultPrint() const override;
};

struct TestReorderColumns : public TestCase {
   [[nodiscard]] TreeStructure getInnerTree(const Database& db) const override;
   [[nodiscard]] std::string resultPrint() const override;
};

struct TestSimpleFilter : public TestCase {
   [[nodiscard]] TreeStructure getInnerTree(const Database& db) const override;
   [[nodiscard]] std::string resultPrint() const override;
};

struct TestMultipleFilterPredicates : public TestCase {
   [[nodiscard]] TreeStructure getInnerTree(const Database& db) const override;
   [[nodiscard]] std::string resultPrint() const override;
};

struct TestSimpleJoin : public TestCase {
   factDB::algebra::JoinMode joinMode;
   TestSimpleJoin(algebra::JoinMode mode = algebra::JoinMode::TopInsert) : joinMode(mode) {}
   [[nodiscard]] TreeStructure getInnerTree(const Database& db) const override;
   [[nodiscard]] std::string resultPrint() const override;
};

struct TestSimpleRightSemiJoin : public TestCase {
   factDB::algebra::JoinMode joinMode;
   TestSimpleRightSemiJoin(algebra::JoinMode mode = algebra::JoinMode::TopInsert) : joinMode(mode) {}
   [[nodiscard]] TreeStructure getInnerTree(const Database& db) const override;
   [[nodiscard]] std::string resultPrint() const override;
};

struct TestTwoKeyJoin : public TestCase {
   factDB::algebra::JoinMode joinMode;
   TestTwoKeyJoin(algebra::JoinMode mode = algebra::JoinMode::TopInsert) : joinMode(mode) {}
   [[nodiscard]] TreeStructure getInnerTree(const Database& db) const override;
   [[nodiscard]] std::string resultPrint() const override;
};

struct TestTwoHopJoinLeftDeep : public TestCase {
   factDB::algebra::JoinMode joinMode1, joinMode2;
   TestTwoHopJoinLeftDeep(algebra::JoinMode mode1 = algebra::JoinMode::TopInsert, algebra::JoinMode mode2 = algebra::JoinMode::TopInsert) : joinMode1(mode1), joinMode2(mode2) {}

   [[nodiscard]] TreeStructure getInnerTree(const Database& db) const override;
   [[nodiscard]] std::string resultPrint() const override;
};

struct TestTwoHopJoinRightDeep : public TestCase {
   factDB::algebra::JoinMode joinMode1, joinMode2;
   TestTwoHopJoinRightDeep(algebra::JoinMode mode1 = algebra::JoinMode::TopInsert, algebra::JoinMode mode2 = algebra::JoinMode::TopInsert) : joinMode1(mode1), joinMode2(mode2) {}

   [[nodiscard]] TreeStructure getInnerTree(const Database& db) const override;
   [[nodiscard]] std::string resultPrint() const override;
};

struct TestThreeHopJoinBushy : public TestCase {
   factDB::algebra::JoinMode joinMode1, joinMode2, joinMode3;
   TestThreeHopJoinBushy(algebra::JoinMode mode1, algebra::JoinMode mode2, algebra::JoinMode mode3) : joinMode1(mode1), joinMode2(mode2), joinMode3(mode3) {}

   [[nodiscard]] TreeStructure getInnerTree(const Database& db) const override;
   [[nodiscard]] std::string resultPrint() const override;
};

struct TestTwoHopSemiJoin : public TestCase {
   factDB::algebra::JoinMode joinMode1, joinMode2;
   TestTwoHopSemiJoin(algebra::JoinMode mode1 = algebra::JoinMode::TopInsert, algebra::JoinMode mode2 = algebra::JoinMode::TopInsert) : joinMode1(mode1), joinMode2(mode2) {}

   [[nodiscard]] TreeStructure getInnerTree(const Database& db) const override;
   [[nodiscard]] std::string resultPrint() const override;
};

struct TestThreeHopJoin : public TestCase {
   factDB::algebra::JoinMode joinMode1, joinMode2, joinMode3;
   TestThreeHopJoin(algebra::JoinMode mode1 = algebra::JoinMode::TopInsert, algebra::JoinMode mode2 = algebra::JoinMode::TopInsert, algebra::JoinMode mode3 = algebra::JoinMode::TopInsert) : joinMode1(mode1), joinMode2(mode2), joinMode3(mode3) {}

   [[nodiscard]] TreeStructure getInnerTree(const Database& db) const override;
   [[nodiscard]] std::string resultPrint() const override;
};

struct TopInsertMultipleRequired : public TestCase {
   TopInsertMultipleRequired() = default;

   [[nodiscard]] TreeStructure getInnerTree(const Database& db) const override;
   [[nodiscard]] std::string resultPrint() const override;
};

struct NotTopmostTopInsert : public TestCase {
   NotTopmostTopInsert() = default;

   [[nodiscard]] TreeStructure getInnerTree(const Database& db) const override;
   [[nodiscard]] std::string resultPrint() const override;
};

} // namespace defaultQueryTrees

} // namespace factDB::test

#endif //  H_FACTDB_TEST_FACT_DB_UTIL_TRANSLATIONTEST_HPP