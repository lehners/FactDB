#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/newftree/FIterator.hpp"
#include "factDB/newftree/generator/util/FGenUtil.hpp"
#include "factDB/newftree/generator/util/FNodeGenerator.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct FIteratorGenerator : public FNodeGenerator {
   const FIterator& iterator;
   FIteratorGenerator(FileWriter& o, const FIterator& iter) : FNodeGenerator(o), iterator(iter) {};

   bool isNodeRequired(const FNode& node) const;
   bool isNodeRequiredByReferenceIterator(const FNode& node) const;
   bool isNodeInIteratorContained(const FNode& node) const;
   std::unordered_set<size_t> inIteratorContainedIDs() const;
   bool hasRequiredChild(const FNode& node) const;
   bool hasNotRequiredChild(const FNode& node) const;
   bool hasChildNotRequiredByReferenceIterator(const FNode& node, bool checkIteratedIUs = true) const;
   void recurseRequiredChildren(const FNode& node);
   void recurseNotRequiredChildren(const FNode& node);
   void enumerateRequiredChildren(const FNode& node, size_t& id);

   bool containsFreeNodeInSubtree(const FNode& node) const;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
