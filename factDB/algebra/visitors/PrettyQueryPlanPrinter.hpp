#ifndef H_FACTDB_FACT_DB_ALGEBRA_VISITORS_PRETTYQUERYPLANPRINTER_HPP
#define H_FACTDB_FACT_DB_ALGEBRA_VISITORS_PRETTYQUERYPLANPRINTER_HPP

#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace factDB::algebra::visitors {

class PrettyQueryPlanWriter : public AlgebraVisitorDB {
   private:
   int curLevel = 1;
   int curIndent = 1;

   class TreePrinter {
      struct PairHash {
         template <class T1, class T2>
         std::size_t operator()(const std::pair<T1, T2>& p) const {
            return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
         }
      };
      std::unordered_map<std::pair<int, int>, std::tuple<std::string, size_t, size_t>, PairHash> tree;
      size_t maxLevel = 0;
      size_t maxIndent = 0;
      std::ostream& out;
      static const size_t width = 25;

      std::ostream& center(const std::string& str, size_t begin, size_t end);

      public:
      explicit TreePrinter(std::ostream& outstream);
      void print();
      size_t& insert(size_t level, size_t indent, const std::string& str);
   };

   TreePrinter printer;

   public:
   explicit PrettyQueryPlanWriter(const Database& db, std::ostream& writer);
   virtual ~PrettyQueryPlanWriter() override = default;

   void visitProduce(const InnerJoin& join) override;
   void visitProduce(const Print& print) override;
   void visitProduce(const Selection& selection) override;
   void visitProduce(const TableScan& tableScan) override;
   void visitProduce(const Count& count) override;

   void close() override;
};

} // namespace factDB::algebra::visitors

#endif // H_FACTDB_FACT_DB_ALGEBRA_VISITORS_PRETTYQUERYPLANPRINTER_HPP
