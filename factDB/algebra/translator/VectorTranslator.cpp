// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/translator/VectorTranslator.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/util/ranges.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::translator {
// ---------------------------------------------------------------------------------------------------
FWContainer IuTupleTranslator::type() const {
   return fw::func(
      [&](FileWriter& out) {
         out << "std::tuple<";
         bool first = true;
         for (const auto& iu : ius) {
            out << (!first ? ", " : fw::nop()) << iu->type;
            first = false;
         }
         out << ">";
      });
}
// ---------------------------------------------------------------------------------------------------
FWContainer TupleVectorTranslator::instance() const {
   return fw::lc("std::vector<", tupleTypeVar.type(), "> ", instanceName, fw::sendl());
}
// ---------------------------------------------------------------------------------------------------
FWContainer TupleVectorTranslator::pushBackTuple(const std::string& tuple, std::source_location loc) {
   return fw::lc(instanceName, ".push_back(", tuple, ");", fw::endl(loc));
}
// ---------------------------------------------------------------------------------------------------
const FWContainer TupleVectorTranslator::tupleType() const {
   return tupleTypeVar.type();
}
// ---------------------------------------------------------------------------------------------------
FWContainer TupleVectorTranslator::size(const std::source_location& loc) const {
   return fw::lc(instanceName, ".size();", fw::endl(loc));
}
// ---------------------------------------------------------------------------------------------------
FWContainer IuTupleTranslator::make_tuple(std::initializer_list<FWContainer> vals) const {
   assert(vals.size() == ius.size());
   auto container = fw::lc("std::make_tuple(", fw::pushSeparator(", "));
   auto v = vals.begin();
   for (const IU* iu : ius) {
      container = fw::lc(std::move(container), fw::separator(), iu->type, "(", *(v++), ")");
   }
   return fw::lc(std::move(container), fw::popSeparator(), ")");
}
// ---------------------------------------------------------------------------------------------------
FWContainer TupleVectorTranslator::pushBackT(std::initializer_list<FWContainer> vals) {
   return fw::lc(instanceName, ".push_back(", tupleTypeVar.make_tuple(vals), ");");
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::translator
// ---------------------------------------------------------------------------------------------------