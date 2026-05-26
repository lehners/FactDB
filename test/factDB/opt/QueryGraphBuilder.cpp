// ---------------------------------------------------------------------------------------------------
#include "test/factDB/opt/QueryGraphBuilder.hpp"
#include "factDB/Database.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/opt/QueryGraph.hpp"
#include "fmt/format.h"
#include <iostream>
#include <sstream>
// ---------------------------------------------------------------------------------------------------
using namespace std;
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
std::string_view KRelationTestMany = "KRelationTestMany";
// ---------------------------------------------------------------------------------------------------
QueryGraphBuilder::QueryGraphBuilder(const Database& database, size_t relationsRequired) : db(&database) {
   database.getRelationID(KRelationTestMany); // check that relation is contained.
   relations.reserve(relationsRequired);
   for (size_t i = 0; i != relationsRequired; ++i) {
      addRelation();
   }
}
// ---------------------------------------------------------------------------------------------------
void QueryGraphBuilder::addRelation() {
   assert(db != nullptr);
   auto& curAlias = container.emplace_back(fmt::format("r{}", relations.size()));
   relations.emplace_back(db->getRelationID(KRelationTestMany), curAlias);
}
// ---------------------------------------------------------------------------------------------------
std::pair<const IU*, const IU*> QueryGraphBuilder::addJoinInternal(size_t leftOwner, size_t rightOwner, const infra::BitSet64& leftRelation, const infra::BitSet64& rightRelation, const factDB::IU* leftIU, const factDB::IU* rightIU, algebra::JoinType joinType) {
   assert(std::max(leftRelation.back(), rightRelation.back()) < relations.size());
   assert(leftRelation.contains(leftOwner) && rightRelation.contains(rightOwner));
   // create new IUs
   if (leftIU && leftIU->table != relations[leftOwner].alias) {
      std::swap(leftIU, rightIU); // probably wrong input order
   }
   assert(leftIU == nullptr || leftIU->table == relations[leftOwner].alias);
   assert(rightIU == nullptr || rightIU->table == relations[rightOwner].alias);
   if (leftIU == nullptr)
      leftIU = &ius.emplace_back(relations[leftOwner].alias, fmt::format("iu{}", ius.size()), schemac::Type::Integer());
   if (rightIU == nullptr)
      rightIU = &ius.emplace_back(relations[rightOwner].alias, fmt::format("iu{}", ius.size()), schemac::Type::Integer());
   // create the join
   joins.emplace_back(leftRelation, rightRelation, joinInfos.size(), joinType);
   std::initializer_list<factDB::JoinCondition> jcinit = {JoinCondition::create(leftIU, rightIU)};
   joinConditions.emplace_back(jcinit);
   joinInfos.emplace_back(&joinConditions.back());
   return {leftIU, rightIU};
}
// ---------------------------------------------------------------------------------------------------
std::pair<const IU*, const IU*> QueryGraphBuilder::addJoin(size_t leftRelation, size_t rightRelation, const IU* leftIU, const IU* rightIU, algebra::JoinType joinType) {
   return addJoinInternal(leftRelation, rightRelation, infra::BitSet64(leftRelation), infra::BitSet64(rightRelation), leftIU, rightIU, joinType);
}
// ---------------------------------------------------------------------------------------------------
opt::QueryGraph QueryGraphBuilder::getQueryGraph() const {
   assert(db != nullptr);
   return opt::QueryGraph::generateQueryGraph(*db, relations, joins, joinInfos, {});
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------