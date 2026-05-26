// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/SQLGeneratorSemijoinGroupBy.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/CrossProduct.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Operator.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/IUSet.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/util/ranges.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
#include "factDB/util/TemporaySetter.hpp"
#include <sstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
FileWriter& SQLGeneratorSemijoinGroupBy::printBinaryExpression(FileWriter& writer, const Expression& expression, const AliasMap& map, std::string_view sep) {
   assert(expression.get_type() == Expression::AndExpression || expression.get_type() == Expression::OrExpression || expression.get_type() == Expression::CompareL || expression.get_type() == Expression::CompareLE || expression.get_type() == Expression::CompareEq || expression.get_type() == Expression::CompareG || expression.get_type() == Expression::CompareGE);
   auto& binaryExpression = static_cast<const BinaryExpression&>(expression);
   writer << "(";
   printExpression(writer, binaryExpression.get_left(), map) << " " << sep << " ";
   printExpression(writer, binaryExpression.get_right(), map);
   return writer << ")";
}
// ---------------------------------------------------------------------------------------------------
FWContainer SQLGeneratorSemijoinGroupBy::printIU(const IU& iu, const AliasMap& map) {
   if (map.contains(iu.table)) {
      return fw::lc(iu);
   } else {
      return fw::lc(iu.table, ".", iu.column);
   }
}
// ---------------------------------------------------------------------------------------------------
FileWriter& SQLGeneratorSemijoinGroupBy::printExpression(FileWriter& writer, const Expression& expression, const AliasMap& map) {
   switch (expression.get_type()) {
      case Expression::IURef:
         return writer << printIU(static_cast<const IURef&>(expression).get_iu(), map);
      case Expression::Const: {
         const auto& c = static_cast<const Const&>(expression);
         switch (c.getConstType().tclass) {
            case schemac::Type::Class::KChar:
            case schemac::Type::Class::KVarchar:
            case schemac::Type::Class::KUndefined:
               return writer << "'" << c.get_value() << "'";
            case schemac::Type::Class::KBool:
            case schemac::Type::Class::KInteger:
            case schemac::Type::Class::KUInt64:
            case schemac::Type::Class::KTimestamp:
            case schemac::Type::Class::KDate:
            case schemac::Type::Class::KNumeric:
               return writer << c.get_value();
         }
         unreachable();
      }
      case Expression::Reference:
         return printExpression(writer, static_cast<const ReferenceExpression&>(expression).getReferencedExpr(), map);
      case Expression::AndExpression:
         return printBinaryExpression(writer, expression, map, "AND");
      case Expression::OrExpression:
         return printBinaryExpression(writer, expression, map, "OR");
      case Expression::CompareL:
         return printBinaryExpression(writer, expression, map, "<");
      case Expression::CompareLE:
         return printBinaryExpression(writer, expression, map, "<=");
      case Expression::CompareEq:
         return printBinaryExpression(writer, expression, map, "=");
      case Expression::CompareGE:
         return printBinaryExpression(writer, expression, map, ">=");
      case Expression::CompareG:
         return printBinaryExpression(writer, expression, map, ">");
      case Expression::JoinCondition: {
         std::stringstream ss1, ss2;
         fw::FileWriter localWriter1(ss1, fw::FileWriter::NoFinalNewline), localWriter2(ss2, fw::FileWriter::NoFinalNewline);
         localWriter1 << printIU(static_cast<const JoinCondition&>(expression).get_left(), map);
         localWriter2 << printIU(static_cast<const JoinCondition&>(expression).get_right(), map);

         auto leftIU = ss1.str();
         auto rightIU = ss2.str();

         if (leftIU < rightIU)
            writer << leftIU << "=" << rightIU;
         else
            writer << rightIU << "=" << leftIU;
      }
         return writer;
      case Expression::JoinConditionList:
         unreachable();
      default: unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
void SQLGeneratorSemijoinGroupBy::visitProduce(const InnerJoin& join) {
   switch (join.getJoinType()) {
      case JoinType::Inner:
         join.getLeftChild()->produce(*this);
         join.getRightChild()->produce(*this);
         break;
      case JoinType::RightSemi: {
         std::string subquery;
         {
            TemporarySetter tsTables(tables, {});
            TemporarySetter tsConditions(conditions, {});

            join.getLeftChild()->produce(*this);

            OrderedIUSet ius(join.getLeftRequired());
            std::sort(ius.begin(), ius.end(), OrderedIUSet::iuOrder);
            std::sort(tables.begin(), tables.end());
            std::sort(conditions.begin(), conditions.end());
            FWContainer iuPrinterGroupBy = fw::iter(ius | views::transform([&](const IU* iu) { return printIU(*iu, aliasMap); }));
            FWContainer iuPrinterProjection = fw::iter(ius | views::transform([&](const IU* iu) { return fw::fmt("{} as {}", printIU(*iu, aliasMap), iu); }));
            FWContainer tblIter = fw::iter(tables | views::transform([&](const std::string& tbl) { if (tbl.starts_with("(")) return fw::lc(tbl, " semijoin", ++semijoin_id); else return fw::lc(tbl); }));

            if (conditions.empty())
               conditions.emplace_back("1=1");

            std::stringstream ss;
            fw::FileWriter localWriter(ss, fw::FileWriter::NoFinalNewline);
            localWriter << fw::fmt("(SELECT {} FROM {} WHERE {} GROUP BY {})", iuPrinterProjection, tblIter, fw::iter(conditions, " AND "), iuPrinterGroupBy);
            subquery = ss.str();
         }
         tables.emplace_back(std::move(subquery));
         // checks have to be performed first since otherwise it may already be inserted in the loop
         for (auto& iu : join.getLeftRequired())
            assert(!aliasMap.contains(iu->table));
         for (auto& iu : join.getLeftRequired()) {
            aliasMap.insert(iu->table);
         }
         join.getRightChild()->produce(*this);
      } break;
   }
   // need to do this afterward due to the alias map
   for (auto c : join.getJoinCondition()) {
      std::stringstream ss;
      fw::FileWriter localWriter(ss, fw::FileWriter::NoFinalNewline);

      printExpression(localWriter, c, aliasMap);
      conditions.emplace_back(ss.str());
   }
}
// ---------------------------------------------------------------------------------------------------
void SQLGeneratorSemijoinGroupBy::visitProduce(const CrossProduct& crossProduct) {
   crossProduct.getLeftChild()->produce(*this);
   crossProduct.getRightChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void SQLGeneratorSemijoinGroupBy::visitProduce(const Print& print) {
   print.getChild()->produce(*this);

   auto ius = print.getRequiredIus();
   std::sort(ius.begin(), ius.end(), OrderedIUSet::iuOrder);
   std::sort(tables.begin(), tables.end());
   std::sort(conditions.begin(), conditions.end());
   FWContainer iuPrinter = fw::iter(ius | views::transform([&](const IU* iu) { return printIU(*iu, aliasMap); }));
   FWContainer tblIter = fw::iter(tables | views::transform([&](const std::string& tbl) { if (tbl.starts_with("(")) return fw::lc(tbl, " semijoin", ++semijoin_id); else return fw::lc(tbl); }));

   if (conditions.empty())
      conditions.emplace_back("1=1");

   out << fw::fmt("SELECT {} FROM {} WHERE {};", iuPrinter, tblIter, fw::iter(conditions, " AND "));
}
// ---------------------------------------------------------------------------------------------------
void SQLGeneratorSemijoinGroupBy::visitProduce(const Count& count) {
   count.getChild()->produce(*this);

   std::sort(tables.begin(), tables.end());
   std::sort(conditions.begin(), conditions.end());
   FWContainer tblIter = fw::iter(tables | views::transform([&](const std::string& tbl) { if (tbl.starts_with("(")) return fw::lc(tbl, " semijoin", ++semijoin_id); else return fw::lc(tbl); }));

   if (conditions.empty())
      conditions.emplace_back("1=1");

   out << fw::fmt("SELECT count(*) FROM {} WHERE {};", tblIter, fw::iter(conditions, " AND "));
}
// ---------------------------------------------------------------------------------------------------
void SQLGeneratorSemijoinGroupBy::visitProduce(const Selection& selection) {
   std::stringstream ss;
   fw::FileWriter localWriter(ss, fw::FileWriter::NoFinalNewline);

   printExpression(localWriter, selection.getPredicate(), aliasMap);
   conditions.emplace_back(ss.str());

   selection.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void SQLGeneratorSemijoinGroupBy::visitProduce(const TableScan& tableScan) {
   tables.emplace_back(database.getSchema(tableScan.getTable()).name + " " + tableScan.getAlias());
}
// ---------------------------------------------------------------------------------------------------
void SQLGeneratorSemijoinGroupBy::genSQL(factDB::algebra::Operator* op, const Database& db, std::ostream& out) {
   assert(op->getType() != algebra::Operator::Print && op->getType() != algebra::Operator::Count);

   SQLGeneratorSemijoinGroupBy sqlGen(out, db);

   op->produce(sqlGen);

   std::sort(sqlGen.tables.begin(), sqlGen.tables.end());
   std::sort(sqlGen.conditions.begin(), sqlGen.conditions.end());
   FWContainer tblIter = fw::iter(sqlGen.tables | views::transform([&](const std::string& tbl) { if (tbl.starts_with("(")) return fw::lc(tbl, " semijoin", ++sqlGen.semijoin_id); else return fw::lc(tbl); }));

   if (sqlGen.conditions.empty())
      sqlGen.conditions.emplace_back("1=1");

   sqlGen.out << fw::fmt("SELECT count(*) FROM {} WHERE {};", tblIter, fw::iter(sqlGen.conditions, " AND "));
}
// ---------------------------------------------------------------------------------------------------
void SQLGeneratorSemijoinGroupBy::genSQL(SQLGeneratorSemijoinGroupBy::TreeStructure&& tree, const Database& db, std::ostream& ostream) {
   genSQL(std::get<0>(tree), db, std::get<1>(tree), ostream);
}
// ---------------------------------------------------------------------------------------------------
void SQLGeneratorSemijoinGroupBy::genSQL(std::unique_ptr<factDB::algebra::Operator>& tree, const Database& db, OrderedIUSet& requiredColumns, std::ostream& ostream) {
   SQLGeneratorSemijoinGroupBy sqlGen(ostream, db);
   switch (tree->getType()) {
      case algebra::Operator::Print:
         static_cast<algebra::Print*>(tree.get())->prepare(sqlGen, requiredColumns, nullptr);
         break;
      case algebra::Operator::Count:
         static_cast<algebra::Count*>(tree.get())->prepare(sqlGen, requiredColumns, nullptr);
         break;
      case algebra::Operator::InnerJoin:
      case algebra::Operator::Selection:
      case algebra::Operator::TableScan:
      case algebra::Operator::CrossProduct:
      case algebra::Operator::Reference:
         unreachable();
   }
   tree->produce(sqlGen);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
