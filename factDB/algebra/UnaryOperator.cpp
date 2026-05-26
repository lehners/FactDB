#include "factDB/algebra/UnaryOperator.hpp"
#include "factDB/infra/IUSet.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
OrderedIUSet UnaryOperator::collectIUs() const {
   return child->collectIUs();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
