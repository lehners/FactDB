#include "factDB/algebra/visitors/factorized/FTreeComposition.hpp"
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
void FTreeCompositionVisualizer::visitPrepare(const Stage s, const TableScan& op) {
   deriver.visitPrepare(s, op);
}
// ---------------------------------------------------------------------------------------------------
void FTreeCompositionVisualizer::visitPrepare(const Stage s, const InnerJoin& op) {
   deriver.visitPrepare(s, op);
}
// ---------------------------------------------------------------------------------------------------
void FTreeCompositionVisualizer::visitPrepare(const Stage s, const Print& op) {
   deriver.visitPrepare(s, op);
}
// ---------------------------------------------------------------------------------------------------
void FTreeCompositionVisualizer::visitPrepare(const Stage s, const Count& op) {
   deriver.visitPrepare(s, op);
}
// ---------------------------------------------------------------------------------------------------
void FTreeCompositionVisualizer::report(const Operator& op) {
   auto& ftree = deriver.getFTree(op);
   ftree.printGeneration(out);
}
// ---------------------------------------------------------------------------------------------------
void FTreeCompositionVisualizer::visitProduce(const InnerJoin& op) {
   report(op);
}
// ---------------------------------------------------------------------------------------------------
void FTreeCompositionVisualizer::visitProduce(const TableScan& op) {
   report(op);
}
// ---------------------------------------------------------------------------------------------------
void FTreeCompositionVisualizer::visitProduce(const Print& op) {
   op.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FTreeCompositionVisualizer::visitProduce(const Selection& op) {
   op.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FTreeCompositionVisualizer::visitProduce(const Count& op) {
   op.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
