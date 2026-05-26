#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include "factDB/algebra/JoinMode.hpp"
#include "factDB/infra/BitSet.hpp"
#include "factDB/infra/Expression.hpp"
#include <list>
#include <string>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class IU;
// ---------------------------------------------------------------------------------------------------
namespace opt {
// ---------------------------------------------------------------------------------------------------
struct OptimizerRelation;
struct OptimizerJoin;
class QueryGraph;
struct JoinInfos;
// ---------------------------------------------------------------------------------------------------
} // namespace opt
// ---------------------------------------------------------------------------------------------------
struct QueryGraphBuilder {
   const Database* db;
   std::list<std::string> container;
   std::list<IU> ius;
   std::vector<opt::OptimizerRelation> relations;
   std::vector<opt::OptimizerJoin> joins;
   std::vector<opt::JoinInfos> joinInfos;
   std::list<factDB::JoinConditionList> joinConditions;

   explicit QueryGraphBuilder(const Database& db, size_t relationsRequired);

   void addRelation();

   std::pair<const IU*, const IU*> addJoinInternal(size_t leftOwner, size_t rightOwner, const infra::BitSet64& leftRelation, const infra::BitSet64& rightRelation, const IU* leftIU = nullptr, const IU* rightIU = nullptr, algebra::JoinType joinType = algebra::JoinType::Inner);
   std::pair<const IU*, const IU*> addJoin(size_t leftOwner, size_t rightOwner, const infra::BitSet64& leftRelation, const infra::BitSet64& rightRelation, algebra::JoinType joinType = algebra::JoinType::Inner) {
      return addJoinInternal(leftOwner, rightOwner, leftRelation, rightRelation, nullptr, nullptr, joinType);
   }
   std::pair<const IU*, const IU*> addJoin(size_t leftRelation, size_t rightRelation, const IU* leftIU = nullptr, const IU* rightIU = nullptr, algebra::JoinType joinType = algebra::JoinType::Inner);

   [[nodiscard]] opt::QueryGraph getQueryGraph() const;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------