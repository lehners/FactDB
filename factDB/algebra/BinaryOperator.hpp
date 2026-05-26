#ifndef H_factdb_algebra_binaryOperator
#define H_factdb_algebra_binaryOperator
// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/Operator.hpp"
#include "factDB/infra/IUSet.hpp"
#include <memory>
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
class BinaryOperator : public Operator {
   /// Implements the cross product operator, currently only for parsing & optimizer, should no more exist in codegen
   protected:
   // Left Child operator
   std::unique_ptr<Operator> leftChild;
   // Right Child operator
   std::unique_ptr<Operator> rightChild;

   // Globally required ius
   IUSet parentPipelineRequired;
   IUSet leftPipelineRequired;
   IUSet rightPipelineRequired;

   public:
   BinaryOperator(OperatorType otype_, std::unique_ptr<Operator> leftChild_, std::unique_ptr<Operator> rightChild_)
      : Operator(otype_), leftChild(std::move(leftChild_)), rightChild(std::move(rightChild_)){};
   ~BinaryOperator() = default;

   public:
   [[nodiscard]] inline const std::unique_ptr<Operator>& getLeftChild() const { return leftChild; }
   [[nodiscard]] inline const std::unique_ptr<Operator>& getRightChild() const { return rightChild; }
   [[nodiscard]] inline std::unique_ptr<Operator>& getLeftChild() { return leftChild; }
   [[nodiscard]] inline std::unique_ptr<Operator>& getRightChild() { return rightChild; }

   [[nodiscard]] OrderedIUSet collectIUs() const override;

   [[nodiscard]] inline const IUSet& getParentRequired() const { return parentPipelineRequired; }
   [[nodiscard]] inline const IUSet& getLeftRequired() const { return leftPipelineRequired; }
   [[nodiscard]] inline const IUSet& getRightRequired() const { return rightPipelineRequired; }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_algebra_binaryOperator
