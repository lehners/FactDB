#include "factDB/algebra/Reference.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/CrossProduct.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/IUSet.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
OrderedIUSet Reference::collectIUs() const {
   return child.collectIUs();
}
// ---------------------------------------------------------------------------------------------------
void Reference::prepare(visitors::AlgebraVisitor& visitor, IUSet& requiredPipeline, Operator* consumerP, const IUSet& required) {
   child.prepare(visitor, requiredPipeline, consumerP, required);
}
// ---------------------------------------------------------------------------------------------------
void Reference::produce(visitors::AlgebraVisitor& visitor) const {
   child.produce(visitor);
}
// ---------------------------------------------------------------------------------------------------
void Reference::produce(visitors::AlgebraVisitor& visitor) {
   child.produce(visitor);
}
// ---------------------------------------------------------------------------------------------------
void Reference::consume(const visitors::AlgebraVisitor::ConsumeStage, visitors::AlgebraVisitor&, const Operator*) const {
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
