#include "factDB/algebra/visitors/factorized/FTreeDifference.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include "factDB/infra/Setting.hpp"
#include <iostream>
#include <sstream>
// ---------------------------------------------------------------------------------------------------
using namespace factDB::algebra::visitors;
// ---------------------------------------------------------------------------------------------------
factDB::Setting<bool>& naiveSetting() {
   return *factDB::SettingBase::getSetting<bool>("codegen.factorized.naiveMerge");
}
// ---------------------------------------------------------------------------------------------------
void FTreeDifference::visitPrepare(const Stage s, const TableScan& op) {
   {
      auto tmp = naiveSetting().setTemporary(true);
      naiveDeriver.visitPrepare(s, op);
   }
   improvedDeriver.visitPrepare(s, op);
}
// ---------------------------------------------------------------------------------------------------
void FTreeDifference::visitPrepare(const Stage s, const InnerJoin& op) {
   {
      auto tmp = naiveSetting().setTemporary(true);
      naiveDeriver.visitPrepare(s, op);
   }
   improvedDeriver.visitPrepare(s, op);
}
// ---------------------------------------------------------------------------------------------------
void FTreeDifference::visitPrepare(const Stage s, const Print& op) {
   {
      auto tmp = naiveSetting().setTemporary(true);
      naiveDeriver.visitPrepare(s, op);
   }
   improvedDeriver.visitPrepare(s, op);
}
// ---------------------------------------------------------------------------------------------------
void FTreeDifference::visitPrepare(const Stage s, const Count& op) {
   {
      auto tmp = naiveSetting().setTemporary(true);
      naiveDeriver.visitPrepare(s, op);
   }
   improvedDeriver.visitPrepare(s, op);
}
// ---------------------------------------------------------------------------------------------------
void FTreeDifference::visitProduce(const InnerJoin& op) {
   report(op);
}
// ---------------------------------------------------------------------------------------------------
void FTreeDifference::visitProduce(const TableScan& op) {
   report(op);
}
// ---------------------------------------------------------------------------------------------------
void FTreeDifference::visitProduce(const Print& op) {
   op.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FTreeDifference::visitProduce(const Selection& op) {
   op.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FTreeDifference::visitProduce(const Count& op) {
   op.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FTreeDifference::report(const Operator& op) const {
   auto& naiveTree = naiveDeriver.getFTree(op);
   auto& improvedTree = improvedDeriver.getFTree(op);

   std::string improvedStr, naiveStr;
   {
      std::stringstream ss;
      fw::FileWriter fw(ss, FileWriter::Silent);
      improvedTree.print(fw, false);
      improvedStr = ss.str();
   }
   {
      std::stringstream ss;
      fw::FileWriter fw(ss, FileWriter::Silent);
      naiveTree.print(fw, false);
      naiveStr = ss.str();
   }

   if (improvedStr != naiveStr) {
      std::cout << "improved: " << std::endl
                << improvedStr
                << "naive" << std::endl
                << naiveStr << std::endl
                << "--------------------------" << std::endl;
   }
}
// ---------------------------------------------------------------------------------------------------
