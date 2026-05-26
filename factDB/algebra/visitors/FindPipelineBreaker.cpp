// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/visitors/FindPipelineBreaker.hpp"
#include "factDB/algebra/Count.hpp"
#include "factDB/algebra/CrossProduct.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/algebra/Print.hpp"
#include "factDB/algebra/Selection.hpp"
#include "factDB/algebra/TableScan.hpp"
#include <cassert>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
void FindPipelineBreaker::visitProduce(const Print& print) {
   print.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FindPipelineBreaker::visitProduce(const Selection& selection) {
   selection.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FindPipelineBreaker::visitProduce(const Count& count) {
   count.getChild()->produce(*this);
}
// ---------------------------------------------------------------------------------------------------
void FindPipelineBreaker::visitProduce(const InnerJoin& join) {
   pipelineBreaker = &join;
}
// ---------------------------------------------------------------------------------------------------
void FindPipelineBreaker::visitProduce(const TableScan& tableScan) {
   pipelineBreaker = &tableScan;
}
// ---------------------------------------------------------------------------------------------------
void FindPipelineBreaker::visitProduce(const CrossProduct& crossProduct) {
   pipelineBreaker = &crossProduct;
}
// ---------------------------------------------------------------------------------------------------
void FindPipelineBreaker::visitConsume(const ConsumeStage, const InnerJoin& join, const Operator*) {
   pipelineBreaker = &join;
}
// ---------------------------------------------------------------------------------------------------
void FindPipelineBreaker::visitConsume(const ConsumeStage, const CrossProduct& crossProduct, const Operator*) {
   pipelineBreaker = &crossProduct;
}
// ---------------------------------------------------------------------------------------------------
void FindPipelineBreaker::visitConsume(const ConsumeStage, const Print& print, const Operator*) {
   pipelineBreaker = &print;
}
// ---------------------------------------------------------------------------------------------------
void FindPipelineBreaker::visitConsume(const ConsumeStage, const Count& count, const Operator*) {
   pipelineBreaker = &count;
}
// ---------------------------------------------------------------------------------------------------
void FindPipelineBreaker::visitConsume(const ConsumeStage, const Selection& selection, const Operator*) {
   selection.consumerConsume(ConsumeStage::InLoop, *this);
}
// ---------------------------------------------------------------------------------------------------
const Operator& FindPipelineBreaker::getPipelineStarter(const factDB::algebra::Operator& op, bool checkNullptr) {
   FindPipelineBreaker helper;
   op.produce(helper);
   if (checkNullptr) assert(helper.pipelineBreaker != nullptr);
   return *helper.pipelineBreaker;
}
// ---------------------------------------------------------------------------------------------------
const Operator& FindPipelineBreaker::getUpperPipelineBreaker(const factDB::algebra::Operator& op) {
   assert(op.getConsumer() != nullptr);
   FindPipelineBreaker helper;
   op.consumerConsume(ConsumeStage::InLoop, helper);
   assert(helper.pipelineBreaker != nullptr);
   return *helper.pipelineBreaker;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------