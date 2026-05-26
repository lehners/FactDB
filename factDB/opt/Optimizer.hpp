#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include "factDB/infra/IUSet.hpp"
#include <memory>
#include <unordered_map>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class Database;
class IU;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra { class Operator; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
class Plan;
// ---------------------------------------------------------------------------------------------------
struct QueryOptimizer {
   using OperatorTree = std::unique_ptr<algebra::Operator>;
   using TreeStructure = std::pair<OperatorTree, factDB::OrderedIUSet>;

   Database& database;
   OperatorTree tree;
   OrderedIUSet requiredColumns;
   std::unordered_map<const IU*, size_t> iu2equivalenceClass;

   QueryOptimizer(Database& db, OperatorTree tree, OrderedIUSet requiredColumns);

   std::unordered_map<const IU*, size_t> getEquivalenceClasses() { return iu2equivalenceClass; }
   TreeStructure getOptimalTree();

   void pushPredicatesDown() const;
   void reorderJoins();
   void flattenJoins() const;
   void optimizeBuildSides();
   void optimizeTree();

   static OperatorTree optimize(Database& db, OperatorTree tree, OrderedIUSet& requiredColumns);
   static OrderedIUSet updateRequiredIUs(const OrderedIUSet& requiredColumns, algebra::Operator& newTree);

   static size_t estimateCost(algebra::Operator& tree, Database& database);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
