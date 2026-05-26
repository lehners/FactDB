#ifndef H_FACTDB_FACT_DB_INFRA_IUSET_HPP
#define H_FACTDB_FACT_DB_INFRA_IUSET_HPP
// ---------------------------------------------------------------------------------------------------
#include <algorithm>
#include <string_view>
#include <unordered_set>
#include <vector>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class IU;
// ---------------------------------------------------------------------------------------------------
struct IUHash {
   std::size_t operator()(const std::reference_wrapper<const IU>& iuRef) const;
};
// ---------------------------------------------------------------------------------------------------
struct IUEqual {
   bool operator()(const std::reference_wrapper<const IU>& lhs, const std::reference_wrapper<const IU>& rhs) const;
};
// ---------------------------------------------------------------------------------------------------
struct IUPointerHash {
   std::size_t operator()(const IU* iu) const;
};
// ---------------------------------------------------------------------------------------------------
struct IUPointerEqual {
   bool operator()(const IU* iu1, const IU* iu2) const;
};
// ---------------------------------------------------------------------------------------------------
struct OrderedIUSet;
// ---------------------------------------------------------------------------------------------------
struct IUSet : public std::unordered_set<const IU*, IUPointerHash, IUPointerEqual> {
   using SetType = std::unordered_set<const IU*, IUPointerHash, IUPointerEqual>;

   public:
   IUSet() : SetType() {}
   template <std::input_iterator Iterable>
   IUSet(Iterable&& first, Iterable&& last) : SetType(std::forward<Iterable>(first), std::forward<Iterable>(last)) {}
   IUSet(std::initializer_list<const IU*> ius) : SetType(ius) {}
   IUSet(const OrderedIUSet& orderedSet);
   template <std::same_as<IU>... IUTypes>
   IUSet(const IUTypes&... ius) : SetType({&ius...}) {}

   [[nodiscard]] IUSet intersect(const IUSet& other) const;
   [[nodiscard]] IUSet merge(const IUSet& other) const noexcept;

   IUSet operator+(const IUSet& other) const noexcept { return merge(other); }
   IUSet& operator+=(const IUSet& other) {
      this->insert(other.begin(), other.end());
      return *this;
   }

   [[nodiscard]] IUSet intersect(const OrderedIUSet& other) const;
   [[nodiscard]] IUSet difference(const OrderedIUSet& other) const;
};
// ---------------------------------------------------------------------------------------------------
struct OrderedIUSet : public std::vector<const IU*> {
   public:
   OrderedIUSet() : std::vector<const IU*>() {}
   OrderedIUSet(std::initializer_list<const IU*> ius) : vector<const IU*>(ius) {}
   template <std::same_as<IU>... IUTypes>
   OrderedIUSet(const IUTypes&... ius) : vector<const IU*>({&ius...}) {}
   OrderedIUSet(IUSet ius) : vector<const IU*>(ius.begin(), ius.end()) {}
   template <std::input_iterator Iterable>
   OrderedIUSet(Iterable&& first, Iterable&& last) : vector<const IU*>(std::forward<Iterable>(first), std::forward<Iterable>(last)) {}

   [[nodiscard]] const IU* find(const IU& iu) const;

   [[nodiscard]] inline bool contains(const IU* iu) const { return contains(*iu); }
   [[nodiscard]] bool contains(const IU& iu) const;

   OrderedIUSet& removeDuplicates();
   OrderedIUSet& merge(const OrderedIUSet& other);

   [[nodiscard]] IUSet intersect(const OrderedIUSet& other) const;
   [[nodiscard]] IUSet intersect(const IUSet& other) const;
   [[nodiscard]] IUSet difference(const IUSet& other) const;

   static bool iuOrder(const IU* first, const IU* second);
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACT_DB_INFRA_IUSET_HPP
