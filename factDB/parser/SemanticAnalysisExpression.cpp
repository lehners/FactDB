// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include "factDB/algebra/CrossProduct.hpp"
#include "factDB/algebra/Operator.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/RuntimeException.hpp"
#include "factDB/parser/AST.hpp"
#include "factDB/parser/SemanticAnalysis.hpp"
#include "factDB/statement/CopyStatement.hpp"
#include "factDB/statement/CreateTableStatement.hpp"
#include "factDB/statement/Statement.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::parser {
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<Expression> SemanticAnalysis::analyseUnaryExpression(parser::UnaryExpression& unaryExpression) {
   switch (unaryExpression.getSubType()) {
      case UnaryExpression::UMINUS:
         throw RuntimeException(RuntimeError, "UnaryExpression UMINUS not implemented yet");
      case UnaryExpression::UPLUS:
         throw RuntimeException(RuntimeError, "UnaryExpression UPLUS not implemented yet");
      case UnaryExpression::NOT:
         throw RuntimeException(RuntimeError, "UnaryExpression NOT not implemented yet");
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<Expression> SemanticAnalysis::analyseBinaryExpression(parser::BinaryExpression& binaryExpression) {
   auto leftExpression = analyseExpression(*binaryExpression.getLeft());
   auto rightExpression = analyseExpression(*binaryExpression.getRight());

   switch (binaryExpression.getOp()) {
      case BinaryExpression::PLUS: throw RuntimeException(RuntimeError, "Binary Expression PLUS not implemented yet.");
      case BinaryExpression::MINUS: throw RuntimeException(RuntimeError, "Binary Expression MINUS not implemented yet.");
      case BinaryExpression::MUL: throw RuntimeException(RuntimeError, "Binary Expression MUL not implemented yet.");
      case BinaryExpression::DIV: throw RuntimeException(RuntimeError, "Binary Expression DIV not implemented yet.");
      case BinaryExpression::MOD: throw RuntimeException(RuntimeError, "Binary Expression MOD not implemented yet.");
      case BinaryExpression::POW: throw RuntimeException(RuntimeError, "Binary Expression POW not implemented yet.");
      case BinaryExpression::LT: return std::make_unique<CompareLess>(std::move(leftExpression), std::move(rightExpression));
      case BinaryExpression::GT: return std::make_unique<CompareGreater>(std::move(leftExpression), std::move(rightExpression));
      case BinaryExpression::EQ: return std::make_unique<CompareEqual>(std::move(leftExpression), std::move(rightExpression));
      case BinaryExpression::LE: return std::make_unique<CompareLessEqual>(std::move(leftExpression), std::move(rightExpression));
      case BinaryExpression::GE: return std::make_unique<CompareGreaterEqual>(std::move(leftExpression), std::move(rightExpression));
      case BinaryExpression::NEQ: throw RuntimeException(RuntimeError, "Binary Expression POW not implemented yet.");
      case BinaryExpression::AND: return std::make_unique<AndExpression>(std::move(leftExpression), std::move(rightExpression));
      case BinaryExpression::OR: return std::make_unique<OrExpression>(std::move(leftExpression), std::move(rightExpression));
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<Expression> SemanticAnalysis::analyseBetweenExpression(parser::BetweenExpression& binaryExpression) {
   auto valExpression1 = analyseExpression(*binaryExpression.getVal());
   auto valExpression2 = analyseExpression(*binaryExpression.getVal());
   auto lowerExpression = analyseExpression(*binaryExpression.getLower());
   auto upperExpression = analyseExpression(*binaryExpression.getUpper());

   return std::make_unique<AndExpression>(
      std::make_unique<CompareGreaterEqual>(std::move(valExpression2), std::move(lowerExpression)),
      std::make_unique<CompareLessEqual>(std::move(valExpression1), std::move(upperExpression)));
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<Expression> SemanticAnalysis::analyseColId(parser::ColId& colId) {
   const IU* iu = scope.findIU(extractName(colId.getColumn()), extractNamePtr(colId.getTable()));
   return std::make_unique<IURef>(iu);
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<Expression> SemanticAnalysis::analyseLiteral(parser::Literal& literal) {
   auto literalText = std::string(literal.getTextVal());
   switch (literal.getLiteralType()) {
      case Literal::INT: return std::make_unique<Const>(std::move(literalText), schemac::Type::Integer());
      case Literal::NUMERIC: return std::make_unique<Const>(std::move(literalText), schemac::Type::Numeric(0, 0));
      case Literal::STRING: return std::make_unique<Const>(std::move(literalText));
      case Literal::TRUE: return std::make_unique<Const>("1", schemac::Type::Bool());
      case Literal::FALSE: return std::make_unique<Const>("0", schemac::Type::Bool());
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
std::unique_ptr<Expression> SemanticAnalysis::analyseExpression(parser::AST& ast) {
   switch (ast.getASTType()) {
      case AST::UnaryExpressionType:
         return analyseUnaryExpression(*UnaryExpression::dynCast(&ast));
      case AST::BinaryExpressionType:
         return analyseBinaryExpression(*BinaryExpression::dynCast(&ast));
      case AST::BetweenExpressionType:
         return analyseBetweenExpression(*BetweenExpression::dynCast(&ast));
      case AST::ColIdType:
         return analyseColId(*ColId::dynCast(&ast));
      case AST::LiteralType:
         return analyseLiteral(*Literal::dynCast(&ast));
      default:
         throw RuntimeException(RuntimeError, "Invalid AST encountered.");
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::parser
// ---------------------------------------------------------------------------------------------------
