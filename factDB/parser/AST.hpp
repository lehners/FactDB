#ifndef H_FACT_DB_PARSER_AST_H_
#define H_FACT_DB_PARSER_AST_H_
// ---------------------------------------------------------------------------------------------------
#include "factDB/schemac/Table.hpp"
#include <memory>
// ---------------------------------------------------------------------------------------------------
namespace factDB::parser {
// ---------------------------------------------------------------------------------------------------
class AST {
   public:
   enum ASTType {
      CreateTableType,
      CopyTableType,
      LiteralType,
      IdentifierType,
      ColIdType,
      BinaryExpressionType,
      BetweenExpressionType,
      RelationExprType,
      TableRefType,
      TargetType,
      SelectType,
      AliasType,
      UnaryExpressionType,
      CountStarType
   };

   private:
   ASTType type;

   protected:
   constexpr explicit AST(ASTType typeParam) : type(typeParam) {};

   public:
   // AST() = default;
   virtual ~AST() = default;

   /// get the type
   [[nodiscard]] ASTType getASTType() const { return type; }

   template <AST::ASTType type, std::derived_from<AST> T>
   static T* dynCastTemplate(std::conditional_t<std::is_const_v<T>, const AST, AST>* ast) {
      return (ast && ast->getASTType() == type) ? static_cast<T*>(ast) : nullptr;
   }
};
// ---------------------------------------------------------------------------------------------------
class Target;
class TableRef;
// ---------------------------------------------------------------------------------------------------
class Select : public AST {
   private:
   /// select clause (class Target)
   std::vector<std::unique_ptr<AST>> targets;
   /// from clause (class TableRef)
   std::vector<std::unique_ptr<AST>> from;
   /// where clause (class Expression)
   std::unique_ptr<AST> where;
   /// group by clause (class Expression)
   std::vector<std::unique_ptr<AST>> groupbys;
   /// having clause (class Expression)
   std::unique_ptr<AST> having;

   public:
   /// Constructor
   Select(std::vector<std::unique_ptr<AST>> targetsP, std::vector<std::unique_ptr<AST>> fromP, std::unique_ptr<AST> whereP)
      : AST(SelectType), targets(std::move(targetsP)), from(std::move(fromP)), where(std::move(whereP)) {}
   /// Destructor
   ~Select() override = default;

   /// Return the targets (is empty if * is set)
   [[nodiscard]] std::vector<Target*> getTargets() const;
   /// Return from as pointers to TableRef
   [[nodiscard]] std::vector<TableRef*> getFrom() const;
   /// Return where
   [[nodiscard]] AST* getWhere() const { return where.get(); }

   static auto* dynCast(AST* ast) { return dynCastTemplate<SelectType, Select>(ast); }
   static auto* dynCast(const AST* ast) { return dynCastTemplate<SelectType, const Select>(ast); }
};
// ---------------------------------------------------------------------------------------------------
class Target : public AST {
   private:
   /// expression (class Expression)
   std::unique_ptr<AST> expression;
   /// alias name (class ColId)
   std::unique_ptr<AST> name;

   public:
   Target(std::unique_ptr<AST> value, std::unique_ptr<AST> nameP) : AST(TargetType), expression(std::move(value)), name(std::move(nameP)) {}
   ~Target() override = default;

   [[nodiscard]] AST* getExpression() const { return expression.get(); }
   [[nodiscard]] AST* getName() const { return name.get(); }

   static auto* dynCast(AST* ast) { return dynCastTemplate<TargetType, Target>(ast); }
   static auto* dynCast(const AST* ast) { return dynCastTemplate<TargetType, const Target>(ast); }
};
// ---------------------------------------------------------------------------------------------------
class CountStar : public AST {
   public:
   CountStar() : AST(CountStarType) {}
   ~CountStar() override = default;

   static auto* dynCast(AST* ast) { return dynCastTemplate<CountStarType, CountStar>(ast); }
   static auto* dynCast(const AST* ast) { return dynCastTemplate<CountStarType, const CountStar>(ast); }
};
// ---------------------------------------------------------------------------------------------------
class TableRef : public AST {
   public:
   enum class SubType : unsigned { Relation,
                                   Joined,
                                   Subselect };

   private:
   /// value (class RelationExpr)
   std::unique_ptr<AST> input;
   /// alias (class Alias)
   std::unique_ptr<AST> alias;
   /// subtype
   SubType subType;

   public:
   TableRef(SubType subTypeP, std::unique_ptr<AST> inputP, std::unique_ptr<AST> aliasP)
      : AST(TableRefType), input(std::move(inputP)), alias(std::move(aliasP)), subType(subTypeP) {}
   /// Destructor
   ~TableRef() override = default;

   /// Get the table name
   [[nodiscard]] AST* getInput() const { return input.get(); }
   /// Get the alias
   [[nodiscard]] AST* getAlias() const { return alias.get(); }
   /// Get the subtype
   [[nodiscard]] SubType getSubType() const { return subType; }

   static auto* dynCast(AST* ast) { return dynCastTemplate<TableRefType, TableRef>(ast); }
   static auto* dynCast(const AST* ast) { return dynCastTemplate<TableRefType, const TableRef>(ast); }
};
// ---------------------------------------------------------------------------------------------------
class Alias : public AST {
   private:
   /// the alias
   std::string alias;

   public:
   /// Constructor
   explicit Alias(std::string aliasP) : AST(AliasType), alias(std::move(aliasP)) {};
   /// Destructor
   ~Alias() override = default;

   [[nodiscard]] std::string_view getAliasValue() const { return alias; }

   static auto* dynCast(AST* ast) { return dynCastTemplate<AliasType, Alias>(ast); }
   static auto* dynCast(const AST* ast) { return dynCastTemplate<AliasType, const Alias>(ast); }
};
// ---------------------------------------------------------------------------------------------------
class RelationExpr : public AST {
   private:
   std::unique_ptr<AST> input;

   public:
   explicit RelationExpr(std::unique_ptr<AST> inputP) : AST(RelationExprType), input(std::move(inputP)) {};
   ~RelationExpr() override = default;

   [[nodiscard]] AST* getInput() const { return input.get(); }

   static auto* dynCast(AST* ast) { return dynCastTemplate<RelationExprType, RelationExpr>(ast); }
   static auto* dynCast(const AST* ast) { return dynCastTemplate<RelationExprType, const RelationExpr>(ast); }
};
// ---------------------------------------------------------------------------------------------------
class UnaryExpression : public AST {
   public:
   /// Type of the UnaryExpression
   enum SubType {
      UMINUS,
      UPLUS,
      NOT,
   };

   private:
   /// op type
   SubType op;
   /// expression
   std::unique_ptr<AST> expr;

   public:
   UnaryExpression(SubType op_, std::unique_ptr<AST> expr_) : AST(UnaryExpressionType), op(op_), expr(std::move(expr_)) {}
   ~UnaryExpression() override = default;

   [[nodiscard]] SubType getSubType() const { return op; }
   [[nodiscard]] AST* getExpression() const { return expr.get(); }

   static auto* dynCast(AST* ast) { return dynCastTemplate<UnaryExpressionType, UnaryExpression>(ast); }
   static auto* dynCast(const AST* ast) { return dynCastTemplate<UnaryExpressionType, const UnaryExpression>(ast); }
};
// ---------------------------------------------------------------------------------------------------
class BinaryExpression : public AST {
   public:
   /// Type of the BinaryExpression
   enum SubType {
      PLUS,
      MINUS,
      MUL,
      DIV,
      MOD,
      POW,
      LT,
      GT,
      EQ,
      LE,
      GE,
      NEQ,
      AND,
      OR,
   };

   private:
   /// type of the BinaryExpression
   SubType op;
   /// left expression
   std::unique_ptr<AST> left;
   /// right expression
   std::unique_ptr<AST> right;

   public:
   BinaryExpression(SubType opT, std::unique_ptr<AST> leftT, std::unique_ptr<AST> rightT)
      : AST(BinaryExpressionType), op(opT), left(std::move(leftT)), right(std::move(rightT)) {}
   ~BinaryExpression() override = default;

   [[nodiscard]] SubType getOp() const { return op; }
   [[nodiscard]] AST* getLeft() const { return left.get(); }
   [[nodiscard]] AST* getRight() const { return right.get(); }

   static auto* dynCast(AST* ast) { return dynCastTemplate<BinaryExpressionType, BinaryExpression>(ast); }
   static auto* dynCast(const AST* ast) { return dynCastTemplate<BinaryExpressionType, const BinaryExpression>(ast); }
};
// ---------------------------------------------------------------------------------------------------
class BetweenExpression : public AST {
   public:
   private:
   /// value to check if in range
   std::unique_ptr<AST> val;
   /// lower expression
   std::unique_ptr<AST> lower;
   /// upper expression
   std::unique_ptr<AST> upper;

   public:
   BetweenExpression(std::unique_ptr<AST> valT, std::unique_ptr<AST> lowerT, std::unique_ptr<AST> upperT)
      : AST(BetweenExpressionType), val(std::move(valT)), lower(std::move(lowerT)), upper(std::move(upperT)) {}
   ~BetweenExpression() override = default;

   [[nodiscard]] AST* getVal() const { return val.get(); }
   [[nodiscard]] AST* getLower() const { return lower.get(); }
   [[nodiscard]] AST* getUpper() const { return upper.get(); }

   static auto* dynCast(AST* ast) { return dynCastTemplate<BetweenExpressionType, BetweenExpression>(ast); }
   static auto* dynCast(const AST* ast) { return dynCastTemplate<BetweenExpressionType, const BetweenExpression>(ast); }
};
// ---------------------------------------------------------------------------------------------------
class ColId : public AST {
   private:
   std::unique_ptr<AST> table;
   std::unique_ptr<AST> column;

   public:
   ColId(std::unique_ptr<AST> tableParam, std::unique_ptr<AST> columnParam) : AST(ColIdType), table(std::move(tableParam)), column(std::move(columnParam)) {}
   ~ColId() override = default;

   AST* getTable() { return table.get(); }
   AST* getColumn() { return column.get(); }

   static auto* dynCast(AST* ast) { return dynCastTemplate<ColIdType, ColId>(ast); }
   static auto* dynCast(const AST* ast) { return dynCastTemplate<ColIdType, const ColId>(ast); }
};
// ---------------------------------------------------------------------------------------------------
struct Identifier : public AST {
   private:
   std::string identifier;
   bool quoted;

   public:
   Identifier(std::string identifierParam, bool quotedParam) : AST(IdentifierType), identifier(std::move(identifierParam)), quoted(quotedParam) {}
   ~Identifier() override = default;

   [[nodiscard]] std::string_view getIdentifierValue() const { return identifier; }
   [[nodiscard]] const std::string* getIdentifierPtr() const { return &identifier; }
   [[nodiscard]] bool getQuoted() const { return quoted; }

   static auto* dynCast(AST* ast) { return dynCastTemplate<IdentifierType, Identifier>(ast); }
   static auto* dynCast(const AST* ast) { return dynCastTemplate<IdentifierType, const Identifier>(ast); }
};
// ---------------------------------------------------------------------------------------------------
struct Literal : public AST {
   enum SubType {
      INT,
      NUMERIC,
      STRING,
      TRUE,
      FALSE
   };

   private:
   SubType literalType;
   std::string val;

   public:
   Literal(SubType op, std::string text) : AST(LiteralType), literalType(op), val(std::move(text)) {}
   ~Literal() override = default;

   [[nodiscard]] SubType getLiteralType() const { return literalType; }
   [[nodiscard]] std::string_view getTextVal() const { return val; }

   static auto* dynCast(AST* ast) { return dynCastTemplate<LiteralType, Literal>(ast); }
   static auto* dynCast(const AST* ast) { return dynCastTemplate<LiteralType, const Literal>(ast); }
};
// ---------------------------------------------------------------------------------------------------
struct CreateTable : public AST {
   private:
   schemac::Table table;

   public:
   explicit CreateTable(schemac::Table t) : AST(CreateTableType), table(std::move(t)) {}
   ~CreateTable() override = default;

   [[nodiscard]] const schemac::Table& getTable() const { return table; }

   static auto* dynCast(AST* ast) { return dynCastTemplate<CreateTableType, CreateTable>(ast); }
   static auto* dynCast(const AST* ast) { return dynCastTemplate<CreateTableType, const CreateTable>(ast); }
};
// ---------------------------------------------------------------------------------------------------
struct CopyTable : public AST {
   std::string table;
   std::string file;
   std::string delimiter;

   public:
   CopyTable(std::string tbl, std::string filename, std::string separator = ",") : AST(CopyTableType), table(std::move(tbl)), file(std::move(filename)), delimiter(std::move(separator)) {}
   ~CopyTable() override = default;

   static auto* dynCast(AST* ast) { return dynCastTemplate<CopyTableType, CopyTable>(ast); }
   static auto* dynCast(const AST* ast) { return dynCastTemplate<CopyTableType, const CopyTable>(ast); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::parser
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_PARSER_AST_H_