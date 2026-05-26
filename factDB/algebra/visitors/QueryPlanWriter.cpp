#include "factDB/algebra/visitors/QueryPlanWriter.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
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
#include <stack>

namespace factDB::algebra::visitors {

QueryPlanWriter::QueryPlanWriter(const Database& db, std::ostream& writer)
   : AlgebraVisitorDB(db), out(writer) {
}

struct JsonHelper {
   using json = nlohmann::json;

   private:
   static json print_iu(const IU& iu, json& iu_container) {
      iu_container["table"] = iu.table;
      iu_container["column"] = iu.column;
      iu_container["Type"] = iu.type.toString();
      return iu_container;
   }

   static std::tuple<std::string, std::string, factDB::schemac::Type> read_iu(json::reference& iu_json) {
      std::string table = iu_json["table"];
      std::string column = iu_json["column"];
      auto type = factDB::schemac::Type::fromString(iu_json["Type"]);
      return {table, column, type};
   }

   public:
   static json print_iu(const IU& iu) {
      json iu_container;
      return print_iu(iu, iu_container);
   }

   static const IU* find_iu(json::reference& iu_json, const OrderedIUSet& ius) {
      const auto iu_vals = read_iu(iu_json);
      const IU iu(std::get<0>(iu_vals), std::get<1>(iu_vals), std::get<2>(iu_vals));
      assert(ius.find(iu) != nullptr);
      return ius.find(iu);
   }

   public:
   static json print_expression(const Expression& expression) {
      json container;
      std::stack<std::pair<const Expression*, json::reference>> todo;
      todo.emplace(&expression, container);

      while (!todo.empty()) {
         auto [cur_expr, cur_json] = todo.top();
         cur_json["expression"] = cur_expr->type_string();
         todo.pop();
         switch (cur_expr->get_type()) {
            case Expression::IURef:
               print_iu(static_cast<const IURef*>(cur_expr)->get_iu(), cur_json);
               break;
            case Expression::Const: {
               const auto& constExp = *static_cast<const Const*>(cur_expr);
               cur_json["value"] = constExp.get_value();
               cur_json["type"] = constExp.getConstType().toString();
            } break;
            case Expression::AndExpression:
            case Expression::OrExpression:
            case Expression::CompareL:
            case Expression::CompareLE:
            case Expression::CompareEq:
            case Expression::CompareGE:
            case Expression::CompareG:
               cur_json["comparator"] = static_cast<const BinaryExpression*>(cur_expr)->getComparator();
               todo.emplace(&static_cast<const BinaryExpression*>(cur_expr)->get_left(), cur_json["left"]);
               todo.emplace(&static_cast<const BinaryExpression*>(cur_expr)->get_right(), cur_json["right"]);
               break;
            case Expression::JoinCondition:
               cur_json["left"] = print_iu(static_cast<const JoinCondition*>(cur_expr)->get_left());
               cur_json["right"] = print_iu(static_cast<const JoinCondition*>(cur_expr)->get_right());
               break;
            case Expression::JoinConditionList: {
               auto& condition_list = *static_cast<const JoinConditionList*>(cur_expr);
               for (size_t i = 0; i < condition_list.size(); i++) {
                  cur_json["conditions"].push_back(nlohmann::json::object());
               }
               for (const auto& [l_cond, l_json] : views::zip(condition_list, cur_json["conditions"])) {
                  todo.emplace(&l_cond, l_json);
               }
               break;
            }
            case Expression::Reference:
               print_expression(static_cast<const ReferenceExpression&>(expression).getReferencedExpr());
         }
      }
      return container;
   }

   static ExpressionPtr read_predicate(json::reference& json_expr, const OrderedIUSet& ius) {
      switch (Expression::type_from_string(json_expr["expression"])) {
         case Expression::IURef:
            return IURef::create(find_iu(json_expr, ius));
         case Expression::Const: {
            auto expr = std::make_unique<Const>(json_expr["value"], schemac::Type::fromString(json_expr["type"]));
            return expr;
         }
         case Expression::AndExpression:
            return AndExpression::create(read_predicate(json_expr["left"], ius), read_predicate(json_expr["right"], ius));
         case Expression::OrExpression:
            return OrExpression::create(read_predicate(json_expr["left"], ius), read_predicate(json_expr["right"], ius));
         case Expression::CompareL: {
            auto left = read_predicate(json_expr["left"], ius);
            auto right = read_predicate(json_expr["right"], ius);
            return CompareLess::create(std::move(left), std::move(right));
         }
         case Expression::CompareLE: {
            auto left = read_predicate(json_expr["left"], ius);
            auto right = read_predicate(json_expr["right"], ius);
            return CompareLessEqual::create(std::move(left), std::move(right));
         }
         case Expression::CompareEq: {
            auto left = read_predicate(json_expr["left"], ius);
            auto right = read_predicate(json_expr["right"], ius);
            return CompareEqual::create(std::move(left), std::move(right));
         }
         case Expression::CompareGE: {
            auto left = read_predicate(json_expr["left"], ius);
            auto right = read_predicate(json_expr["right"], ius);
            return CompareGreaterEqual::create(std::move(left), std::move(right));
         }
         case Expression::CompareG: {
            auto left = read_predicate(json_expr["left"], ius);
            auto right = read_predicate(json_expr["right"], ius);
            return CompareGreater::create(std::move(left), std::move(right));
         }
         case Expression::JoinCondition:
            assert(false && "Type JoinCondition cannot occur in read_predicate.");
         case Expression::JoinConditionList: {
            std::vector<JoinCondition> conditions;
            conditions.reserve(json_expr["conditions"].size());
            for (auto& cond : json_expr["conditions"]) {
               assert(Expression::type_from_string(cond["expression"]) == Expression::JoinCondition);
               const IU* left_iu = find_iu(cond["left"], ius);
               const IU* right_iu = find_iu(cond["right"], ius);
               conditions.push_back(JoinCondition::create(left_iu, right_iu));
            }
            return std::make_unique<JoinConditionList>(std::move(conditions));
         }
         case Expression::Reference:
            unreachable();
      }
      unreachable();
   }
};

void QueryPlanWriter::visitProduce(const InnerJoin& join) {
   nlohmann::json new_level;
   new_level["operator"] = join.typeToString();
   new_level["joinCondition"] = JsonHelper::print_expression(join.getJoinCondition());
   join.getLeftChild()->produce(*this);
   new_level["leftChild"] = cur_level;
   cur_level = nlohmann::json();
   join.getRightChild()->produce(*this);
   new_level["rightChild"] = cur_level;
   cur_level = new_level;
}

void QueryPlanWriter::visitProduce(const Print& print) {
   nlohmann::json new_level;
   new_level["operator"] = print.typeToString();
   for (const auto& col : print.getRequiredIus())
      new_level["columns"].push_back(JsonHelper::print_iu(*col));
   print.getChild()->produce(*this);
   new_level["seperator"] = print.getSeperator();
   new_level["child"] = cur_level;
   cur_level = new_level;
}

void QueryPlanWriter::visitProduce(const Count& count) {
   nlohmann::json new_level;
   new_level["operator"] = count.typeToString();
   for (const auto& col : count.getRequiredIus())
      new_level["columns"].push_back(JsonHelper::print_iu(*col));
   count.getChild()->produce(*this);
   new_level["child"] = cur_level;
   cur_level = new_level;
}

void QueryPlanWriter::visitProduce(const CrossProduct&) {
   not_implemented();
}

void QueryPlanWriter::visitProduce(const Selection& selection) {
   nlohmann::json new_level;
   new_level["operator"] = selection.typeToString();
   new_level["predicate"] = JsonHelper::print_expression(selection.getPredicate());
   selection.getChild()->produce(*this);
   new_level["child"] = cur_level;
   cur_level = new_level;
}

void QueryPlanWriter::visitProduce(const TableScan& tableScan) {
   cur_level["operator"] = tableScan.typeToString();
   cur_level["alias"] = tableScan.getAlias();
   cur_level["table"] = database.getSchema(tableScan.getTable()).name;
}

void QueryPlanWriter::close() {
   out << cur_level.dump(2) << std::endl;
}

std::unique_ptr<Operator> QueryPlanReader::read_operator(const Database& db, nlohmann::basic_json<>::value_type& json_operator) {
   switch (Operator::typeFromString(json_operator["operator"])) {
      case Operator::InnerJoin: {
         auto left_child = read_operator(db, json_operator["leftChild"]);
         auto right_child = read_operator(db, json_operator["rightChild"]);

         auto left_ius = left_child->collectIUs();
         auto right_ius = right_child->collectIUs();
         left_ius.insert(left_ius.end(), right_ius.begin(), right_ius.end());

         auto join_condition = JsonHelper::read_predicate(json_operator["joinCondition"], left_ius);
         assert(join_condition->get_type() == Expression::JoinConditionList);

         std::unique_ptr<JoinConditionList> casted_join_condition(static_cast<JoinConditionList*>(join_condition.release()));
         return std::make_unique<InnerJoin>(std::move(left_child), std::move(right_child), std::move(casted_join_condition), JoinMode::TopInsert);
      }
      case Operator::Print: {
         auto child = read_operator(db, json_operator["child"]);
         auto child_ius = child->collectIUs();
         OrderedIUSet required_ius;
         required_ius.reserve(json_operator["columns"].size());
         for (auto col : json_operator["columns"]) {
            required_ius.push_back(JsonHelper::find_iu(col, child_ius));
         }
         auto printer = std::make_unique<Print>(std::move(child), json_operator["seperator"]);
         DefaultVisitor v;
         printer->prepare(v, required_ius, nullptr);
         return printer;
      }
      case Operator::Count: {
         auto child = read_operator(db, json_operator["child"]);
         auto child_ius = child->collectIUs();
         auto counter = std::make_unique<Count>(std::move(child));
         DefaultVisitor v;
         IUSet required_ius;
         required_ius.reserve(json_operator["columns"].size());
         for (auto col : json_operator["columns"])
            required_ius.insert(JsonHelper::find_iu(col, child_ius));
         counter->prepare(v, required_ius, nullptr);
         return counter;
      }
      case Operator::Selection: {
         auto child = read_operator(db, json_operator["child"]);
         auto predicate = JsonHelper::read_predicate(json_operator["predicate"], child->collectIUs());
         return std::make_unique<Selection>(std::move(child), std::move(predicate));
      }
      case Operator::TableScan: {
         std::string table = json_operator["table"];
         return std::make_unique<TableScan>(db, table, json_operator["alias"]);
      }
      default:
         unreachable();
   }
}

} // namespace factDB::algebra::visitors
