#ifndef H_FACTDB_FACTDB_OPT_OPTIMIZERREPRESENTATION_HPP
#define H_FACTDB_FACTDB_OPT_OPTIMIZERREPRESENTATION_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/BitSet.hpp"
#include "factDB/infra/Expression.hpp"
#include <cstdint>
#include <string_view>
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB { class IU; }
namespace factDB::algebra { enum class JoinType : std::uint8_t; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
struct OptimizerRelation {
   uint32_t relation;
   std::string_view alias;
};
// ---------------------------------------------------------------------------------------------------
struct OptimizerJoin {
   infra::BitSet64 leftRelation;
   infra::BitSet64 rightRelation;

   unsigned joinID;

   algebra::JoinType joinType = static_cast<algebra::JoinType>(0);

   OptimizerJoin(unsigned left, unsigned right, unsigned joinId_, algebra::JoinType joinType_ = static_cast<algebra::JoinType>(0)) // NOLINT(bugprone-easily-swappable-parameters)
      : leftRelation(left), rightRelation(right), joinID(joinId_), joinType(joinType_) {}
   OptimizerJoin(infra::BitSet64 left, infra::BitSet64 right, unsigned joinID_, algebra::JoinType joinType_ = static_cast<algebra::JoinType>(0)) // NOLINT(bugprone-easily-swappable-parameters)
      : leftRelation(std::move(left)), rightRelation(std::move(right)), joinID(joinID_), joinType(joinType_) {}

   [[nodiscard]] bool operator==(const OptimizerJoin& other) const { return joinID == other.joinID; }
   // [[nodiscard]] bool cmpNormalized(const OptimizerJoin& other) const;
};
// ---------------------------------------------------------------------------------------------------
struct JoinInfos {
   const Expression* joinCondition;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_OPT_OPTIMIZERREPRESENTATION_HPP