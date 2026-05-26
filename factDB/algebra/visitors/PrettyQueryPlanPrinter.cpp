#include "factDB/algebra/visitors/PrettyQueryPlanPrinter.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Operator.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/iu.hpp"
#include "fmt/format.h"
#include <sstream>

namespace factDB::algebra::visitors {

namespace {
std::ostream& operator<<(std::ostream& os, const IU& iu) {
   return os << iu.table << "." << iu.column;
}

std::ostream& operator<<(std::ostream& os, const Expression& expression) {
   switch (expression.get_type()) {
      case Expression::CompareL:
      case Expression::CompareLE:
      case Expression::CompareEq:
      case Expression::CompareGE:
      case Expression::CompareG: {
         const auto& cmp = static_cast<const BinaryExpression&>(expression);
         return os << cmp.get_left() << cmp.getComparator() << cmp.get_right();
      }
      case Expression::IURef:
         return os << static_cast<const IURef&>(expression).get_iu();
      case Expression::Const:
         return os << static_cast<const Const&>(expression).get_value();
      case Expression::Reference:
         return os << static_cast<const ReferenceExpression&>(expression).getReferencedExpr();
      case Expression::AndExpression: {
         const auto& andExpr = static_cast<const AndExpression&>(expression);
         return os << "( " << andExpr.get_left() << "\n&&" << andExpr.get_right() << ")";
      }
      case Expression::OrExpression: {
         const auto& andExpr = static_cast<const AndExpression&>(expression);
         return os << "( " << andExpr.get_left() << "\n||" << andExpr.get_right() << ")";
      }
      case Expression::JoinCondition:
         break;
      case Expression::JoinConditionList:
         for (auto& a : static_cast<const JoinConditionList&>(expression))
            os << a.get_left() << " == " << a.get_right() << "\n";
         return os;
   }
   return os;
}

std::string exprToString(const Expression& expr) {
   std::stringstream ss;
   ss << expr;
   return ss.str();
}

} // anonymous namespace

PrettyQueryPlanWriter::TreePrinter::TreePrinter(std::ostream& ostream) : out(ostream) {}

std::ostream& PrettyQueryPlanWriter::TreePrinter::center(const std::string& str, size_t begin, size_t end) {
   auto totalWidth = (end - begin + 1) * width;
   auto strSize = str.size();
   if (strSize < totalWidth) {
      return out << std::string((totalWidth - strSize) / 2, ' ') << str << std::string((totalWidth - strSize + 1) / 2, ' ');
   } else {
      return out << str.substr(0, totalWidth);
   }
}

void PrettyQueryPlanWriter::TreePrinter::print() {
   for (size_t level = 1; level <= maxLevel; ++level) {
      std::vector<std::vector<std::string>> splittedStrings;
      splittedStrings.reserve(maxIndent);
      size_t maxLines = 0;
      // preprocess lines and split at newlines
      for (size_t indent = 1; indent <= maxIndent; indent++) {
         if (!tree.contains({level, indent})) continue;
         auto& lines = splittedStrings.emplace_back();
         std::stringstream ss(std::get<0>(tree[{level, indent}]));
         std::string line;

         while (std::getline(ss, line, '\n'))
            lines.push_back(line);
         maxLines = std::max(lines.size(), maxLines);
         indent = std::get<2>(tree[{level, indent}]);
      }

      for (size_t lineIdx = 0; lineIdx < maxLines; lineIdx++) {
         size_t curIdx = 0;
         for (size_t indent = 1; indent <= maxIndent; ++indent) {
            if (tree.contains({level, indent})) {
               const auto& [_, begin, end] = tree[{level, indent}];
               auto& curStrings = splittedStrings[curIdx++];
               center(lineIdx < curStrings.size() ? curStrings[lineIdx] : "", begin, end);
               indent = end;
            } else {
               center("", 0, 0);
            }
         }
         out << std::endl;
      }
      out << std::endl;
   }
}

size_t& PrettyQueryPlanWriter::TreePrinter::insert(size_t level, size_t indent, const std::string& str) {
   tree.emplace(std::make_pair(level, indent), std::make_tuple(str, indent, indent));
   maxLevel = std::max(maxLevel, level);
   maxIndent = std::max(maxIndent, indent);
   return std::get<2>(tree[{level, indent}]);
}

PrettyQueryPlanWriter::PrettyQueryPlanWriter(const Database& db, std::ostream& writer)
   : AlgebraVisitorDB(db), printer(writer) {
}

void PrettyQueryPlanWriter::visitProduce(const InnerJoin& join) {
   size_t& right = printer.insert(curLevel, curIndent, fmt::format("Join\n{}", exprToString(join.getJoinCondition())));
   ++curLevel;
   join.getLeftChild()->produce(*this);
   right = ++curIndent;
   join.getRightChild()->produce(*this);
   --curLevel;
}

void PrettyQueryPlanWriter::visitProduce(const Print& print) {
   size_t& right = printer.insert(curLevel, curIndent, "Print");
   ++curLevel;
   print.getChild()->produce(*this);
   --curLevel;
   right = curIndent;
}

void PrettyQueryPlanWriter::visitProduce(const Selection& selection) {
   size_t& right = printer.insert(curLevel, curIndent, fmt::format("Selection\n{}", exprToString(selection.getPredicate())));
   ++curLevel;
   selection.getChild()->produce(*this);
   --curLevel;
   right = curIndent;
}

void PrettyQueryPlanWriter::visitProduce(const TableScan& tableScan) {
   printer.insert(curLevel, curIndent, fmt::format("TableScan\n{}\n{}", database.getSchema(tableScan.getTable()).name, tableScan.getAlias()));
}

void PrettyQueryPlanWriter::visitProduce(const Count& count) {
   size_t& right = printer.insert(curLevel, curIndent, "Count");
   ++curLevel;
   count.getChild()->produce(*this);
   --curLevel;
   right = curIndent;
}

void PrettyQueryPlanWriter::close() {
   printer.print();
}

} // namespace factDB::algebra::visitors

#include "factDB/infra/util/ranges.hpp"
