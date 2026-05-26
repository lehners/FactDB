// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/translator/ChainingHashtableTranslator.hpp"
#include "factDB/algebra/InnerJoin.hpp"
#include "factDB/infra/Expression.hpp"
#include "factDB/infra/IUSet.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/infra/iu.hpp"
#include "factDB/infra/util/ranges.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::algebra {
// ---------------------------------------------------------------------------------------------------
HashTableTranslator::HashTableTranslator(FWContainer&& htNameP, FWContainer&& keyTypeP, FWContainer&& valueTypeP)
   : htName(std::move(htNameP)), keyType(std::move(keyTypeP)), valueType(std::move(valueTypeP)) {
}
// ---------------------------------------------------------------------------------------------------
HashTableTranslator::HashTableTranslator(FWContainer&& htNameP, FWContainer&& keyTypeP, const FWContainer& valueTypeP)
   : htName(std::move(htNameP)), keyType(std::move(keyTypeP)), valueType(fw::lc(valueTypeP)) {
}
// ---------------------------------------------------------------------------------------------------
OrderedIUSet HashTableTranslator::getLeftKeys(const factDB::algebra::InnerJoin& join) {
   auto iter = join.getJoinCondition() | views::transform([](const JoinCondition& jc) { return &jc.get_left(); });
   return {iter.begin(), iter.end()};
}
// ---------------------------------------------------------------------------------------------------
OrderedIUSet HashTableTranslator::getRightKeys(const factDB::algebra::InnerJoin& join) {
   auto iter = join.getJoinCondition() | views::transform([](const JoinCondition& jc) { return &jc.get_right(); });
   return {iter.begin(), iter.end()};
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::genTypeTuple(const OrderedIUSet& iuSet) {
   return fw::func([iuSet](FileWriter& out) { out << "std::tuple<" << fw::pushSeparator(", ") << (iuSet | views::transform([](const IU* iu) { return iu->type; })) << fw::popSeparator() << ">"; });
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::genTuple(const OrderedIUSet& iuSet) {
   return fw::func([iuSet](FileWriter& out) { out << "std::make_tuple(" << fw::pushSeparator(", ") << (iuSet | views::transform([](const IU* iu) { return iu; })) << fw::popSeparator() << ")"; });
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::instance() {
   return fw::fmt("ChainingHashTable<{}, {}> {};", keyType, valueType, htName);
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::finalize() const {
   bool parallelFor = SettingBase::getSetting<bool>("codegen.parallelFor")->get();
   if (parallelFor)
      return fw::lc(htName, ".finalizeParallel();");
   else
      return fw::lc(htName, ".finalizeSingleThreaded();");
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::size() const {
   return fw::lc(htName, ".size();");
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::hash(const FWContainer& varName, FWContainer&& keys) const {
   return fw::fmt("auto {} = Hash::hashTuple({});", varName, std::forward<FWContainer>(keys));
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::hash(const FWContainer& varName, const OrderedIUSet& keys) const {
   return hash(varName, genTuple(keys));
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::localEntries(const FWContainer& varName) const {
   return fw::fmt("auto& {} = {}.localEntries();", varName, htName);
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::insert(const FWContainer& hash, FWContainer&& keys, FWContainer&& values, const FWContainer& localEntries) const {
   [[maybe_unused]] auto useHashtableTLS = SettingBase::getSetting<bool>("codegen.hashtable.tls")->get();
   if (localEntries.isNOP()) {
      assert(!useHashtableTLS);
      return fw::fmt("{}.insert({}, {}, {});", htName, hash, std::forward<FWContainer>(keys), std::forward<FWContainer>(values));
   } else {
      assert(useHashtableTLS);
      return fw::fmt("{}.insert({}, {}, {}, {});", htName, hash, std::forward<FWContainer>(keys), std::forward<FWContainer>(values), localEntries);
   }
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::insert(const FWContainer& hash, const OrderedIUSet& keys, const OrderedIUSet& values, const FWContainer& localEntries) const {
   return insert(hash, genTuple(keys), genTuple(values), localEntries);
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::genStats(const FWContainer& name) const {
   return fw::lc("std::tuple<size_t, size_t, size_t, size_t> ", name, " = {0, 0, 0, 0};");
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::printStats(const FWContainer& name, size_t join_id) const {
   return fw::lc("std::cout << \"htStats", join_id, ": size: \" << ", htName, ".size() << \", hits: \"", " << ", fw::get(2, name), " << \", no_hits: \" << ", fw::get(3, name),
                 " << \", chain hit: \" << ", fw::get(0, name), " << \", chain no ht: \" << ", fw::get(1, name), " << std::endl;");
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::printStatsBuild(const FWContainer& output) const {
   return fw::lc(htName, ".genStatistics(", output, ");");
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::lookupStats(const FWContainer& hash, const FWContainer& iterator, const FWContainer& stats) const {
   return fw::lc("auto ", iterator, " = ", htName, ".lookup(", hash, ", ", stats, ");");
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::lookup(const FWContainer& hash, const FWContainer& iterator) const {
   return fw::lc("auto ", iterator, " = ", htName, ".lookup(", hash, ");");
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::lookupNext(const FWContainer& iterator) const {
   return fw::lc(iterator, " = ", htName, ".lookupNext(", iterator, ")");
}
// ---------------------------------------------------------------------------------------------------
FWContainer HashTableTranslator::couldContain(const FWContainer& hash) const {
   return fw::lc(htName, ".couldContain(", hash, ")");
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::algebra
// ---------------------------------------------------------------------------------------------------
