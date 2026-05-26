#include "factDB/algebra/translator/FactorizedTableTranslator.hpp"
#include "factDB/queryc/NewFileWriter.hpp"
#include "factDB/util/DatabaseLoadUtil.hpp"
#include "factDB/util/QueryFile.hpp"
#include "gtest/gtest.h"
#include <unordered_set>

namespace {

using namespace factDB;
using FactorizedTableNode = factDB::algebra::translator::FactorizedTableNode;
using IU = factDB::IU;
using Type = factDB::schemac::Type;

void test_iterator(const std::function<void(FileWriter&)>& query) {
   auto db = DatabaseLoadUtil::genTestDB();
   factDB::util::generateCodeQuery(
      db, [&query](FileWriter& out) {
         for (const auto& h : FactorizedTableNode::get_required_headers())
            out.addInclude(h);

         out.addInclude("factDB/infra/util/Builtin.hpp")
               .addInclude("factDB/infra/util/Hash.hpp")
               .addInclude("unordered_set")
               .addInclude("factDB/query.hpp")
               .addInclude("factDB/queryc/PerformanceRecord.hpp")
            << fw::endl()
            << "namespace factDB::generated {" << fw::endl();
         out.addFunctionHeader("void query(factDB::Database &database, std::ostream& out_stream, queryc::PerformanceRecorderExecution recorder)")
            << "using namespace factDB::infra;" << fw::endl()
            << "using namespace factDB::assert;" << fw::endl();
         query(out);
         out << "  }" << fw::endl()
             << "} // namespace factDB::generated" << fw::endl();
      });
}

TEST(factDB_algebra_translator_FactorizedTableNode, Simple) {
   FactorizedTableNode n;
   n.require_size();
   auto a = std::make_unique<IU>("a", Type::Integer());
   auto b = std::make_unique<IU>("b", Type::Integer());
   auto c1 = std::make_unique<IU>("c1", Type::Integer());
   auto c2 = std::make_unique<IU>("c2", Type::Integer());
   auto d = std::make_unique<IU>("d", Type::Integer());
   auto e = std::make_unique<IU>("e", Type::Integer());
   auto f = std::make_unique<IU>("f", Type::Integer());

   // a -- c1, c2 -- e -- f
   // |       |
   // b       d
   n.add_iu(*a);
   n.addChild(*b);
   auto& cn = n.addChild(*c1);
   cn.add_iu(*c2);
   cn.addChild(*d);
   cn.addChild(*e).addChild(*f);
   n.setIteratorIdxs();
   n.set_dereference_order(*a, *b, *c1, *c2, *d, *e, *f);

   ASSERT_NO_THROW(test_iterator([&](FileWriter& out) {
      n.genStruct(out);

      auto push_back1 = [&](const std::string& handle, const IU& iu, int val, size_t size) { return fw::lc(n.push_back("ft", handle, iu, val), fw::fmt(" assert_eq(ft.size(), {});", size), fw::endl()); };
      // std::move(n.push_back("ft", handle, iu, val) + " assert_eq(ft.size(), " + size + ");" + fw::endl()); };
      auto push_back2 = [&](const std::string& handle, const IU& iu, int val1, int val2, size_t size) { return fw::lc(n.push_back("ft", handle, iu, val1, val2), fw::fmt(" assert_eq(ft.size(), {});", size), fw::endl()); };
      //std::move(n.push_back("ft", handle, iu, val1, val2) + " assert_eq(ft.size(), " + size + ");" + fw::endl()); };

      out << "FileWriter fw_out(std::cout, FileWriter::Silent);" << fw::endl()
          << n.class_name << " ft;" << fw::endl()
          << "auto& h1 = " << push_back1("", *a, 1, 0)
          << push_back1("h1", *b, 11, 0)
          << "auto& h12 = " << push_back2("h1", *c1, 12, 12, 0)
          << push_back1("h12", *d, 121, 0)
          << "auto& h122 = " << push_back1("h12", *e, 122, 0)
          << push_back1("h122", *f, 1221, 1)
          << push_back1("h122", *f, 1222, 2)
          << fw::endl()
          << "auto& h2 = " << push_back1("", *a, 2, 2)
          << push_back1("h2", *b, 211, 2)
          << push_back1("h2", *b, 212, 2)
          << "auto& h22 = " << push_back2("h2", *c1, 22, 12, 2)
          << push_back1("h22", *d, 221, 2)
          << "auto& h222 = " << push_back1("h22", *e, 222, 2)
          << push_back1("h222", *f, 2221, 4)
          << push_back1("h222", *f, 2222, 6)
          << push_back1("h222", *f, 2223, 8)
          << fw::endl()
          << "using tupleType = std::tuple<int, int, int, int, int, int, int>;" << fw::endl()
          << "std::vector<tupleType> results({{1,11,12,12,121,122,1221},{1,11,12,12,121,122,1222}," << fw::endl()
          << "                      {2,211,22,12,221,222,2221},{2,211,22,12,221,222,2222},{2,211,22,12,221,222,2223}," << fw::endl()
          << "                      {2,212,22,12,221,222,2221},{2,212,22,12,221,222,2222},{2,212,22,12,221,222,2223}});" << fw::endl()
          << "std::vector<tupleType> seen;" << fw::endl()
          << "for (const auto& [a, b, c1, c2, d, e, f] : ft) {" << fw::endl()
          << "  auto new_tuple = std::make_tuple(a.value, b.value, c1.value, c2.value, d.value, e.value, f.value);" << fw::endl()
          << "  assert_exists(results, new_tuple);" << fw::endl()
          << "  assert_not_exists(seen, new_tuple);" << fw::endl()
          << "  seen.push_back(new_tuple);" << fw::endl()
          << "}" << fw::endl()
          << "assert_eq(results.size(), seen.size());" << fw::endl();
   }));
}

TEST(factDB_algebra_translator_FactorizedTableNode, InsertLeafMany) {
   FactorizedTableNode n;
   n.require_size();
   auto a = std::make_unique<IU>("a", Type::Integer());
   n.add_iu(*a);
   int elements = 1000;
   n.set_dereference_order(*a);

   ASSERT_NO_THROW(test_iterator([&](FileWriter& out) {
      n.genStruct(out);

      out << n.class_name << " ft;" << fw::endl()
          << "for (int i = 0; i < " << elements << "; i++) {" << fw::endl()
          << n.push_back("ft", *a, "i") << fw::endl()
          << "}" << fw::endl()
          << "assert_eq(ft.size(), " << elements << ");" << fw::endl()
          << fw::endl()
          << "size_t cur_val = 0;" << fw::endl()
          << "for (const auto& [fact_elem] : ft) {" << fw::endl()
          << "   assert_eq(fact_elem.value, cur_val++);" << fw::endl()
          << "}" << fw::endl()
          << fw::endl()
          << "assert_eq(cur_val, " << elements << ");";
   }));
}

TEST(factDB_algebra_translator_FactorizedTableNode, IterateInnerTemplated) {
   FactorizedTableNode n;
   n.require_size();
   auto a = std::make_unique<IU>("a", Type::Integer());
   auto b = std::make_unique<IU>("b", Type::Integer());
   n.add_iu(*a);
   n.addChild(*b);
   n.set_dereference_order(*a, *b);

   ASSERT_NO_THROW(test_iterator([&](FileWriter& out) {
      n.genStruct(out);
      out << n.class_name << " ft;" << fw::endl()
          << "auto& handle1 = " << n.push_back("ft", *a, 1) << "assert_eq(ft.size(), 0);" << fw::endl()
          << n.push_back("ft", "handle1", *b, 11) << "assert_eq(ft.size(), 1);" << fw::endl()
          << n.push_back("ft", "handle1", *b, 12) << "assert_eq(ft.size(), 2);" << fw::endl()
          << fw::endl()
          << "auto& handle2 = " << n.push_back("ft", *a, 2) << "assert_eq(ft.size(), 2);" << fw::endl()
          << n.push_back("ft", "handle2", *b, 21) << "assert_eq(ft.size(), 3);" << fw::endl()
          << n.push_back("ft", "handle2", *b, 22) << "assert_eq(ft.size(), 4);" << fw::endl()
          << n.push_back("ft", "handle2", *b, 23) << "assert_eq(ft.size(), 5);" << fw::endl()
          << fw::endl()
          << "int f_a = 1, f_b = 11;" << fw::endl()
          << "for (const auto& [a, b] : ft) {" << fw::endl()
          << "  assert_eq(f_a, a.value);" << fw::endl()
          << "  assert_eq(f_b++, b.value);" << fw::endl()
          << "  if (f_b == 13) { f_a = 2; f_b = 21; }" << fw::endl()
          << "}" << fw::endl()
          << "assert_eq(f_a, 2);" << fw::endl()
          << "assert_eq(f_b, 24);" << fw::endl();
   }));
}
TEST(factDB_algebra_translator_FactorizedTableNode, TwoChildTableInsertMany) {
   int max_i = 100;

   FactorizedTableNode n;
   n.require_size();
   auto a = std::make_unique<IU>("a", Type::Integer());
   auto b = std::make_unique<IU>("b", Type::Integer());
   auto c = std::make_unique<IU>("c", Type::Integer());
   n.add_iu(*a);
   n.addChild(*b);
   n.addChild(*c);
   n.set_dereference_order(*a, *b, *c);

   ASSERT_NO_THROW(test_iterator([&](FileWriter& out) {
      n.genStruct(out);
      out << n.class_name << " ft;" << fw::endl()
          << "for (int i = 1; i < " << max_i << "; i++) {" << fw::endl()
          << "  auto& handle = " << n.push_back("ft", *a, "i") << fw::endl()
          << "  for (int j = 0; j < i; j++) {" << fw::endl()
          << n.push_back("ft", "handle", *b, "+1000 * i + j")
          << n.push_back("ft", "handle", *c, "-1000 * i + j")
          << "  }" << fw::endl()
          << "}" << fw::endl()
          << fw::endl()
          << "using tupleType = std::tuple<int, int, int>;" << fw::endl()
          << "using hasher = factDB::Hash::HashTuple<int, int, int>;" << fw::endl()
          << fw::endl()
          << "std::unordered_set<tupleType, hasher> results;" << fw::endl()
          << "int expected_size = " << ((max_i - 1) * (2 * (max_i - 1) + 1) * ((max_i - 1) + 1)) / 6 << ";" << fw::endl()
          << "results.reserve(expected_size);" << fw::endl()
          << "for (int i = 1; i < " << max_i << "; i++) {" << fw::endl()
          << "  for (int j1 = 0; j1 < i; j1++) {" << fw::endl()
          << "    for (int j2 = 0; j2 < i; j2++) {" << fw::endl()
          << "      results.insert(std::make_tuple(i, 1000*i+j1, -1000*i+j2));" << fw::endl()
          << "    }" << fw::endl()
          << "  }" << fw::endl()
          << "}" << fw::endl()
          << "std::unordered_set<tupleType, hasher> seen;" << fw::endl()
          << "seen.reserve(expected_size);" << fw::endl()
          << fw::endl()
          << "assert_eq(results.size(), ft.size());" << fw::endl()
          << "assert_eq(ft.size(), expected_size);" << fw::endl()
          << "for (const auto& [a, b, c] : ft) {" << fw::endl()
          << "  auto new_tuple = std::make_tuple(a.value, b.value, c.value);" << fw::endl()
          << "  assert_exists(results, new_tuple);" << fw::endl()
          << "  assert_not_exists(seen, new_tuple);" << fw::endl()
          << "  seen.insert(new_tuple);" << fw::endl()
          << "}" << fw::endl()
          << "assert_eq(results.size(), seen.size());" << fw::endl();
   }));
}

TEST(factDB_algebra_translator_FactorizedTableNode, TwoChildTablePartialIteratorSingle) {
   FactorizedTableNode n;
   n.require_size();
   auto a = std::make_unique<IU>("a", Type::Integer());
   auto b = std::make_unique<IU>("b", Type::Integer());
   auto c = std::make_unique<IU>("c", Type::Integer());
   n.add_iu(*a);
   n.addChild(*b);
   n.addChild(*c);

   ASSERT_NO_THROW(test_iterator([&](FileWriter& out) {
      n.set_dereference_order(*a);
      auto [b_begin, b_end] = n.add_subiterator(*b);
      auto [c_begin, c_end] = n.add_subiterator(*c);
      auto [d_begin, d_end] = n.add_subiterator(*c, *a);
      n.genStruct(out);

      auto generate_tester = [&](const std::string& result, const factDB::FWContainer& begin, const factDB::FWContainer& end) {
         out << "{" << fw::endl()
             << "  std::vector<int> results(" << result << ");" << fw::endl()
             << "  std::vector<int> seen;" << fw::endl()
             << "  for (auto iter = ft." << begin << "(); iter != ft." << end << "(); ++iter) {" << fw::endl()
             << "    const auto& [v] = *iter;" << fw::endl()
             << "    std::cout << v << std::endl;" << fw::endl()
             //<< fw::cout("v")
             << "    assert_exists(results, v.value);" << fw::endl()
             << "    seen.push_back(v.value);" << fw::endl()
             << "  }" << fw::endl()
             << "  assert_true(seen.size() < ft.size());" << fw::endl()
             << "  remove_duplicates(seen);" << fw::endl()
             << "  assert_eq(results.size(), seen.size());" << fw::endl()
             << "  std::cout << std::endl;" << fw::endl() // fw::cout()
             << "}" << fw::endl()
             << fw::endl();
      };

      out << n.class_name << " ft;" << fw::endl()
          << "auto& h1 = " << n.push_back("ft", *a, "Integer(1)") << fw::endl()
          << n.push_back("ft", "h1", *b, 11) << "assert_eq(ft.size(), 0);" << fw::endl()
          << n.push_back("ft", "h1", *c, 12) << "assert_eq(ft.size(), 1);" << fw::endl()
          << fw::endl()
          << "auto& h2 = " << n.push_back("ft", *a, 2) << fw::endl()
          << n.push_back("ft", "h2", *b, 21) << "assert_eq(ft.size(), 1);" << fw::endl()
          << n.push_back("ft", "h2", *b, 22) << "assert_eq(ft.size(), 1);" << fw::endl()
          << n.push_back("ft", "h2", *c, 23) << "assert_eq(ft.size(), 3);" << fw::endl()
          << n.push_back("ft", "h2", *c, 24) << "assert_eq(ft.size(), 5);" << fw::endl()
          << fw::endl();
      generate_tester("{1,2}", fw::lc("begin"), fw::lc("end"));
      generate_tester("{11,21,22}", b_begin, b_end);
      generate_tester("{12,23,24}", c_begin, c_end);
   }));
}

TEST(factDB_algebra_translator_FactorizedTableNode, TwoChildTablePartialIteratorDouble) {
   FactorizedTableNode n;
   n.require_size();
   auto a = std::make_unique<IU>("a", Type::Integer());
   auto b = std::make_unique<IU>("b", Type::Integer());
   auto c = std::make_unique<IU>("c", Type::Integer());
   n.add_iu(*a);
   n.addChild(*b);
   n.addChild(*c);

   ASSERT_NO_THROW(test_iterator([&](FileWriter& out) {
      n.set_dereference_order(*a);
      auto [a_begin, a_end] = n.add_subiterator(*a, *a);
      auto [b_begin, b_end] = n.add_subiterator(*b, *a);
      auto [c_begin, c_end] = n.add_subiterator(*c, *b);
      auto [d_begin, d_end] = n.add_subiterator(*c, *a);
      n.genStruct(out);

      auto generate_tester2 = [&](const std::string& result, const factDB::FWContainer& begin, const factDB::FWContainer& end, bool seen_size_smaller) {
         out << "{" << fw::endl()
             << "  std::vector<std::tuple<int, int>> results(" << result << ");" << fw::endl()
             << "  std::vector<std::tuple<int, int>> seen;" << fw::endl()
             << "  for (auto iter = ft." << begin << "(); iter != ft." << end << "(); ++iter) {" << fw::endl()
             << "    auto [v1, v2] = *iter;" << fw::endl()
             << "    auto v_t = std::make_tuple(v1.value, v2.value);" << fw::endl()
             << "    assert_exists(results, v_t);" << fw::endl()
             << "    seen.push_back(v_t);" << fw::endl()
             << "  }" << fw::endl()
             << (seen_size_smaller ? "  assert_true(seen.size() < ft.size());" : "") << fw::endl()
             << "  remove_duplicates(seen);" << fw::endl()
             << "  assert_eq(results.size(), seen.size());" << fw::endl()
             << "  std::cout << std::endl;" << fw::endl()
             << "}" << fw::endl()
             << fw::endl();
      };

      out << n.class_name << " ft;" << fw::endl()
          << "auto& h1 = " << n.push_back("ft", *a, 1) << fw::endl()
          << n.push_back("ft", "h1", *b, 11) << "assert_eq(ft.size(), 0);" << fw::endl()
          << n.push_back("ft", "h1", *c, 12) << "assert_eq(ft.size(), 1);" << fw::endl()
          << fw::endl()
          << "auto& h2 = " << n.push_back("ft", *a, 2) << fw::endl()
          << n.push_back("ft", "h2", *b, 21) << "assert_eq(ft.size(), 1);" << fw::endl()
          << n.push_back("ft", "h2", *b, 22) << "assert_eq(ft.size(), 1);" << fw::endl()
          << n.push_back("ft", "h2", *c, 23) << "assert_eq(ft.size(), 3);" << fw::endl()
          << n.push_back("ft", "h2", *c, 24) << "assert_eq(ft.size(), 5);" << fw::endl()
          << fw::endl();
      generate_tester2("{{1,1},{2,2}}", a_begin, a_end, true);
      generate_tester2("{{11,1},{21,2},{22,2}}", b_begin, b_end, true);
      generate_tester2("{{12,11},{23,21},{23,22},{24,21},{24,22}}", c_begin, c_end, false);
      generate_tester2("{{12,1},{23,2},{24,2}}", d_begin, d_end, true);
   }));
}

TEST(factDB_algebra_translator_FactorizedTableNode, TwoChildTablePartialIteratorTriple) {
   FactorizedTableNode n;
   n.require_size();
   auto a = std::make_unique<IU>("a", Type::Integer());
   auto b = std::make_unique<IU>("b", Type::Integer());
   auto c = std::make_unique<IU>("c", Type::Integer());
   auto d = std::make_unique<IU>("d", Type::Integer());
   auto e = std::make_unique<IU>("e", Type::Integer());
   n.add_iu(*a);
   n.addChild(*b);
   n.addChild(*c);

   FactorizedTableNode n2;
   n2.add_iu(*a);
   n2.addChild(*b).addChild(*d);
   n2.addChild(*c);

   ASSERT_NO_THROW(test_iterator([&](FileWriter& out) {
      n.set_dereference_order(*a, *b, *c);
      n2.set_dereference_order();
      n2.genStruct(out);
      n.genStruct(out);
      auto& n3 = n2.genIteratorInsert({e.get()});
      n3.set_dereference_order(*a, *b, *c);
      n3.genStruct(out);

      std::string handle = "h1";
      out << n.class_name << " ft;" << fw::endl()
          << "auto& " << handle << " = " << n.push_back("ft", *a, 1)
          << n.push_back("ft", handle, *b, 11) << "assert_eq(ft.size(), 0);" << fw::endl()
          << n.push_back("ft", handle, *c, 12) << "assert_eq(ft.size(), 1);" << fw::endl()
          << n.push_back("ft", *a, 3) << "assert_eq(ft.size(), 1);" << fw::endl()
          << fw::endl();
      handle = "h2";
      out << "auto& " << handle << " = " << n.push_back("ft", *a, 2) << fw::endl()
          << n.push_back("ft", handle, *b, 21) << "assert_eq(ft.size(), 1);" << fw::endl()
          << n.push_back("ft", handle, *b, 22) << "assert_eq(ft.size(), 1);" << fw::endl()
          << n.push_back("ft", handle, *c, 23) << "assert_eq(ft.size(), 3);" << fw::endl()
          << n.push_back("ft", handle, *c, 24) << "assert_eq(ft.size(), 5);" << fw::endl()
          << fw::endl()
          << n.push_back("ft", *a, 4) << "assert_eq(ft.size(), 5);" << fw::endl()
          << fw::endl()

          << fw::endl()
          << "{" << fw::endl()
          << "  std::vector<std::tuple<int, int, int>> results({{1,11,12},{2,21,23},{2,21,24},{2,22,23},{2,22,24}});" << fw::endl()
          << "  std::vector<std::tuple<int, int, int>> seen;" << fw::endl()
          << "  for (auto iter = ft.begin(); iter != ft.end(); ++iter) {" << fw::endl()
          << "    auto [v1, v2, v3] = *iter;" << fw::endl()
          << "    auto v_t = std::make_tuple(v1.value, v2.value, v3.value);" << fw::endl()
          << "  std::cout << v1.value << \"\t\"<< v2.value << \"\t\"<< v3.value << std::endl;" << fw::endl()
          // << fw::cout("v1.value << \"\t\"<< v2.value << \"\t\"<< v3.value")
          << "    assert_exists(results, v_t);" << fw::endl()
          << "    seen.push_back(v_t);" << fw::endl()
          << "  }" << fw::endl()
          << "  remove_duplicates(seen);" << fw::endl()
          << "  assert_eq(results.size(), seen.size());" << fw::endl()
          << "  std::cout << std::endl;" << fw::endl()
          //<< fw::cout()
          << "}" << fw::endl()
          << fw::endl();
   }));
}

TEST(factDB_algebra_translator_FactorizedTableNode, TwoChildTableInsert) {
   FactorizedTableNode n;
   n.require_size();
   auto a = std::make_unique<IU>("a", Type::Integer());
   auto b = std::make_unique<IU>("b", Type::Integer());
   auto c = std::make_unique<IU>("c", Type::Integer());
   auto e = std::make_unique<IU>("e", Type::Integer());
   n.add_iu(*a);
   n.addChild(*b);
   n.addChild(*c);

   n.set_dereference_order(*a, *b, *c);
   auto& n2 = n.genIteratorInsert({e.get()});
   n2.set_dereference_order(*a, *b, *c, *e);

   ASSERT_NO_THROW(test_iterator([&](FileWriter& out) {
      n.genStruct(out);

      std::string handle = "h1";
      out << n.class_name << " ft;" << fw::endl()
          << n2.class_name << " ft2;" << fw::endl()
          << "auto& " << handle << " = " << n.push_back("ft", *a, 1)
          << n.push_back("ft", handle, *b, 11) << "assert_eq(ft.size(), 0);" << fw::endl()
          << n.push_back("ft", handle, *c, 12) << "assert_eq(ft.size(), 1);" << fw::endl()
          << n.push_back("ft", *a, 3) << "assert_eq(ft.size(), 1);" << fw::endl()
          << fw::endl();
      handle = "h2";
      out << "auto& " << handle << " = " << n.push_back("ft", *a, 2) << fw::endl()
          << n.push_back("ft", handle, *b, 21) << "assert_eq(ft.size(), 1);" << fw::endl()
          << n.push_back("ft", handle, *b, 22) << "assert_eq(ft.size(), 1);" << fw::endl()
          << n.push_back("ft", handle, *c, 23) << "assert_eq(ft.size(), 3);" << fw::endl()
          << n.push_back("ft", handle, *c, 24) << "assert_eq(ft.size(), 5);" << fw::endl()
          << fw::endl()
          << n.push_back("ft", *a, 4) << "assert_eq(ft.size(), 5);" << fw::endl()
          << fw::endl()
          << fw::endl()
          << "{" << fw::endl()
          << "  std::vector<std::tuple<int, int, int>> results({{1,11,12},{2,21,23},{2,21,24},{2,22,23},{2,22,24}});" << fw::endl()
          << "  std::vector<std::tuple<int, int, int>> seen;" << fw::endl()
          << "  for (auto iter = ft.begin(); iter != ft.end(); ++iter) {" << fw::endl()
          << "    auto [v1, v2, v3] = *iter;" << fw::endl()
          << "    auto v_t = std::make_tuple(v1.value, v2.value, v3.value);" << fw::endl()
          << "    std::cout << v1.value << \"\t\" << v2.value << \"\t\" << v3.value << std::endl;" << fw::endl()
          // << fw::cout("v1.value << \"\t\" << v2.value << \"\t\" << v3.value ")
          << "    assert_exists(results, v_t);" << fw::endl()
          << "    seen.push_back(v_t);" << fw::endl()
          << "    iter.insert(ft2, Integer(1));" << fw::endl()
          << "  }" << fw::endl()
          << "  remove_duplicates(seen);" << fw::endl()
          << "  assert_eq(results.size(), seen.size());" << fw::endl()
          << "}" << fw::endl()
          << "{" << fw::endl()
          << "  std::vector<std::tuple<int, int, int, int>> results({{1,11,12,1},{2,21,23,1},{2,21,24,1},{2,22,23,1},{2,22,24,1}});" << fw::endl()
          << "  std::vector<std::tuple<int, int, int, int>> seen;" << fw::endl()
          << "  for (auto iter = ft2.begin(); iter != ft2.end(); ++iter) {" << fw::endl()
          << "    auto [v1, v2, v3, v4] = *iter;" << fw::endl()
          << "    auto v_t = std::make_tuple(v1.value, v2.value, v3.value, v4.value);" << fw::endl()
          << " std::cout << v1.value << \"\t\" << v2.value << \"\t\" << v3.value << \"\t\" << v4.value << std::endl;" << fw::endl()
          // << fw::cout("v1.value << \"\t\" << v2.value << \"\t\" << v3.value << \"\t\" << v4.value ")
          << "    assert_exists(results, v_t);" << fw::endl()
          << "    seen.push_back(v_t);" << fw::endl()
          << "  }" << fw::endl()
          << "  assert_eq(results.size(), seen.size());" << fw::endl()
          << "}" << fw::endl()
          << fw::endl();
   }));
}

} // namespace
