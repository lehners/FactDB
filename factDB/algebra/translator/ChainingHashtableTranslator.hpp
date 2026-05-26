#ifndef H_FACTDB_FACT_DB_ALGEBRA_TRANSLATOR_CHAININGHASHTABLETRANSLATOR_HPP
#define H_FACTDB_FACT_DB_ALGEBRA_TRANSLATOR_CHAININGHASHTABLETRANSLATOR_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/FWContainer.hpp"
// ---------------------------------------------------------------------------------------------------
// clang-format off
// ---------------------------------------------------------------------------------------------------
namespace factDB { struct OrderedIUSet; }
// ---------------------------------------------------------------------------------------------------
// clang-format on
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
class InnerJoin;
// ---------------------------------------------------------------------------------------------------
class HashTableTranslator {
   public:
   FWContainer htName;

   private:
   FWContainer keyType;
   FWContainer valueType;

   public:
   HashTableTranslator(FWContainer&& htNameP, FWContainer&& keyTypeP, FWContainer&& valueTypeP);
   HashTableTranslator(FWContainer&& htNameP, FWContainer&& keyTypeP, const FWContainer& valueTypeP);

   [[nodiscard]] static OrderedIUSet getLeftKeys(const InnerJoin& join);
   [[nodiscard]] static OrderedIUSet getRightKeys(const InnerJoin& join);

   [[nodiscard]] static FWContainer genTypeTuple(const OrderedIUSet& iuSet);
   [[nodiscard]] static FWContainer genTuple(const OrderedIUSet& iuSet);

   [[nodiscard]] FWContainer instance();
   [[nodiscard]] FWContainer finalize() const;
   [[nodiscard]] FWContainer size() const;

   [[nodiscard]] FWContainer hash(const FWContainer& varName, FWContainer&& keys) const;
   [[nodiscard]] FWContainer hash(const FWContainer& varName, const OrderedIUSet& keys) const;

   [[nodiscard]] FWContainer localEntries(const FWContainer& varName) const;

   [[nodiscard]] FWContainer insert(const FWContainer& hash, FWContainer&& keys, FWContainer&& values, const FWContainer& localEntries = {}) const;
   [[nodiscard]] FWContainer insert(const FWContainer& hash, const OrderedIUSet& keys, const OrderedIUSet& values, const FWContainer& localEntries = {}) const;
   [[nodiscard]] FWContainer genStats(const FWContainer& name) const;

   [[nodiscard]] FWContainer printStats(const FWContainer& name, size_t join_id) const;
   [[nodiscard]] FWContainer printStatsBuild(const FWContainer& output) const;

   [[nodiscard]] FWContainer lookupStats(const FWContainer& hash, const FWContainer& iterator, const FWContainer& stats) const;
   [[nodiscard]] FWContainer lookup(const FWContainer& hash, const FWContainer& iterator) const;
   [[nodiscard]] FWContainer lookupNext(const FWContainer& iterator) const;

   [[nodiscard]] FWContainer couldContain(const FWContainer& hash) const;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
#endif //  H_FACTDB_FACT_DB_ALGEBRA_TRANSLATOR_CHAININGHASHTABLETRANSLATOR_HPP