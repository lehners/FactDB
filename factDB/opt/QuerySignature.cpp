// ---------------------------------------------------------------------------------------------------
#include "factDB/opt/QuerySignature.hpp"
#include "factDB/Database.hpp"
#include "factDB/algebra/JoinMode.hpp"
#include "factDB/infra/Config.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/opt/OptimizerRepresentationGenerator.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
#include <sstream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
bool QuerySignature::compareRelations(const QuerySignature::OptimizerRelationTuple& rel1, const QuerySignature::OptimizerRelationTuple& rel2) {
   // return true is rel1's id is smaller
   if (rel1.relation.relation != rel2.relation.relation)
      return rel1.relation.relation < rel2.relation.relation;
   // sort by used ids twice the same relation is used
   return rel1.usedIUs.compareNumeric(rel2.usedIUs);
}
// ---------------------------------------------------------------------------------------------------
void QuerySignature::orderJoins() {
   /*auto compareJoins = [&](const OptimizerJoin& join1, const OptimizerJoin& join2) {
      assert(join1.leftRelation.compareNumeric(join1.rightRelation) || join1.joinType == algebra::JoinType::RightSemi);
      assert(join2.leftRelation.compareNumeric(join2.rightRelation) || join2.joinType == algebra::JoinType::RightSemi);
      assert(iu2relationIdx.contains(*join1.leftIU) && iu2relationIdx.contains(*join1.rightIU));
      assert(iu2relationIdx.contains(*join2.leftIU) && iu2relationIdx.contains(*join2.rightIU));

      if (join1.leftRelation != join2.leftRelation)
         return join1.leftRelation.compareNumeric(join2.leftRelation);
      else if (join1.rightRelation != join2.rightRelation)
         return join1.rightRelation.compareNumeric(join2.rightRelation);
      else if (*join1.leftIU != *join2.leftIU) {
         return iu2relationIdx.find(*join1.leftIU)->second < iu2relationIdx.find(*join2.leftIU)->second;
      } else {
         return iu2relationIdx.find(*join1.rightIU)->second < iu2relationIdx.find(*join2.rightIU)->second;
      }
   };
   std::sort(joins.begin(), joins.end(), compareJoins);*/
}
// ---------------------------------------------------------------------------------------------------
void QuerySignature::prepare(const algebra::Operator* /*op*/) {
   /*auto optRepresent = OptimizerRepresentationGenerator::apply(op);

   relations.clear();
   joins.clear();
   relations.reserve(optRepresent.relations.size());
   joins.reserve(optRepresent.joins.size());

   // prepare relations for sorting
   for (size_t idx = 0; const auto& rel : optRepresent.relations) {
      auto columns = db.getColumns(rel.relation, rel.alias);
      for (size_t i = 0; i != columns.size(); ++i) {
         assert(!iu2relationIdx.contains(*columns[i]));
         iu2relationIdx[*columns[i]] = i;
      }
      infra::BitSetVar usedIUs(columns.size());
      OptimizerRelation rel2(rel);
      relations.emplace_back(rel2, usedIUs, idx++);
   }

   auto insertIUIntoBitmap = [&](const IU& iu) {
      assert(iu2relationIdx.contains(iu));
      auto iuIdx = iu2relationIdx.find(iu)->second;
      relations[optRepresent.getTableIdx(iu.table)].usedIUs.insert(iuIdx);
   };

   // set the bitmaps to prepare the ordering
   for (const auto& j : optRepresent.joins) {
      insertIUIntoBitmap(*j.leftIU);
      insertIUIntoBitmap(*j.rightIU);
   }
   orderRelations();

   std::vector<size_t> updatedRelationsIdx;
   updatedRelationsIdx.resize(relations.size());
   for (size_t idx = 0; idx != relations.size(); ++idx)
      updatedRelationsIdx[relations[idx].oldIdx] = idx;

   auto updateRelationIndexes = [&](const infra::BitSet64& oldIdxes) {
      infra::BitSet64 updated;
      for (auto oldIdx : oldIdxes)
         updated.insert(updatedRelationsIdx[oldIdx]);
      return updated;
   };

   // create the join list
   for (const auto& j : optRepresent.joins) {
      infra::BitSet64 leftRel = updateRelationIndexes(j.leftRelation);
      infra::BitSet64 rightRel = updateRelationIndexes(j.rightRelation);
      if (leftRel.compareNumeric(rightRel) || j.joinType == algebra::JoinType::RightSemi)
         joins.emplace_back(leftRel, rightRel, j.leftIU, j.rightIU, j.joinType);
      else
         joins.emplace_back(rightRel, leftRel, j.rightIU, j.leftIU, j.joinType);

      iu2relation[*j.leftIU] = updatedRelationsIdx[optRepresent.getTableIdx(j.leftIU->table)];
      iu2relation[*j.rightIU] = updatedRelationsIdx[optRepresent.getTableIdx(j.rightIU->table)];
   }

   // order the join list
   orderJoins();*/
}
// ---------------------------------------------------------------------------------------------------
std::string getJoinTypeAlias(algebra::JoinType joinType) {
   switch (joinType) {
      case algebra::JoinType::Inner: return "I";
      case algebra::JoinType::RightSemi: return "RS";
   }
   unreachable();
}
// ---------------------------------------------------------------------------------------------------
std::string QuerySignature::generateSignature() const {
   std::stringstream strStream;
   FileWriter ss(strStream, FileWriter::FinalNewlineEnum::NoFinalNewline);
   for (const auto& rel : relations)
      ss << rel.relation.relation << ",";
   /*for (const auto& join : joins) {
      ss << "(" << join.leftRelation << ", " << join.rightRelation << ", ";
      ss << iu2relation.find(*join.leftIU)->second << "." << join.leftIU->column << ", ";
      ss << iu2relation.find(*join.rightIU)->second << "." << join.rightIU->column << ", ";
      ss << getJoinTypeAlias(join.joinType) << "), ";
   }*/
   return strStream.str();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
