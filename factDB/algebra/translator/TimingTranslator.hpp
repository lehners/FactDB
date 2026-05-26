#ifndef H_FACTDB_FACT_DB_ALGEBRA_TRANSLATOR_TIMINGTRANSLATOR_HPP
#define H_FACTDB_FACT_DB_ALGEBRA_TRANSLATOR_TIMINGTRANSLATOR_HPP
// ---------------------------------------------------------------------------------------------------
#include "factDB/queryc/writer/FWContainer.hpp"
#include <cstddef>
#include <string_view>
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
struct TimingTranslator {
   struct TimerRecordReference {
      size_t idx;
   };

   private:
   FWContainer name;
   bool useAlways;
   size_t cur_idx = 0;
   TimingTranslator* previousTimer = nullptr;

   constexpr const static std::string_view startPrefix = "timer_start_";
   constexpr const static std::string_view endPrefix = "timer_end_";

   [[nodiscard]] bool requiresTimer() const;

   [[nodiscard]] FWContainer timerName(size_t idx) const;
   [[nodiscard]] FWContainer startName() const;
   [[nodiscard]] FWContainer endName() const;
   [[nodiscard]] FWContainer now() const;

   public:
   TimingTranslator(FWContainer&& nameP, bool useTimerAlways = false);
   TimingTranslator(FWContainer&& nameP, TimingTranslator& previous, bool useTimerAlways = false);

   FWContainer start() noexcept;
   FWContainer record(TimerRecordReference& ref) noexcept;
   FWContainer end() noexcept;

   [[nodiscard]] FWContainer print(const FWContainer& ostream) const noexcept;
   [[nodiscard]] FWContainer print(const FWContainer& ostream, TimerRecordReference start, TimerRecordReference end) const noexcept;

   [[nodiscard]] FWContainer getTimeDiff(const FWContainer& varName) const noexcept;
   [[nodiscard]] FWContainer getTimeDiff(TimerRecordReference start, TimerRecordReference end, const FWContainer& varName) const noexcept;
};
// ---------------------------------------------------------------------------------------------------
} // namespace factDB
// ---------------------------------------------------------------------------------------------------
#endif // H_FACTDB_FACT_DB_ALGEBRA_TRANSLATOR_TIMINGTRANSLATOR_HPP