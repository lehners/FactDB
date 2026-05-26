#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/BitSet.hpp"
#include "factDB/infra/IUSet.hpp"
#include "factDB/opt/OptimizerRepresentation.hpp"
#include <unordered_map>
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB { class Database; }
namespace factDB::algebra { class Operator; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
struct QuerySignature {
   struct OptimizerRelationTuple {
      OptimizerRelation relation;
      infra::BitSetVar usedIUs;
      size_t oldIdx;

      OptimizerRelationTuple(const OptimizerRelation& rel, infra::BitSetVar& usedIUsP, size_t idx) : relation(rel), usedIUs(usedIUsP), oldIdx(idx) {}
   };

   const Database& db;
   std::vector<OptimizerRelationTuple> relations;
   std::vector<OptimizerJoin> joins;
   std::unordered_map<const IU, size_t, IUHash, IUEqual> iu2relationIdx;
   std::unordered_map<const IU, size_t, IUHash, IUEqual> iu2relation;

   explicit QuerySignature(const Database& database) : db(database){};

   static bool compareRelations(const OptimizerRelationTuple& rel1, const OptimizerRelationTuple& rel2);

   void orderRelations() { std::sort(relations.begin(), relations.end(), compareRelations); }
   void orderJoins();
   void prepare(const algebra::Operator* op);
   std::string generateSignature() const;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
