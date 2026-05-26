// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/algs/FixedPlan.hpp"
#include "factDB/algebra/JoinMode.hpp"
#include "factDB/infra/util/ranges.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "factDB/opt/oracle/BaseOracle.hpp"
#include <ranges>
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt::algs {
// ---------------------------------------------------------------------------------------------------
void FixedPlan::skipSpaces() {
   while (pos < planStr.size() && std::isspace(planStr[pos]))
      pos++;
}
// ---------------------------------------------------------------------------------------------------
bool FixedPlan::peek(char c) const {
   return pos < planStr.size() && planStr[pos] == c;
}
// ---------------------------------------------------------------------------------------------------
void FixedPlan::expect(char c) {
   if (pos >= planStr.size() || planStr[pos] != c)
      throw std::runtime_error(std::string("Expected '") + c + "' at position " + std::to_string(pos));
   ++pos;
}
// ---------------------------------------------------------------------------------------------------
uint32_t FixedPlan::parseUInt32() {
   skipSpaces();
   uint32_t val = 0;
   bool hasDigit = false;
   while (pos < planStr.size() && std::isdigit(planStr[pos])) {
      val = val * 10 + (planStr[pos++] - '0');
      hasDigit = true;
   }
   if (!hasDigit)
      throw std::runtime_error("Expected integer at position " + std::to_string(pos));
   return val;
}
// ---------------------------------------------------------------------------------------------------
algebra::JoinMode FixedPlan::parseJoinMode() {
   skipSpaces();
   algebra::JoinMode mode = algebra::JoinMode::TopInsert;
   switch (planStr[pos]) {
      case 'B': mode = algebra::JoinMode::BottomInsert; break;
      case 'T': mode = algebra::JoinMode::TopInsert; break;
      default:
         throw std::runtime_error("Expected valid join mode starting with 'B' or 'T' " + std::to_string(pos));
   }
   ++pos;
   if (planStr[pos++] != 'I') {
      throw std::runtime_error("Expected valid join mode ending with 'I' " + std::to_string(pos));
   }
   return mode;
}
// ---------------------------------------------------------------------------------------------------
std::string FixedPlan::parseAlias() {
   skipSpaces();
   std::string name;
   while (pos < planStr.size() && (std::isalnum(planStr[pos]) || planStr[pos] == '_')) {
      name.push_back(planStr[pos++]);
   }
   if (name.empty())
      throw std::runtime_error("Expected identifier at position " + std::to_string(pos));
   return name;
}
// ---------------------------------------------------------------------------------------------------
opt::Plan* FixedPlan::parseExpr() {
   // Could be either a base table "tbl alias" or a join "(left plan) (right plan)"

   if (peek('(')) {
      expect('(');
      Plan* leftPlan = parseExpr();
      expect(')');
      auto mode = parseJoinMode();
      expect('(');
      Plan* rightPlan = parseExpr();
      expect(')');
      JoinPlan* joinPlan = planAllocator.allocate();
      *joinPlan = Plan::makeJoin(leftPlan, rightPlan, mode);
      return joinPlan;
   } else {
      uint32_t relation = parseUInt32();
      skipSpaces();
      auto alias = parseAlias();
      for (auto&& [idx, tbl] : queryGraph.getRelations() | views::enumerate) {
         if (tbl.relation == relation && tbl.alias == alias)
            return &baseTables.emplace_back(idx);
      }
      unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
void FixedPlan::generateOptimalPlan() {
   optimalPlan = parseExpr();
}
// ---------------------------------------------------------------------------------------------------
Plan* FixedPlan::getOptimalPlan() const {
   return optimalPlan;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt::algs
// ---------------------------------------------------------------------------------------------------