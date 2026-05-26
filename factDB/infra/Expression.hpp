#ifndef H_factdb_infra_predicate
#define H_factdb_infra_predicate

#include "factDB/schemac/Type.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace factDB {

class IU;
struct IUSet;
struct IUPointerHash;
struct IUPointerEqual;
struct RuntimeValue;

class Expression {
   public:
   enum ExpressionType {
      CompareL,
      CompareLE,
      CompareEq,
      CompareGE,
      CompareG,

      IURef,
      Const,
      AndExpression,
      OrExpression,
      JoinCondition,
      JoinConditionList,
      Reference,
   };

   private:
   const ExpressionType type;

   protected:
   using IUMap = std::unordered_map<const IU*, size_t, IUPointerHash, IUPointerEqual>;
   explicit Expression(ExpressionType t) : type(t) {}

   template <ExpressionType type, std::derived_from<Expression> T>
   static T* dynCastTemplate(std::conditional_t<std::is_const_v<T>, const Expression, Expression>* ptr) {
      return (ptr && ptr->get_type() == type) ? static_cast<T*>(ptr) : nullptr;
   }

   public:
   [[nodiscard]] ExpressionType get_type() const { return type; };
   virtual ~Expression() = default;

   virtual std::unique_ptr<Expression> copy(const IUSet& ius) const = 0;

   [[nodiscard]] IUSet collectIUs() const;
   [[nodiscard]] RuntimeValue evaluate(const std::vector<RuntimeValue>& values, const IUMap& iuMapping) const;

   [[nodiscard]] const std::string type_string() const {
      switch (type) {
         case CompareL: return "CompareL";
         case CompareLE: return "CompareLE";
         case CompareEq: return "CompareEq";
         case CompareGE: return "CompareGE";
         case CompareG: return "CompareG";
         case IURef: return "IURef";
         case Const: return "Const";
         case AndExpression: return "AndExpression";
         case OrExpression: return "OrExpression";
         case JoinCondition: return "JoinCondition";
         case JoinConditionList: return "JoinConditionList";
         case Reference: return "ReferenceExpression";
         default: return "unreachable";
      }
   }

   [[nodiscard]] static ExpressionType type_from_string(const std::string& str) {
      switch (str[0]) {
         case 'A':
            if (str == "AndExpression")
               return AndExpression;
            else
               throw std::runtime_error("invalid expression Type");
         case 'C':
            if (str == "Const")
               return Const;
            else if (str.starts_with("Compare")) {
               if (str == "CompareL")
                  return CompareL;
               else if (str == "CompareLE")
                  return CompareLE;
               else if (str == "CompareEq")
                  return CompareEq;
               else if (str == "CompareGE")
                  return CompareGE;
               else if (str == "CompareG")
                  return CompareG;
               else
                  throw std::runtime_error("invalid expression Type");
            } else
               throw std::runtime_error("invalid expression Type");
         case 'I':
            if (str == "IURef")
               return IURef;
            else
               throw std::runtime_error("invalid expression Type");
         case 'J':
            if (str == "JoinCondition")
               return JoinCondition;
            else if (str == "JoinConditionList")
               return JoinConditionList;
            else
               throw std::runtime_error("invalid expression Type");
         case 'O':
            if (str == "OrExpression")
               return OrExpression;
            else
               throw std::runtime_error("invalid expression Type");
         default:
            throw std::runtime_error("invalid expression Type");
      }
   }
};

using ExpressionPtr = std::unique_ptr<Expression>;

class Const final : public Expression {
   const std::string value;
   const schemac::Type type;

   public:
   explicit Const(const char* value_) : Expression(ExpressionType::Const), value(value_) {};
   explicit Const(std::string value_) : Expression(ExpressionType::Const), value(std::move(value_)) {};
   explicit Const(std::string value_, schemac::Type type_) : Expression(ExpressionType::Const), value(std::move(value_)), type(type_) {};
   ~Const() override = default;
   [[nodiscard]] const std::string get_value() const { return value; }
   [[nodiscard]] const schemac::Type getConstType() const { return type; }

   [[nodiscard]] IUSet collectIUs() const;
   [[nodiscard]] RuntimeValue evaluate(const std::vector<RuntimeValue>& values, const IUMap& iuMapping) const;

   [[nodiscard]] static ExpressionPtr create(const char* val) { return std::make_unique<factDB::Const>(val); }
   [[nodiscard]] static ExpressionPtr create(const std::string& val) { return std::make_unique<factDB::Const>(val.c_str()); }
   [[nodiscard]] ExpressionPtr copy(const IUSet&) const override { return std::make_unique<Const>(value, type); }
};

class IURef final : public Expression {
   const IU* iu;

   friend class JoinCondition;

   public:
   explicit IURef(const IU* _iu) : Expression(ExpressionType::IURef), iu(_iu) {};
   ~IURef() override = default;
   [[nodiscard]] const IU& get_iu() const { return *iu; }

   [[nodiscard]] IUSet collectIUs() const;
   [[nodiscard]] RuntimeValue evaluate(const std::vector<RuntimeValue>& values, const IUMap& iuMapping) const;

   [[nodiscard]] static ExpressionPtr create(const IU* _iu) { return std::make_unique<factDB::IURef>(_iu); }
   [[nodiscard]] static ExpressionPtr create(const IU& _iu) { return std::make_unique<factDB::IURef>(&_iu); }

   [[nodiscard]] static auto dynCast(Expression* expr) { return dynCastTemplate<ExpressionType::IURef, IURef>(expr); }
   [[nodiscard]] static auto dynCast(const Expression* expr) { return dynCastTemplate<ExpressionType::IURef, const IURef>(expr); }
   [[nodiscard]] ExpressionPtr copy(const IUSet& ius) const override;
};

class BinaryExpression : public Expression {
   protected:
   ExpressionPtr left;
   ExpressionPtr right;
   BinaryExpression(ExpressionType _type, ExpressionPtr&& _left, ExpressionPtr&& _right) : Expression(_type), left(std::move(_left)), right(std::move(_right)) {}

   public:
   ~BinaryExpression() override = default;
   [[nodiscard]] const Expression& get_left() const { return *left.get(); }
   [[nodiscard]] const Expression& get_right() const { return *right.get(); }
   [[nodiscard]] ExpressionPtr releaseLeft() { return std::move(left); }
   [[nodiscard]] ExpressionPtr releaseRight() { return std::move(right); }
   [[nodiscard]] IUSet collectIUs() const;
   [[nodiscard]] RuntimeValue evaluate(const std::vector<RuntimeValue>& values, const IUMap& iuMapping) const;

   [[nodiscard]] virtual std::string getComparator() const = 0;
};

class CompareLess : public BinaryExpression {
   public:
   CompareLess(ExpressionPtr&& _left, ExpressionPtr&& _right) : BinaryExpression(ExpressionType::CompareL, std::move(_left), std::move(_right)) {};
   ~CompareLess() = default;

   [[nodiscard]] std::string getComparator() const override { return "<"; }

   [[nodiscard]] static ExpressionPtr create(ExpressionPtr&& _left, ExpressionPtr&& _right) { return std::make_unique<factDB::CompareLess>(std::move(_left), std::move(_right)); }
   [[nodiscard]] static ExpressionPtr create(const IU* _left, const IU* _right) { return std::make_unique<factDB::CompareLess>(IURef::create(_left), IURef::create(_right)); }
   [[nodiscard]] static ExpressionPtr create(const IU* _left, const char* _right) { return std::make_unique<factDB::CompareLess>(IURef::create(_left), Const::create(_right)); }
   [[nodiscard]] ExpressionPtr copy(const IUSet& ius) const override { return std::make_unique<CompareLess>(left->copy(ius), right->copy(ius)); }
};

class CompareLessEqual : public BinaryExpression {
   public:
   CompareLessEqual(ExpressionPtr&& _left, ExpressionPtr&& _right) : BinaryExpression(ExpressionType::CompareLE, std::move(_left), std::move(_right)) {};
   ~CompareLessEqual() = default;

   [[nodiscard]] std::string getComparator() const override { return "<="; }

   [[nodiscard]] static ExpressionPtr create(ExpressionPtr&& _left, ExpressionPtr&& _right) { return std::make_unique<factDB::CompareLessEqual>(std::move(_left), std::move(_right)); }
   [[nodiscard]] static ExpressionPtr create(const IU* _left, const IU* _right) { return std::make_unique<factDB::CompareLessEqual>(IURef::create(_left), IURef::create(_right)); }
   [[nodiscard]] static ExpressionPtr create(const IU* _left, const char* _right) { return std::make_unique<factDB::CompareLessEqual>(IURef::create(_left), Const::create(_right)); }
   [[nodiscard]] ExpressionPtr copy(const IUSet& ius) const override { return std::make_unique<CompareLessEqual>(left->copy(ius), right->copy(ius)); }
};

class CompareEqual : public BinaryExpression {
   public:
   CompareEqual(ExpressionPtr&& _left, ExpressionPtr&& _right) : BinaryExpression(ExpressionType::CompareEq, std::move(_left), std::move(_right)) {};
   ~CompareEqual() = default;

   [[nodiscard]] std::string getComparator() const override { return "=="; }

   [[nodiscard]] static ExpressionPtr create(ExpressionPtr&& _left, ExpressionPtr&& _right) { return std::make_unique<factDB::CompareEqual>(std::move(_left), std::move(_right)); }
   [[nodiscard]] static ExpressionPtr create(const IU* _left, const IU* _right) { return std::make_unique<factDB::CompareEqual>(IURef::create(_left), IURef::create(_right)); }
   [[nodiscard]] static ExpressionPtr create(const IU* _left, const char* _right) { return std::make_unique<factDB::CompareEqual>(IURef::create(_left), Const::create(_right)); }
   [[nodiscard]] static ExpressionPtr create(const IU* _left, int _right) { return std::make_unique<factDB::CompareEqual>(IURef::create(_left), std::make_unique<factDB::Const>(std::to_string(_right), schemac::Type::Integer())); }

   [[nodiscard]] static auto dynCast(Expression* expr) { return dynCastTemplate<ExpressionType::CompareEq, CompareEqual>(expr); }
   [[nodiscard]] static auto dynCast(const Expression* expr) { return dynCastTemplate<ExpressionType::CompareEq, const CompareEqual>(expr); }
   [[nodiscard]] ExpressionPtr copy(const IUSet& ius) const override { return std::make_unique<CompareEqual>(left->copy(ius), right->copy(ius)); }
};

class CompareGreaterEqual : public BinaryExpression {
   public:
   CompareGreaterEqual(ExpressionPtr&& _left, ExpressionPtr&& _right) : BinaryExpression(ExpressionType::CompareGE, std::move(_left), std::move(_right)) {};
   ~CompareGreaterEqual() = default;

   [[nodiscard]] std::string getComparator() const override { return ">="; }

   [[nodiscard]] static ExpressionPtr create(ExpressionPtr&& _left, ExpressionPtr&& _right) { return std::make_unique<factDB::CompareGreaterEqual>(std::move(_left), std::move(_right)); }
   [[nodiscard]] static ExpressionPtr create(const IU* _left, const IU* _right) { return std::make_unique<factDB::CompareGreaterEqual>(IURef::create(_left), IURef::create(_right)); }
   [[nodiscard]] static ExpressionPtr create(const IU* _left, const char* _right) { return std::make_unique<factDB::CompareGreaterEqual>(IURef::create(_left), Const::create(_right)); }
   [[nodiscard]] ExpressionPtr copy(const IUSet& ius) const override { return std::make_unique<CompareGreaterEqual>(left->copy(ius), right->copy(ius)); }
};

class CompareGreater : public BinaryExpression {
   public:
   CompareGreater(ExpressionPtr&& _left, ExpressionPtr&& _right) : BinaryExpression(ExpressionType::CompareG, std::move(_left), std::move(_right)) {};
   ~CompareGreater() = default;

   [[nodiscard]] std::string getComparator() const override { return ">"; }

   [[nodiscard]] static ExpressionPtr create(ExpressionPtr&& _left, ExpressionPtr&& _right) { return std::make_unique<factDB::CompareGreater>(std::move(_left), std::move(_right)); }
   [[nodiscard]] static ExpressionPtr create(const IU* _left, const IU* _right) { return std::make_unique<factDB::CompareGreater>(IURef::create(_left), IURef::create(_right)); }
   [[nodiscard]] static ExpressionPtr create(const IU* _left, const char* _right) { return std::make_unique<factDB::CompareGreater>(IURef::create(_left), Const::create(_right)); }
   [[nodiscard]] ExpressionPtr copy(const IUSet& ius) const override { return std::make_unique<CompareGreater>(left->copy(ius), right->copy(ius)); }
};

class AndExpression : public BinaryExpression {
   public:
   AndExpression(ExpressionPtr&& _left, ExpressionPtr&& _right) : BinaryExpression(ExpressionType::AndExpression, std::move(_left), std::move(_right)) {}
   ~AndExpression() = default;

   [[nodiscard]] std::string getComparator() const override { return "&&"; }

   [[nodiscard]] static ExpressionPtr create(ExpressionPtr&& _left, ExpressionPtr&& _right) { return std::make_unique<factDB::AndExpression>(std::move(_left), std::move(_right)); }
   [[nodiscard]] static auto dynCast(Expression* expr) { return dynCastTemplate<ExpressionType::AndExpression, AndExpression>(expr); }
   [[nodiscard]] static auto dynCast(const Expression* expr) { return dynCastTemplate<ExpressionType::AndExpression, const AndExpression>(expr); }
   [[nodiscard]] ExpressionPtr copy(const IUSet& ius) const override { return std::make_unique<AndExpression>(left->copy(ius), right->copy(ius)); }
};

class OrExpression : public BinaryExpression {
   public:
   OrExpression(ExpressionPtr&& _left, ExpressionPtr&& _right) : BinaryExpression(ExpressionType::OrExpression, std::move(_left), std::move(_right)) {}
   ~OrExpression() = default;

   [[nodiscard]] std::string getComparator() const override { return "||"; }

   [[nodiscard]] static ExpressionPtr create(ExpressionPtr&& _left, ExpressionPtr&& _right) { return std::make_unique<factDB::OrExpression>(std::move(_left), std::move(_right)); }
   [[nodiscard]] ExpressionPtr copy(const IUSet& ius) const override { return std::make_unique<OrExpression>(left->copy(ius), right->copy(ius)); }
};

// Helper class function to simplify join predicates
class JoinCondition : public Expression {
   private:
   factDB::IURef left;
   factDB::IURef right;

   public:
   JoinCondition(const factDB::IURef&& _left, const factDB::IURef&& _right) : Expression(ExpressionType::JoinCondition), left(std::move(_left)), right(std::move(_right)) {}
   ~JoinCondition() = default;

   [[nodiscard]] const IU& get_left() const { return left.get_iu(); }
   [[nodiscard]] const IU& get_right() const { return right.get_iu(); }

   void swap() {
      std::swap(left.iu, right.iu);
   }

   [[nodiscard]] IUSet collectIUs() const;
   [[nodiscard]] RuntimeValue evaluate(const std::vector<RuntimeValue>& values, const IUMap& iuMapping) const;

   [[nodiscard]] static std::unique_ptr<JoinCondition> create(const factDB::IURef& _left, const factDB::IURef& _right) {
      return std::make_unique<factDB::JoinCondition>(std::move(_left), std::move(_right));
   }
   [[nodiscard]] static JoinCondition create(const IU* _left, const IU* _right) {
      return {factDB::IURef(_left), factDB::IURef(_right)};
   }

   [[nodiscard]] static auto dynCast(Expression* expr) { return dynCastTemplate<ExpressionType::JoinCondition, JoinCondition>(expr); }
   [[nodiscard]] static auto dynCast(const Expression* expr) { return dynCastTemplate<ExpressionType::JoinCondition, const JoinCondition>(expr); }
   [[nodiscard]] ExpressionPtr copy(const IUSet&) const override { not_implemented(); }
   bool operator==(const JoinCondition& other) const;
};

class JoinConditionList : public Expression {
   std::vector<factDB::JoinCondition> conditions;

   public:
   JoinConditionList() : Expression(ExpressionType::JoinConditionList) {}
   JoinConditionList(std::initializer_list<factDB::JoinCondition> initializer_list) : Expression(ExpressionType::JoinConditionList), conditions{initializer_list} {}
   JoinConditionList(std::vector<factDB::JoinCondition> _conditions) : Expression(ExpressionType::JoinConditionList), conditions(std::move(_conditions)) {}
   ~JoinConditionList() = default;

   [[nodiscard]] std::vector<factDB::JoinCondition>& get_conditions() { return conditions; }
   [[nodiscard]] const std::vector<factDB::JoinCondition>& get_conditions() const { return conditions; }
   void insert(JoinConditionList& other);
   void insert(Expression& other);
   [[nodiscard]] IUSet collectIUs() const;
   [[nodiscard]] RuntimeValue evaluate(const std::vector<RuntimeValue>& values, const IUMap& iuMapping) const;

   [[nodiscard]] static std::unique_ptr<JoinConditionList> create(std::initializer_list<factDB::JoinCondition> initializer_list) { return std::make_unique<JoinConditionList>(std::move(initializer_list)); }
   [[nodiscard]] static std::unique_ptr<JoinConditionList> create(const IU* _left, const IU* _right) { return std::make_unique<JoinConditionList>(factDB::JoinConditionList({factDB::JoinCondition::create(_left, _right)})); }

   [[nodiscard]] auto begin() const { return conditions.begin(); }
   [[nodiscard]] auto end() const { return conditions.end(); }
   [[nodiscard]] auto size() const { return conditions.size(); }
   [[nodiscard]] const auto& get(size_t index) const { return conditions[index]; }

   [[nodiscard]] static auto dynCast(Expression* expr) { return dynCastTemplate<ExpressionType::JoinConditionList, JoinConditionList>(expr); }
   [[nodiscard]] static auto dynCast(const Expression* expr) { return dynCastTemplate<ExpressionType::JoinConditionList, const JoinConditionList>(expr); }
   [[nodiscard]] ExpressionPtr copy(const IUSet&) const override { not_implemented(); }
};

class ReferenceExpression : public Expression {
   const Expression& reference;

   public:
   ReferenceExpression(const Expression& expression) : Expression(Reference), reference(expression) {}
   ~ReferenceExpression() override = default;

   const Expression& getReferencedExpr() const { return reference; }

   [[nodiscard]] IUSet collectIUs() const;
   [[nodiscard]] RuntimeValue evaluate(const std::vector<RuntimeValue>& values, const IUMap& iuMapping) const;

   [[nodiscard]] static ExpressionPtr create(const Expression& expr) { return std::make_unique<ReferenceExpression>(expr); }

   [[nodiscard]] static auto dynCast(Expression* expr) { return dynCastTemplate<ExpressionType::Reference, ReferenceExpression>(expr); }
   [[nodiscard]] static auto dynCast(const Expression* expr) { return dynCastTemplate<ExpressionType::Reference, const ReferenceExpression>(expr); }
   std::unique_ptr<Expression> copy(const IUSet&) const override { not_implemented(); }
};

} // namespace factDB

#endif // H_factdb_infra_predicate
