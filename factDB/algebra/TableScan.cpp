#include "factDB/algebra/TableScan.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
TableScan::TableScan(const Database& db, const std::string_view table_, const std::string alias_) // NOLINT
   : Operator(OperatorType::TableScan), relation(db.getRelationID(table_)), alias(alias_.empty() ? db.getSchema(relation).name : std::move(alias_)), ius(db.getColumns(relation, alias)) {
}
// ---------------------------------------------------------------------------------------------------
TableScan::TableScan(const Database& db, const uint32_t rel, const std::string alias_) // NOLINT
   : Operator(OperatorType::TableScan), relation(rel), alias(alias_.empty() ? db.getSchema(relation).name : std::move(alias_)), ius(db.getColumns(rel, alias)) {}
// ---------------------------------------------------------------------------------------------------
TableScan::~TableScan() = default;
// ---------------------------------------------------------------------------------------------------
void TableScan::prepare(visitors::AlgebraVisitor& visitor, [[maybe_unused]] IUSet& requiredPipeline, [[maybe_unused]] Operator* _consumer, const IUSet& required) {
   requiredIusPipeline = requiredPipeline;
   requiredIus = required.merge(requiredIusPipeline);
   consumer = _consumer;

   if constexpr (debugMode) { // only in debug mode
      // check if all ius are actually owned by this table scan.
      auto all = collectIUs();
      for ([[maybe_unused]] auto& iu : requiredIus) {
         assert(all.contains(*iu));
      }
   }

   visitor.visitPrepare(visitors::PrepareStage::BeforeCall, *this);
}
// ---------------------------------------------------------------------------------------------------
void TableScan::produce(visitors::AlgebraVisitor& visitor) const {
   visitor.visitProduce(*this);
}
// ---------------------------------------------------------------------------------------------------
void TableScan::produce(visitors::AlgebraVisitor& visitor) {
   visitor.visitProduce(*this);
}
// ---------------------------------------------------------------------------------------------------
void TableScan::consume(const visitors::AlgebraVisitor::ConsumeStage, visitors::AlgebraVisitor&, const Operator*) const {
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
OrderedIUSet TableScan::collectIUs() const {
   OrderedIUSet vec;
   vec.reserve(ius.size());
   for (auto& i : ius)
      vec.push_back(i.get());
   return vec;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------