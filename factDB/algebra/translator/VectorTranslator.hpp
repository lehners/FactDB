#ifndef H_FACTDB_FACTDB_ALGEBRA_TRANSLATOR_VECTORTRANSLATOR_HPP
#define H_FACTDB_FACTDB_ALGEBRA_TRANSLATOR_VECTORTRANSLATOR_HPP
// ---------------------------------------------------------------------------------------------------
#include <cassert>
#include <source_location>
#include <string>
#include <vector>
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB { class IU; }
// ---------------------------------------------------------------------------------------------------
namespace factDB::fw { struct FWContainer; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra::translator {
// ---------------------------------------------------------------------------------------------------
class IuTupleTranslator {
   private:
   friend class TupleVectorTranslator;
   std::vector<const factDB::IU*> ius;

   public:
   IuTupleTranslator(std::initializer_list<const factDB::IU*> elements) : ius(elements) {}
   template <class... IUs>
   IuTupleTranslator(const IUs&... iusParams) : ius({&iusParams...}) {}

   [[nodiscard]] fw::FWContainer type() const;

   private:
   [[nodiscard]] fw::FWContainer make_tuple(std::initializer_list<fw::FWContainer> vals) const;
};
// ---------------------------------------------------------------------------------------------------
class TupleVectorTranslator {
   private:
   const IuTupleTranslator& tupleTypeVar;
   std::string instanceName;

   public:
   TupleVectorTranslator(IuTupleTranslator& tupleTranslator, std::string name) : tupleTypeVar(tupleTranslator), instanceName(std::move(name)) {}

   [[nodiscard]] const std::string& name() const { return instanceName; }
   [[nodiscard]] const fw::FWContainer tupleType() const;

   [[nodiscard]] fw::FWContainer size(const std::source_location& loc = std::source_location::current()) const;

   [[nodiscard]] fw::FWContainer instance() const;

   fw::FWContainer pushBackT(std::initializer_list<fw::FWContainer> vals);
   fw::FWContainer pushBackTuple(const std::string& tuple, std::source_location loc = std::source_location::current());
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra::translator
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_ALGEBRA_TRANSLATOR_VECTORTRANSLATOR_HPP