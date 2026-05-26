// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/BinaryOperator.hpp"
#include "factDB/infra/IUSet.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
OrderedIUSet BinaryOperator::collectIUs() const {
   auto leftIus = leftChild->collectIUs();
   auto rightIus = rightChild->collectIUs();
   return leftIus.merge(rightIus);
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------