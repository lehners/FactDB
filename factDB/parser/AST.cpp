#include "factDB/parser/AST.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB::parser {
// ---------------------------------------------------------------------------------------------------
std::vector<TableRef*> Select::getFrom() const {
   std::vector<TableRef*> ret;
   ret.reserve(from.size());

   for (auto& f : from)
      ret.emplace_back(TableRef::dynCast(f.get()));

   return ret;
}
// ---------------------------------------------------------------------------------------------------
std::vector<Target*> Select::getTargets() const {
   std::vector<Target*> ret;
   ret.reserve(targets.size());

   for (auto& t : targets)
      ret.emplace_back(Target::dynCast(t.get()));

   return ret;
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::parser
// ---------------------------------------------------------------------------------------------------