#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/IUSet.hpp"
#include <set>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class FTree;
// ---------------------------------------------------------------------------------------------------
class FIterator {
   std::size_t iteratorID;
   OrderedIUSet iteratedIUs;
   FTree& owner;

   const FIterator* referencedIterator = nullptr;

   std::unordered_set<std::size_t> requiredNodeIDs;

   public:
   FIterator(size_t iteratorId, const OrderedIUSet& iteratedIUs_, FTree& owner_);

   size_t getIteratorID() const { return iteratorID; }

   void setReferencedIterator(const FIterator& iter) { referencedIterator = &iter; }
   bool hasReferencedIterator() const;
   const FIterator& getReferencedIterator() const { return *referencedIterator; }
   const FIterator* getReferencedIteratorPtr() const { return referencedIterator; }

   const std::unordered_set<std::size_t>& getRequiredNodeIDs() const { return requiredNodeIDs; }
   std::unordered_set<std::size_t> getTransitiveRequiredNodeIDs() const;
   const OrderedIUSet& getIteratedIUs() const { return iteratedIUs; }

   const FTree& getOwner() const { return owner; }
   FTree& getOwnerWriteable() const { return owner; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------