#ifndef H_FACTDB_FACTDB_INFRA_DONOTDESTRUCT_HPP
#define H_FACTDB_FACTDB_INFRA_DONOTDESTRUCT_HPP
// ---------------------------------------------------------------------------------------------------
#include <utility>
// ---------------------------------------------------------------------------------------------------
namespace factDB::infra {
// ---------------------------------------------------------------------------------------------------
template <typename ContainedType>
struct DoNotDestruct {
   union {
      char dummy_;
      ContainedType value_;
   };

   template <typename... Args>
   DoNotDestruct(Args&&... args) : value_(std::forward<Args>(args)...) {}

   const ContainedType& value() const { return value_; }
   ContainedType& value() { return value_; }

   const ContainedType* get() const { return &value_; }
   ContainedType* get() { return &value_; }

   const ContainedType* operator->() const { return get(); }
   ContainedType* operator->() { return get(); }

   ~DoNotDestruct() {}
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB::infra
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACTDB_INFRA_DONOTDESTRUCT_HPP