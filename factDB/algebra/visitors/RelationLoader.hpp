#pragma once
// ---------------------------------------------------------------------------------------------------
#include "factDB/Database.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/CrossProduct.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/algebra/visitors/AlgebraVisitor.hpp"
#include "tbb/parallel_for_each.h"
#include <unordered_set>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
struct RelationLoaderConst : public algebra::visitors::AlgebraVisitorDB {
   std::unordered_set<uint32_t> relations;

   RelationLoaderConst(const Database& db) : AlgebraVisitorDB(db) {}
   void visitProduce(const algebra::Print& op) override { op.getChild()->produce(*this); }
   void visitProduce(const algebra::Selection& op) override { op.getChild()->produce(*this); }
   void visitProduce(const algebra::Count& op) override { op.getChild()->produce(*this); }
   void visitProduce(const algebra::InnerJoin& join) override {
      join.getLeftChild()->produce(*this);
      join.getRightChild()->produce(*this);
   }
   void visitProduce(const algebra::CrossProduct& crossProduct) override {
      crossProduct.getLeftChild()->produce(*this);
      crossProduct.getRightChild()->produce(*this);
   }
   void visitProduce(const algebra::TableScan& tableScan) override {
      relations.insert(tableScan.getTable());
   }
};
// ---------------------------------------------------------------------------------------------------
struct RelationLoader : public algebra::visitors::AlgebraVisitor {
   Database& database;
   std::unordered_set<uint32_t> relations;

   RelationLoader(Database& db) : database(db) {}
   void visitProduce(const algebra::Print& op) override { op.getChild()->produce(*this); }
   void visitProduce(const algebra::Selection& op) override { op.getChild()->produce(*this); }
   void visitProduce(const algebra::Count& op) override { op.getChild()->produce(*this); }
   void visitProduce(const algebra::InnerJoin& join) override {
      join.getLeftChild()->produce(*this);
      join.getRightChild()->produce(*this);
   }
   void visitProduce(const algebra::CrossProduct& crossProduct) override {
      crossProduct.getLeftChild()->produce(*this);
      crossProduct.getRightChild()->produce(*this);
   }
   void visitProduce(const algebra::TableScan& tableScan) override {
      relations.insert(tableScan.getTable());
   }

   void load() {
      tbb::parallel_for_each(relations.begin(), relations.end(), [&](auto& toLoad) {
         if (database.isRelationLoaded(toLoad))
            return;
         database.loadRelation(toLoad);
      });
   }

   void unload() {
      for (auto& toLoad : relations)
         database.unloadRelation(toLoad);
   }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------