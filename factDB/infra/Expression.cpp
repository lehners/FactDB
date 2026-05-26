#include "factDB/infra/Expression.hpp"
#include "Config.hpp"
#include "factDB/infra/IUSet.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/types/Bool.hpp"
#include "factDB/infra/types/Integer.hpp"
#include "factDB/infra/types/RuntimeValue.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
IUSet Expression::collectIUs() const {
   switch (type) {
      case CompareL: return static_cast<const factDB::CompareLess*>(this)->collectIUs(); break;
      case CompareLE: return static_cast<const factDB::CompareLessEqual*>(this)->collectIUs(); break;
      case CompareEq: return static_cast<const factDB::CompareEqual*>(this)->collectIUs(); break;
      case CompareGE: return static_cast<const factDB::CompareGreaterEqual*>(this)->collectIUs(); break;
      case CompareG: return static_cast<const factDB::CompareGreater*>(this)->collectIUs(); break;
      case IURef: return static_cast<const factDB::IURef*>(this)->collectIUs(); break;
      case Const: return static_cast<const factDB::Const*>(this)->collectIUs(); break;
      case AndExpression: return static_cast<const factDB::AndExpression*>(this)->collectIUs(); break;
      case OrExpression: return static_cast<const factDB::OrExpression*>(this)->collectIUs(); break;
      case JoinCondition: return static_cast<const factDB::JoinCondition*>(this)->collectIUs(); break;
      case JoinConditionList: return static_cast<const factDB::JoinConditionList*>(this)->collectIUs(); break;
      case Reference: return static_cast<const factDB::ReferenceExpression*>(this)->collectIUs(); break;
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
IUSet Const::collectIUs() const {
   return {};
}
// ---------------------------------------------------------------------------------------------------
IUSet IURef::collectIUs() const {
   return {*iu};
}
// ---------------------------------------------------------------------------------------------------
IUSet BinaryExpression::collectIUs() const {
   return left->collectIUs().merge(right->collectIUs());
}
// ---------------------------------------------------------------------------------------------------
IUSet JoinCondition::collectIUs() const {
   return left.collectIUs().merge(right.collectIUs());
}
// ---------------------------------------------------------------------------------------------------
IUSet JoinConditionList::collectIUs() const {
   IUSet ret;
   for (const auto& c : conditions) {
      ret = ret.merge(c.collectIUs());
   }
   return ret;
}
// ---------------------------------------------------------------------------------------------------
ExpressionPtr IURef::copy(const IUSet& ius) const {
   for (auto* iu_ : ius) {
      if (*iu_ == get_iu())
         return create(iu_);
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
IUSet ReferenceExpression::collectIUs() const {
   return reference.collectIUs();
}
// ---------------------------------------------------------------------------------------------------
void JoinConditionList::insert(factDB::Expression& other) {
   switch (other.get_type()) {
      case Expression::JoinConditionList:
         return insert(static_cast<factDB::JoinConditionList&>(other));
      case ExpressionType::JoinCondition:
         conditions.push_back(std::move(static_cast<factDB::JoinCondition&>(other)));
         break;
      default:
         unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
void JoinConditionList::insert(factDB::JoinConditionList& other) {
   conditions.reserve(conditions.size() + other.conditions.size());
   for (auto& cond : other.conditions)
      conditions.push_back(std::move(cond));
   other.conditions.clear();
}
// ---------------------------------------------------------------------------------------------------
size_t getIUIdx(const std::unordered_map<const IU*, size_t, IUPointerHash, IUPointerEqual>& map, const IU* iu) {
   assert(map.contains(iu));
   return map.find(iu)->second;
}
// ---------------------------------------------------------------------------------------------------
RuntimeValue Expression::evaluate(const std::vector<RuntimeValue>& values, const IUMap& iuMapping) const {
   switch (type) {
      case CompareL: return static_cast<const factDB::CompareLess*>(this)->evaluate(values, iuMapping); break;
      case CompareLE: return static_cast<const factDB::CompareLessEqual*>(this)->evaluate(values, iuMapping); break;
      case CompareEq: return static_cast<const factDB::CompareEqual*>(this)->evaluate(values, iuMapping); break;
      case CompareGE: return static_cast<const factDB::CompareGreaterEqual*>(this)->evaluate(values, iuMapping); break;
      case CompareG: return static_cast<const factDB::CompareGreater*>(this)->evaluate(values, iuMapping); break;
      case IURef: return static_cast<const factDB::IURef*>(this)->evaluate(values, iuMapping); break;
      case Const: return static_cast<const factDB::Const*>(this)->evaluate(values, iuMapping); break;
      case AndExpression: return static_cast<const factDB::AndExpression*>(this)->evaluate(values, iuMapping); break;
      case OrExpression: return static_cast<const factDB::OrExpression*>(this)->evaluate(values, iuMapping); break;
      case JoinCondition: return static_cast<const factDB::JoinCondition*>(this)->evaluate(values, iuMapping); break;
      case JoinConditionList: return static_cast<const factDB::JoinConditionList*>(this)->evaluate(values, iuMapping); break;
      case Reference: return static_cast<const factDB::ReferenceExpression*>(this)->evaluate(values, iuMapping); break;
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
RuntimeValue Const::evaluate(const std::vector<RuntimeValue>&, const IUMap&) const {
   // for now only handle corner cases, not really efficient
   if (value.starts_with("Integer(") && value.ends_with(")")) {
      std::string val = value.substr(8, value.size() - 9);
      return Integer::castString(val).toRTV();
   }
   switch (getConstType().tclass) {
      case schemac::Type::KUndefined:
         not_implemented();
      case schemac::Type::KInteger:
         return Integer::castString(value).toRTV();
      case schemac::Type::KUInt64:
      case schemac::Type::KTimestamp:
      case schemac::Type::KDate:
      case schemac::Type::KNumeric:
      case schemac::Type::KChar:
         not_implemented();
      case schemac::Type::KVarchar: {
         RuntimeValue rv;
         rv.setString(value);
         rv.setType(rv.getType());
         return rv;
      }
      case schemac::Type::KBool:
         return Bool::castString(value).toRTV();
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
RuntimeValue IURef::evaluate(const std::vector<RuntimeValue>& values, const IUMap& iuMapping) const {
   assert(iuMapping.contains(iu));
   return values[getIUIdx(iuMapping, iu)];
}
// ---------------------------------------------------------------------------------------------------
RuntimeValue BinaryExpression::evaluate(const std::vector<RuntimeValue>& values, const IUMap& iuMapping) const {
   auto leftRes = left->evaluate(values, iuMapping);
   auto rightRes = right->evaluate(values, iuMapping);
   switch (get_type()) {
      case CompareL: return Bool(leftRes < rightRes).toRTV();
      case CompareLE: return Bool(leftRes <= rightRes).toRTV();
      case CompareEq: return Bool(leftRes == rightRes).toRTV();
      case CompareGE: return Bool(leftRes >= rightRes).toRTV();
      case CompareG: return Bool(leftRes > rightRes).toRTV();

      case AndExpression: return (leftRes && rightRes).toRTV();
      case OrExpression: return (leftRes || rightRes).toRTV();

      default: unreachable();
   }
}
// ---------------------------------------------------------------------------------------------------
RuntimeValue JoinCondition::evaluate(const std::vector<RuntimeValue>& values, const IUMap& iuMapping) const {
   return Bool(values[getIUIdx(iuMapping, &get_left())] == values[getIUIdx(iuMapping, &get_right())]).toRTV();
}
// ---------------------------------------------------------------------------------------------------
RuntimeValue JoinConditionList::evaluate(const std::vector<RuntimeValue>& values, const IUMap& iuMapping) const {
   bool resCondition = true;
   for (auto& a : *this) {
      assert(iuMapping.contains(&a.get_left()) && iuMapping.contains(&a.get_right()));
      resCondition &= values[getIUIdx(iuMapping, &a.get_left())] == values[getIUIdx(iuMapping, &a.get_right())];
   }
   return Bool(resCondition).toRTV();
}
// ---------------------------------------------------------------------------------------------------
RuntimeValue ReferenceExpression::evaluate(const std::vector<RuntimeValue>& values, const IUMap& iuMapping) const {
   return reference.evaluate(values, iuMapping);
}
// ---------------------------------------------------------------------------------------------------
bool JoinCondition::operator==(const factDB::JoinCondition& other) const {
   return other.get_left() == get_left() && get_right() == other.get_right();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------