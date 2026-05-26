#ifndef H_FACTDB_FACTDB_ALGEBRA_OPERATOR_HPP
#define H_FACTDB_FACTDB_ALGEBRA_OPERATOR_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/infra/Config.hpp"
#include <cstdint>
#include <stdexcept>
#include <string>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct IUSet;
struct OrderedIUSet;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::visitors {
// ---------------------------------------------------------------------------------------------------
enum class PrepareStage : uint8_t;
enum class ConsumeStage : uint8_t;
class AlgebraVisitor;
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::visitors
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
class Operator {
   friend class factDB::algebra::visitors::AlgebraVisitor;

   public:
   enum OperatorType {
      InnerJoin,
      Print,
      Selection,
      TableScan,
      Count,
      CrossProduct,
      Reference,
   };

   protected:
   static size_t globalOperatorId;

   // Consumer
   Operator* consumer = nullptr;
   OperatorType otype;
   size_t operatorId;

   Operator(OperatorType otypeP) : otype(otypeP), operatorId(++globalOperatorId) {}

   protected:
   template <typename Operator::OperatorType type, std::derived_from<Operator> T>
   static T* dynCastTemplate(std::conditional_t<std::is_const_v<T>, const Operator, Operator>* ptr) {
      return (ptr && ptr->getType() == type) ? static_cast<T*>(ptr) : nullptr;
   }

   public:
   virtual void prepare(visitors::AlgebraVisitor& visitor, IUSet& requiredPipeline, Operator* consumer, const IUSet& requiredGlobal) = 0;
   virtual void produce(visitors::AlgebraVisitor& visitor) const = 0;
   virtual void produce(visitors::AlgebraVisitor& visitor) = 0;
   virtual void consume(const visitors::ConsumeStage, visitors::AlgebraVisitor& visitor, const Operator* caller) const = 0;
   [[nodiscard]] virtual OrderedIUSet collectIUs() const = 0;
   virtual ~Operator() = default;

   inline void consumerConsume(const visitors::ConsumeStage stage, visitors::AlgebraVisitor& visitor) const {
      if (!!consumer) consumer->consume(stage, visitor, this);
   };

   static OperatorType typeFromString(const std::string& str) {
      switch (str[0]) {
         case 'C': return (str == "Count") ? Count : throw std::runtime_error("unknown Type");
         case 'I': return (str == "InnerJoin") ? InnerJoin : throw std::runtime_error("unknown Type");
         case 'P': return (str == "Print") ? Print : throw std::runtime_error("unknown Type");
         case 'S': return (str == "Selection") ? Selection : throw std::runtime_error("unknown Type");
         case 'T': return (str == "TableScan") ? TableScan : throw std::runtime_error("unknown Type");
      }
      throw std::runtime_error("unknown Type");
   }

   [[nodiscard]] const std::string typeToString() const {
      switch (otype) {
         case InnerJoin: return "InnerJoin";
         case CrossProduct: return "CrossProduct,";
         case Print: return "Print";
         case Selection: return "Selection";
         case TableScan: return "TableScan";
         case Count: return "Count";
         case Reference: return "Reference";
      }
      unreachable();
   }

   [[nodiscard]] OperatorType getType() const { return otype; };
   [[nodiscard]] Operator* getConsumer() const { return consumer; };
   [[nodiscard]] size_t getOperatorId() const { return operatorId; };

   static void resetIds() {
      globalOperatorId = 0;
   }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_ALGEBRA_OPERATOR_HPP
