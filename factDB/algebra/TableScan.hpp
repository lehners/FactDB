#ifndef H_FACTDB_FACTDB_ALGEBRA_TABLESCAN_HPP
#define H_FACTDB_FACTDB_ALGEBRA_TABLESCAN_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/Operator.hpp"
#include "factDB/infra/IUSet.hpp"
#include <memory>
#include <string>
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB { class Database; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
class TableScan : public Operator {
   private:
   // Table
   const uint32_t relation;
   // Alias
   const std::string alias;
   // Required ius
   IUSet requiredIus;
   // Required ius pipeline
   IUSet requiredIusPipeline;

   // ius of the table
   std::vector<std::unique_ptr<IU>> ius;

   public:
   TableScan(const Database& db, const std::string_view table, const std::string alias = "");
   TableScan(const Database& db, const uint32_t relation, const std::string alias = "");
   ~TableScan();

   void prepare(visitors::AlgebraVisitor& visitor, IUSet& requiredPipeline, Operator* consumer, const IUSet& required) override;

   void produce(visitors::AlgebraVisitor& visitor) const override;
   void produce(visitors::AlgebraVisitor& visitor) override;

   void consume(const visitors::ConsumeStage stage, visitors::AlgebraVisitor& visitor, const Operator* caller) const override;

   OrderedIUSet collectIUs() const override;

   inline const IUSet& getRequiredIus() const { return requiredIus; };
   inline const IUSet& getRequiredIusPipeline() const { return requiredIusPipeline; };
   inline const uint32_t& getTable() const { return relation; }
   inline const std::string& getAlias() const { return alias; }

   static TableScan* dynCast(Operator* op) { return (op && (op->getType() == OperatorType::TableScan)) ? static_cast<TableScan*>(op) : nullptr; }
   static const TableScan* dynCast(const Operator* op) { return (op && (op->getType() == OperatorType::TableScan)) ? static_cast<const TableScan*>(op) : nullptr; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_ALGEBRA_TABLESCAN_HPP