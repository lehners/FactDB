#ifndef H_FACTDB_FACTDB_INFRA_UNIONFIND_HPP
#define H_FACTDB_FACTDB_INFRA_UNIONFIND_HPP
// ---------------------------------------------------------------------------------------------------
#include <cassert>
#include <cstddef>
#include <unordered_map>
#include <vector>
// ---------------------------------------------------------------------------------------------------
// adapted from Umbra
// (c) 2017 Thomas Neumann
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra {
// ---------------------------------------------------------------------------------------------------
class BitSetVar;
// ---------------------------------------------------------------------------------------------------
class UnionFindBase {
   template <class T, class Hash, class KeyEqual>
   friend class UnionFind;

   using IdType = unsigned;
   using EntryType = unsigned;

   struct Element {
      // representative idx + 1;
      IdType representative = 0;
      size_t rank = 0;

      [[nodiscard]] bool hasRepresentative() const { return representative != 0; }
      void setRepresentative(IdType rep) { representative = rep + 1; }
      [[nodiscard]] IdType getRepresentative() const {
         assert(hasRepresentative());
         return representative - 1;
      }
   };

   protected:
   std::vector<Element> elements;

   public:
   explicit constexpr UnionFindBase(size_t size) : elements(size){};
   ~UnionFindBase() = default;

   /// union two sets and return the new id
   IdType unionSets(EntryType s1, EntryType s2);
   /// find the id for a given entry
   IdType find(EntryType entry);
   /// find the id for a given entry, no path compression
   [[nodiscard]] IdType findConst(EntryType entry) const;
   /// Optimize the union for minimal access paths
   void optimizeAccessPaths();
   /// checks if all elements belong to the same set
   [[nodiscard]] bool isSingleSet();
   /// returns the size of the union find struct
   [[nodiscard]] size_t size() const { return elements.size(); }
   /// Returns an upper bound for the Id
   [[nodiscard]] IdType upperBound() const { return elements.size(); }
   /// returns a bitmap which is set for all indexes with equal representative
   [[nodiscard]] BitSetVar getEqualElements(EntryType entry) const;

   /// merge two UnionFind structs
   static UnionFindBase merge(UnionFindBase& a, UnionFindBase& b);
};
// ---------------------------------------------------------------------------------------------------
template <class T>
struct ReferenceWrapperHash {
   std::size_t operator()(const std::reference_wrapper<const T>& val) const {
      return std::hash<T>{}(val.get());
   }
};
// ---------------------------------------------------------------------------------------------------
template <class T>
struct ReferenceWrapperEqual {
   bool operator()(const std::reference_wrapper<const T>& lhs, const std::reference_wrapper<const T>& rhs) const {
      return &lhs.get() == &rhs.get();
   }
};
// ---------------------------------------------------------------------------------------------------
template <class T, class Hash = ReferenceWrapperHash<T>, class KeyEqual = ReferenceWrapperEqual<T>>
class UnionFind {
   using IdType = unsigned;
   static_assert(!std::is_integral<T>() && "use UnionFindBase for integral types.");

   UnionFindBase unionFind;
   std::unordered_map<std::reference_wrapper<const T>, unsigned, Hash, KeyEqual> type2id;
   std::vector<const T*> id2type;

   public:
   UnionFind() : unionFind(0) {}

   unsigned getId(const T& val) const {
      assert(type2id.contains(val));
      return type2id.find(val)->second;
   }
   unsigned getId(const T& val) {
      if (!type2id.contains(val)) {
         type2id[val] = unionFind.elements.size();
         id2type.emplace_back(&val);
         unionFind.elements.emplace_back();
      }
      return type2id[val];
   }
   const T& getValue(size_t id) {
      assert(id < id2type.size());
      return *id2type[id];
   }

   /// adds an element
   IdType addElement(const T& val) { return unionFind.find(getId(val)); }
   /// union two sets and return the new id
   IdType unionSets(const T& s1, const T& s2) { return unionFind.unionSets(getId(s1), getId(s2)); }
   /// find the id for a given entry
   [[nodiscard]] IdType find(const T& entry) { return unionFind.find(getId(entry)); }
   /// find the id for a given entry, no path compression
   [[nodiscard]] IdType findConst(const T& entry) const { return unionFind.findConst(getId(entry)); }
   /// contains a value?
   [[nodiscard]] bool contains(const T& entry) const { return type2id.contains(entry); }
   /// returns the size of the union find struct
   [[nodiscard]] bool isSingleSet() { return unionFind.isSingleSet(); }
   /// returns the size of the union find struct
   [[nodiscard]] size_t size() const { return unionFind.size(); }
   /// Optimize the union for minimal access paths
   void optimizeAccessPaths() { return unionFind.optimizeAccessPaths(); }

   /// Returns the values stored in the union find
   [[nodiscard]] const std::vector<const T*> getValues() const { return id2type; }
   /// Returns an upper bound for the Id
   [[nodiscard]] IdType upperBound() const { return unionFind.upperBound(); }

   /// merge two UnionFind structs
   static UnionFindBase merge(UnionFind& a, UnionFind& b) {
      assert(a.type2id.size() == b.type2id.size());
      for ([[maybe_unused]] auto& [aKey, _] : a.type2id) {
         assert(b.type2id.contains(aKey));
      }
      return UnionFindBase::merge(a, b);
   }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_INFRA_UNIONFIND_HPP
