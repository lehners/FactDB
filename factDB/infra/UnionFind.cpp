// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/UnionFind.hpp"
#include "factDB/infra/BitSet.hpp"
// ---------------------------------------------------------------------------------------------------
// adapted from Umbra
// (c) 2017 Thomas Neumann
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra {
// ---------------------------------------------------------------------------------------------------
UnionFindBase::IdType UnionFindBase::find(UnionFindBase::EntryType entry) {
   auto& element = elements[entry];

   // no representative found
   if (!element.hasRepresentative())
      return entry;

   // direct child of its representative?
   IdType representative = element.getRepresentative();
   auto& parentElement = elements[representative];
   if (!parentElement.hasRepresentative())
      return representative;

   // recursively search for parent, compress the path
   while (elements[representative].hasRepresentative())
      representative = elements[representative].getRepresentative();
   for (EntryType iter = entry, next; iter != representative; iter = next) {
      assert(elements[iter].hasRepresentative());
      next = elements[iter].getRepresentative();
      elements[iter].setRepresentative(representative);
   }

   return representative;
}
// ---------------------------------------------------------------------------------------------------
UnionFindBase::IdType UnionFindBase::findConst(UnionFindBase::EntryType entry) const {
   auto& element = elements[entry];

   // no representative found
   if (!element.hasRepresentative())
      return entry;

   // recursively search for parent
   IdType representative = element.getRepresentative();
   while (elements[representative].hasRepresentative())
      representative = elements[representative].getRepresentative();
   return representative;
}
// ---------------------------------------------------------------------------------------------------
UnionFindBase::IdType UnionFindBase::unionSets(UnionFindBase::EntryType s1, UnionFindBase::EntryType s2) {
   auto rep1 = find(s1);
   auto rep2 = find(s2);

   if (rep1 == rep2)
      return rep1;

   auto& e1 = elements[rep1];
   auto& e2 = elements[rep2];
   if (e1.rank < e2.rank) {
      e1.setRepresentative(rep2);
      return rep2;
   } else if (e1.rank > e2.rank) {
      e2.setRepresentative(rep1);
      return rep1;
   } else if (rep1 < rep2) { // same rank
      e2.setRepresentative(rep1);
      e1.rank++;
      return rep1;
   } else { // same rank
      e1.setRepresentative(rep2);
      e2.rank++;
      return rep2;
   }
}
// ---------------------------------------------------------------------------------------------------
UnionFindBase UnionFindBase::merge(UnionFindBase& a, UnionFindBase& b) {
   assert(a.elements.size() == b.elements.size());

   UnionFindBase result = a;
   for (EntryType i = 0; i < a.elements.size(); ++i) {
      result.unionSets(a.find(i), b.find(i));
   }
   return result;
}
// ---------------------------------------------------------------------------------------------------
void UnionFindBase::optimizeAccessPaths() {
   for (size_t i = 0; i < elements.size(); ++i)
      find(i); // compress paths and ignore result
}
// ---------------------------------------------------------------------------------------------------
bool UnionFindBase::isSingleSet() {
   auto set0Id = find(0);
   for (IdType iter = 1; iter < elements.size(); ++iter) {
      auto found = find(iter);
      if (set0Id != found)
         return false;
   }
   return true;
}
// ---------------------------------------------------------------------------------------------------
BitSetVar UnionFindBase::getEqualElements(EntryType entry) const {
   auto rep = findConst(entry);
   BitSetVar bitSet(elements.size(), rep);
   for (size_t idx = 0; idx != elements.size(); ++idx) {
      if (bitSet.contains(idx))
         continue;
      if (!elements[idx].hasRepresentative())
         continue; // not in bitset and no representative => different equivalence class

      auto accessPath = bitSet;
      accessPath.insert(idx);
      IdType representative = elements[idx].getRepresentative();
      while (elements[representative].hasRepresentative() && !bitSet.contains(representative)) {
         accessPath.insert(representative);
         representative = elements[representative].getRepresentative();
      }

      if (bitSet.contains(representative))
         bitSet = accessPath;
   }
   return bitSet;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra
// ---------------------------------------------------------------------------------------------------