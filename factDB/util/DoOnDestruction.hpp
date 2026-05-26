#ifndef FACTDB_UTIL_DOONDESTRUCTION_H
#define FACTDB_UTIL_DOONDESTRUCTION_H
// ---------------------------------------------------------------------------------------------------
#include <functional>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
class DoOnDestruction {
   public:
   using FunctionType = std::function<void()>;

   private:
   FunctionType fun;

   public:
   DoOnDestruction(FunctionType fun_) : fun(std::move(fun_)) {}
   DoOnDestruction(const DoOnDestruction&) = delete;
   DoOnDestruction(DoOnDestruction&&) = default;

   virtual ~DoOnDestruction() { fun(); }
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // FACTDB_UTIL_DOONDESTRUCTION_H
