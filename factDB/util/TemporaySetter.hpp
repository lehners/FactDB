#ifndef FACTDB_UTIL_TEMPORARYSETTER
#define FACTDB_UTIL_TEMPORARYSETTER
// ---------------------------------------------------------------------------------------------------
#include <utility>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
template <typename T>
class TemporarySetter;
// ---------------------------------------------------------------------------------------------------
class TemporarySetterBase {
   public:
   TemporarySetterBase() = default;
   TemporarySetterBase(const TemporarySetterBase&) = delete;
   TemporarySetterBase(TemporarySetterBase&&) = default;

   virtual ~TemporarySetterBase() = default;

   template <typename T>
   static std::unique_ptr<TemporarySetterBase> create(T& var, T&& newValue);
};
// ---------------------------------------------------------------------------------------------------
template <typename T>
class TemporarySetter : public TemporarySetterBase {
   T& variable;
   T previousValue;

   public:
   TemporarySetter(T& var, T newValue) : variable(var), previousValue(std::move(var)) {
      var = std::move(newValue);
   }

   TemporarySetter(const TemporarySetter&) = delete;
   TemporarySetter(TemporarySetter&&) noexcept = default;

   ~TemporarySetter() override { variable = std::move(previousValue); }
};
// ---------------------------------------------------------------------------------------------------
template <typename T>
std::unique_ptr<TemporarySetterBase> TemporarySetterBase::create(T& var, T&& newValue) {
   return std::make_unique<TemporarySetter<T>>(var, std::forward<T>(newValue));
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // FACTDB_UTIL_TEMPORARYSETTER
