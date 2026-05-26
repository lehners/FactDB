// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/SQLGenerator.hpp"
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
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
// generate group by for semi-join queries? -> count * all distinct tuples from join of outputted tuples? is that correct, probably duplicates make some problems? todo fix
// SELECT count(*) FROM (
//        select distinct dblp5.* from dblp5, dblp23 dblp23, dblp18 dblp18, dblp9 dblp9, dblp25 dblp25 WHERE
//                (dblp23.s = dblp9.s) AND (dblp18.s = dblp9.s) AND (dblp23.s = dblp5.s) AND (dblp25.s = dblp5.s)
// )
// ---------------------------------------------------------------------------------------------------
FileWriter& SQLGenerator::printBinaryExpression(const Expression& expression, std::string_view sep) {
   assert(expression.get_type() == Expression::AndExpression || expression.get_type() == Expression::OrExpression || expression.get_type() == Expression::CompareL || expression.get_type() == Expression::CompareLE || expression.get_type() == Expression::CompareEq || expression.get_type() == Expression::CompareG || expression.get_type() == Expression::CompareGE);
   auto& binaryExpression = static_cast<const BinaryExpression&>(expression);
   out << "(";
   printExpression(binaryExpression.get_left(), false);
   out << " " << sep << " ";
   printExpression(binaryExpression.get_right(), false);
   return out << ")";
}
// ---------------------------------------------------------------------------------------------------
FileWriter& SQLGenerator::printIU(const IU& iu) {
   return out << iu.table << "." << iu.column;
}
// ---------------------------------------------------------------------------------------------------
FileWriter& SQLGenerator::printExpression(const Expression& expression, bool requiresSeparator) {
   out << fw::separator(requiresSeparator);
   switch (expression.get_type()) {
      case Expression::IURef:
         return printIU(static_cast<const IURef&>(expression).get_iu());
      case Expression::Const: {
         const auto& c = static_cast<const Const&>(expression);
         switch (c.getConstType().tclass) {
            case schemac::Type::Class::KChar:
            case schemac::Type::Class::KVarchar:
            case schemac::Type::Class::KUndefined:
               return out << "'" << c.get_value() << "'";
            case schemac::Type::Class::KBool:
            case schemac::Type::Class::KInteger:
            case schemac::Type::Class::KUInt64:
            case schemac::Type::Class::KTimestamp:
            case schemac::Type::Class::KDate:
            case schemac::Type::Class::KNumeric:
               return out << c.get_value();
         }
         unreachable();
      }
      case Expression::Reference:
         return printExpression(static_cast<const ReferenceExpression&>(expression).getReferencedExpr(), false);
      case Expression::AndExpression:
         return printBinaryExpression(expression, "AND");
      case Expression::OrExpression:
         return printBinaryExpression(expression, "OR");
      case Expression::CompareL:
         return printBinaryExpression(expression, "<");
      case Expression::CompareLE:
         return printBinaryExpression(expression, "<=");
      case Expression::CompareEq:
         return printBinaryExpression(expression, "=");
      case Expression::CompareGE:
         return printBinaryExpression(expression, ">=");
      case Expression::CompareG:
         return printBinaryExpression(expression, ">");
      case Expression::JoinCondition:
         printIU(static_cast<const JoinCondition&>(expression).get_left()) << " = ";
         printIU(static_cast<const JoinCondition&>(expression).get_right());
         return out;
      case Expression::JoinConditionList: {
         auto& condition_list = static_cast<const JoinConditionList&>(expression);
         out << "(";
         if (condition_list.get_conditions().empty())
            out << "1=1";
         for (auto&& [idx, c] : condition_list | views::enumerate) {
            out << (idx ? " AND " : "");
            printExpression(c, false);
         }
         return out << ")";
      }
      default: unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
void SQLGenerator::visitProduce(const InnerJoin& join) {
   switch (join.getJoinType()) {
      case JoinType::Inner:
         if (stage == Predicates)
            printExpression(join.getJoinCondition());
         join.getLeftChild()->produce(*this);
         join.getRightChild()->produce(*this);
         break;
      case JoinType::RightSemi:
         join.getRightChild()->produce(*this);
         if (stage == Predicates) {
            out << fw::separator() << "EXISTS (SELECT * FROM ";
            stage = Tables;
            out.setSeparator(", ");
            join.getLeftChild()->produce(*this);
            out << " WHERE 1=1";
            stage = Predicates;
            out.setSeparator(" AND ", false, false);
            join.getLeftChild()->produce(*this);
            printExpression(join.getJoinCondition());
            out << ")";
         }
   }
}
// ---------------------------------------------------------------------------------------------------
void SQLGenerator::visitProduce(const CrossProduct& crossProduct) {
   crossProduct.getLeftChild()->produce(*this);
   crossProduct.getRightChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void SQLGenerator::visitProduce(const Print& print) {
   out << "SELECT ";
   out.setSeparator(", ");
   for (auto& iu : print.getRequiredIus()) {
      out << fw::separator();
      printIU(*iu);
   }

   out << " FROM ";
   stage = Tables;
   out.setSeparator(", ");
   print.getChild()->produce(*this);
   out << " WHERE 1=1";
   stage = Predicates;
   out.setSeparator(" AND ", false, false);
   print.getChild()->produce(*this);
   out << ";";
}
// ---------------------------------------------------------------------------------------------------
void SQLGenerator::visitProduce(const Count& count) {
   out << "SELECT count(*) FROM ";
   stage = Tables;
   out.setSeparator(", ");
   count.getChild()->produce(*this);
   out << " WHERE 1=1";
   stage = Predicates;
   out.setSeparator(" AND ", false, false);
   count.getChild()->produce(*this);
   out << ";";
}
// ---------------------------------------------------------------------------------------------------
void SQLGenerator::visitProduce(const Selection& selection) {
   if (stage == Predicates)
      printExpression(selection.getPredicate());
   selection.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void SQLGenerator::visitProduce(const TableScan& tableScan) {
   if (stage == Tables)
      out << fw::separator() << database.getSchema(tableScan.getTable()).name << " " << tableScan.getAlias();
}
// ---------------------------------------------------------------------------------------------------
void SQLGenerator::genSQL(factDB::algebra::Operator* op, const Database& db, std::ostream& out) {
   assert(op->getType() != algebra::Operator::Print && op->getType() != algebra::Operator::Count);

   SQLGenerator sqlGen(out, db);
   out << "SELECT count(*) FROM ";
   sqlGen.stage = Tables;
   sqlGen.out.setSeparator(", ");
   op->produce(sqlGen);
   out << " WHERE 1=1";
   sqlGen.stage = Predicates;
   sqlGen.out.setSeparator(" AND ", false, false);
   op->produce(sqlGen);
   out << ";";
}
// ---------------------------------------------------------------------------------------------------
void SQLGenerator::genSQL(SQLGenerator::TreeStructure&& tree, const Database& db, std::ostream& ostream) {
   genSQL(std::get<0>(tree), db, std::get<1>(tree), ostream);
}
// ---------------------------------------------------------------------------------------------------
void SQLGenerator::genSQL(std::unique_ptr<factDB::algebra::Operator>& tree, const Database& db, OrderedIUSet& requiredColumns, std::ostream& ostream) {
   SQLGenerator sqlGen(ostream, db);
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