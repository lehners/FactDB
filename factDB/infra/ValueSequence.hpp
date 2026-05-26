#ifndef H_factdb_infra_ValueSequence
#define H_factdb_infra_ValueSequence
// ---------------------------------------------------------------------------------------------------
#include <ostream>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra {
// ---------------------------------------------------------------------------------------------------
template <auto... Vals>
struct value_sequence {
   using type_info = value_sequence<Vals...>;

   template <auto Val>
   struct value {
      static constexpr auto iter_single(auto fun) {
         fun(Val);
      }
      static constexpr auto filter_single(auto predicate) {
         if constexpr (predicate(Val)) {
            return value_sequence<Val>{};
         } else {
            return value_sequence<>{};
         }
      }
   };

   static constexpr void iter(auto fun) {
      (value<Vals>::iter_single(fun), ...);
   }

   static constexpr auto filter(auto predicate) {
      return (value<Vals>::filter_single(predicate) + ...);
   }

   template <int... predicate_vals>
   static constexpr auto filter_not_contains(value_sequence<predicate_vals...>) {
      auto predicate = [](int i) constexpr { return ((i != predicate_vals) && ...); };
      return (value<Vals>::filter_single(predicate) + ...);
   }

   template <int... predicate_vals>
   static constexpr auto filter_contains(value_sequence<predicate_vals...>) {
      auto predicate = [](int i) constexpr { return ((i == predicate_vals) || ...); };
      return (value<Vals>::filter_single(predicate) + ...);
   }

   static void print(std::ostream& stream) {
      iter([&](int i) constexpr { stream << i << "\t"; });
      stream << std::endl;
   }

   static constexpr bool empty() {
      bool empty_ = false;
      iter([&](int /*i*/) constexpr { empty_ = false; });
      return empty_;
   }
};

template <auto... As, auto... Bs>
constexpr value_sequence<As..., Bs...> operator+(value_sequence<As...>, value_sequence<Bs...>) {
   return {};
}

// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra
// ---------------------------------------------------------------------------------------------------
#endif // H_factdb_infra_ValueSequence
