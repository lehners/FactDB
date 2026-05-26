#include "factDB/algebra/visitors/GraphvizQueryPlan.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/CrossProduct.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
FWContainer genOperatorLbl(const FWContainer& opStr) {
   return fw::fmt("\"{{ {} }} \"", opStr);
}
// ---------------------------------------------------------------------------------------------------
FWContainer genExpressionLbl(const Expression& expr) {
   switch (expr.get_type()) {
      case Expression::CompareL:
      case Expression::CompareLE:
      case Expression::CompareEq:
      case Expression::CompareGE:
      case Expression::CompareG: {
         auto& binaryExpr = static_cast<const BinaryExpression&>(expr);
         return fw::lc(genExpressionLbl(binaryExpr.get_left()),
                       "\\", binaryExpr.getComparator(),
                       genExpressionLbl(binaryExpr.get_right()));
      }
      case Expression::OrExpression:
      case Expression::AndExpression: {
         auto& binaryExpr = static_cast<const BinaryExpression&>(expr);
         return fw::lc(genExpressionLbl(binaryExpr.get_left()),
                       "\\n",
                       binaryExpr.getComparator(),
                       genExpressionLbl(binaryExpr.get_right()));
      }
      case Expression::IURef: return fw::lc(static_cast<const IURef&>(expr).get_iu());
      case Expression::Const: return fw::lc(static_cast<const Const&>(expr).get_value());

      case Expression::JoinCondition: {
         auto& jc = static_cast<const JoinCondition&>(expr);
         return fw::lc(jc.get_left(), "==", jc.get_right());
      };
      case Expression::JoinConditionList: {
         FWContainer ret;
         auto& jcl = static_cast<const JoinConditionList&>(expr);
         for (bool first = true; const auto& jc : jcl.get_conditions()) {
            ret = !first ? fw::lc(std::move(ret),
                                  "&&\\n",
                                  genExpressionLbl(jc)) :
                           genExpressionLbl(jc);
            first = false;
         }
         return ret;
      }
      case Expression::Reference:
         genExpressionLbl(static_cast<const ReferenceExpression&>(expr).getReferencedExpr());
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
GraphvizQueryPlan::GraphvizQueryPlan(const Database& db, std::ostream& writer)
   : AlgebraVisitorDB(db), out(writer, FileWriter::Silent, FileWriter::NoFinalNewline) {
}
// ---------------------------------------------------------------------------------------------------
void GraphvizQueryPlan::open() {
   out << "digraph structs {" << fw::endl()
       << "  node [shape=record];" << fw::endl()
       << fw::pushSeparator("|");
}
// ---------------------------------------------------------------------------------------------------
void GraphvizQueryPlan::close() {
   out << fw::popSeparator()
       << "}" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void GraphvizQueryPlan::visitProduce(const Print& print) {
   out << fw::fmt("struct{} [label={}]", print.getOperatorId(), genOperatorLbl("Print")) << fw::endl();

   print.getChild()->produce(*this);

   out << fw::fmt("struct{} -> struct{};", print.getOperatorId(), print.getChild()->getOperatorId()) << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void GraphvizQueryPlan::visitProduce(const Count& count) {
   out << fw::fmt("struct{} [label=", count.getOperatorId()) << genOperatorLbl("Count") << "];" << fw::endl();

   count.getChild()->produce(*this);

   out << fw::fmt("struct{} -> struct{};", count.getOperatorId(), count.getChild()->getOperatorId()) << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void GraphvizQueryPlan::visitProduce(const Selection& selection) {
   FWContainer lbl = fw::lc("&#963; \\n", genExpressionLbl(selection.getPredicate()));
   out << fw::checkQuotes(false);
   out << fw::fmt("struct{} [label=", selection.getOperatorId()) << "\"{ " << lbl << "}\" ];" << fw::endl();
   out << fw::checkQuotes(true);
   selection.getChild()->produce(*this);
   out << fw::fmt("struct{} -> struct{};", selection.getOperatorId(), selection.getChild()->getOperatorId()) << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void GraphvizQueryPlan::visitProduce(const TableScan& tableScan) {
   std::string lbl = std::string(database.getSchema(tableScan.getTable()).name) + " " + tableScan.getAlias();
   out << fw::fmt("struct{} [label=", tableScan.getOperatorId()) << genOperatorLbl(lbl) << "];" << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void GraphvizQueryPlan::visitProduce(const InnerJoin& innerJoin) {
   FWContainer lbl = fw::lc((innerJoin.getJoinType() == JoinType::Inner ? "&#10781; " : "⋊ "), (innerJoin.getJoinMode() == JoinMode::BottomInsert ? "BI" : "TI"), "\\n",
                            genExpressionLbl(innerJoin.getJoinCondition()));

   out << fw::checkQuotes(false);
   out << fw::fmt("struct{} [label=", innerJoin.getOperatorId()) << "\"{ " << lbl << "}\" ];" << fw::endl();
   innerJoin.getLeftChild()->produce(*this);
   innerJoin.getRightChild()->produce(*this);
   out << fw::checkQuotes(true);
   out << fw::fmt("struct{} -> struct{};", innerJoin.getOperatorId(), innerJoin.getLeftChild()->getOperatorId()) << fw::endl()
       << fw::fmt("struct{} -> struct{};", innerJoin.getOperatorId(), innerJoin.getRightChild()->getOperatorId()) << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void GraphvizQueryPlan::visitProduce(const factDB::algebra::CrossProduct& crossProduct) {
   FWContainer lbl = std::string("&#10005; ");

   out << fw::checkQuotes(false);
   out << fw::fmt("struct{} [label=", crossProduct.getOperatorId()) << "\"{ " << lbl << "}\" ];" << fw::endl();
   crossProduct.getLeftChild()->produce(*this);
   crossProduct.getRightChild()->produce(*this);
   out << fw::checkQuotes(true);
   out << fw::fmt("struct{} -> struct{};", crossProduct.getOperatorId(), crossProduct.getLeftChild()->getOperatorId()) << fw::endl()
       << fw::fmt("struct{} -> struct{};", crossProduct.getOperatorId(), crossProduct.getRightChild()->getOperatorId()) << fw::endl();
}
// ---------------------------------------------------------------------------------------------------
void GraphvizQueryPlan::plotSubPlan(const Database& db, const factDB::algebra::Operator& op, std::ostream& out) {
   GraphvizQueryPlan gqp(db, out);
   gqp.open();
   op.produce(gqp);
   gqp.close();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------