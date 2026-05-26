// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/QueryCompiler.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::queryc {
// ---------------------------------------------------------------------------------------------------
void QueryCompiler::compile(std::unique_ptr<algebra::Operator>& tree, const OrderedIUSet& requiredColumns, algebra::visitors::AlgebraVisitor& visitor) {
   assert(tree->getType() == algebra::Operator::Print || tree->getType() == algebra::Operator::Count);
   switch (tree->getType()) {
      case algebra::Operator::Print:
         static_cast<algebra::Print*>(tree.get())->prepare(visitor, requiredColumns, nullptr);
         break;
      case algebra::Operator::Count:
         static_cast<algebra::Count*>(tree.get())->prepare(visitor, requiredColumns, nullptr);
         break;
      case algebra::Operator::InnerJoin:
      case algebra::Operator::Selection:
      case algebra::Operator::TableScan:
      case algebra::Operator::CrossProduct:
      case algebra::Operator::Reference:
         unreachable();
   }
   visitor.open();
   tree->produce(visitor);
   visitor.close();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::queryc
// ---------------------------------------------------------------------------------------------------