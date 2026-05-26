#ifndef H_FACTDB_FACTDB_OPT_QUERYGRAPH_HPP
#define H_FACTDB_FACTDB_OPT_QUERYGRAPH_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/Operator.hpp"
#include "factDB/infra/BitSet.hpp"
#include "factDB/infra/IUSet.hpp"
#include "factDB/infra/UnionFind.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/opt/OptimizerRepresentation.hpp"
#include <list>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB { class Database; }
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw { struct FileWriter; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
class FactorizationGain;
class FlatSizes;
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra {
class BitSetVar;
} // namespace factDB::infra
// ---------------------------------------------------------------------------------------------------
namespace factDB::opt {
// ---------------------------------------------------------------------------------------------------
enum class AggregationMode : uint8_t;
class BaseEstimator;
class Plan;
class QueryGraph;
// ---------------------------------------------------------------------------------------------------
struct ConnectingJoinInfo {
   algebra::JoinType joinType;
   JoinCondition condition;
};
// ---------------------------------------------------------------------------------------------------
struct OperatorCache {
   std::unordered_map<size_t, std::unique_ptr<algebra::Operator>> plan2operator;

   std::unique_ptr<algebra::Operator> getOperatorPlan(const Plan& plan) const;
   void insertOperatorPlan(const Plan& plan, std::unique_ptr<algebra::Operator>& op);
};
// ---------------------------------------------------------------------------------------------------
class QueryGraph {
   friend class factDB::algebra::visitors::FactorizationGain;
   friend class factDB::algebra::visitors::FlatSizes;

   // the database
   const Database& db;
   // the query
   const algebra::Operator* query = nullptr;
   /// the original relations
   std::vector<OptimizerRelation> relations = {};
   /// the original joins between two nodes
   std::vector<OptimizerJoin> joins = {};
   /// the infos for the joins, holding the respective join conditions
   std::vector<JoinInfos> joinInfos = {};
   /// the container for split join condition lists
   std::list<JoinConditionList> joinConditionLists;
   /// the filter predicates on base tables
   std::vector<std::pair<unsigned, const Expression&>> tablePredicates;

   /// the aggregation mode
   AggregationMode aggregationMode = static_cast<AggregationMode>(0);

   // mapping from the name of a relation to its id
   std::unordered_map<std::string_view, unsigned> relation2id;
   // the mapping from a relation to equivalence classes
   std::vector<infra::BitSetVar> relations2equivClasses;
   std::vector<infra::BitSetVar> relations2strongEquivClasses;
   // the mapping from an equivalence class to its participating ius
   std::vector<IUSet> equivClass2ius;
   std::vector<IUSet> strongEquivClass2ius;
   // the mapping from an iu to the corresponding relation id
   std::unordered_map<const IU*, uint32_t, IUPointerHash, IUPointerEqual> iu2relation;

   infra::UnionFind<IU, IUHash, IUEqual> equivalentIUs;
   infra::UnionFind<IU, IUHash, IUEqual> innerJoinEquivalentIUs;

   OperatorCache* operatorCache = new OperatorCache();

   public:
   explicit QueryGraph(const Database& database) : db(database) {}
   explicit QueryGraph(const Database& database, const algebra::Operator& q) : db(database), query(&q) {}
   ~QueryGraph() { delete operatorCache; }

   void generateIuEquivalenceClasses();
   void deriveOperators();
   void prepareForOptimization();

   public:
   /// prints the iu equivalence classes for debugging purpose
   void printQueryGraph(fw::FileWriter& writer) const;
   void printIUEquivalences(fw::FileWriter& writer) const;

   bool isGraphConnected() const;

   const Database& getDatabase() const { return db; }

   const std::vector<OptimizerRelation>& getRelations() const { return relations; }
   const std::vector<OptimizerJoin>& getJoins() const { return joins; }
   const infra::BitSetVar& getIUEquivalenceClasses(uint32_t relation) const;
   const OptimizerRelation& getRelation(size_t idx) const { return relations[idx]; }
   uint32_t getRelation(const IU* iu) const;
   bool knowsIU(const IU* iu) const { return iu2relation.contains(iu); }
   bool hasJoinEdge(const infra::BitSet64& left, const infra::BitSet64& right) const;
   unsigned containsEquivalenceClass(const IU& iu) const { return equivalentIUs.contains(iu); }
   unsigned getEquivalenceClass(const IU& iu) const { return equivalentIUs.findConst(iu); }
   size_t maxEquivalenceClasses() const { return equivalentIUs.upperBound(); }
   std::vector<ConnectingJoinInfo> getConnectingJoins(infra::BitSet64 left, infra::BitSet64 right, std::unique_ptr<algebra::Operator>& leftPlan, std::unique_ptr<algebra::Operator>& rightPlan, const BaseEstimator& estimator) const;
   AggregationMode getAggregationMode() const { return aggregationMode; }
   std::unique_ptr<algebra::Operator> generateAggregatedTree(std::unique_ptr<algebra::Operator> op, const algebra::Operator& oldTree) const;
   const JoinInfos& getJoinInfo(const OptimizerJoin& join) const;
   std::vector<const Expression*> getFilterPredicates(size_t relationIdx) const;
   const std::vector<std::pair<unsigned, const Expression&>> getTablePredicates() const { return tablePredicates; }

   std::unique_ptr<algebra::Operator> getOperatorPlan(const Plan& plan) const { return operatorCache->getOperatorPlan(plan); }
   void insertOperatorPlan(const Plan& plan, std::unique_ptr<algebra::Operator>& op) const { return operatorCache->insertOperatorPlan(plan, op); }

   static QueryGraph generateQueryGraph(const Database& db, const algebra::Operator& query);
   static QueryGraph generateQueryGraph(const Database& db, const std::vector<OptimizerRelation> relations, const std::vector<OptimizerJoin>, std::vector<JoinInfos>, std::vector<std::pair<unsigned, const Expression&>>);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::opt
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_OPT_QUERYGRAPH_HPP
