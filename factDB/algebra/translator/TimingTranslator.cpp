// ---------------------------------------------------------------------------------------------------
#include "factDB/algebra/translator/TimingTranslator.hpp"
#include "factDB/infra/Setting.hpp"
#include "factDB/queryc/PerformanceRecord.hpp"
#include "factDB/queryc/writer/FWUtil.hpp"
// ---------------------------------------------------------------------------------------------------
namespace factDB {
// ---------------------------------------------------------------------------------------------------
TimingTranslator::TimingTranslator(FWContainer&& nameP, TimingTranslator& previous, bool useTimerAlways)
   : name(std::move(nameP)), useAlways(useTimerAlways), previousTimer(&previous) {
}
// ---------------------------------------------------------------------------------------------------
TimingTranslator::TimingTranslator(FWContainer&& nameP, bool useTimerAlways) : name(std::move(nameP)), useAlways(useTimerAlways) {
}
// ---------------------------------------------------------------------------------------------------
bool TimingTranslator::requiresTimer() const {
   auto* timePipes = SettingBase::getSetting<bool>("time.pipeline");
   return useAlways || (timePipes && timePipes->get());
}
// ---------------------------------------------------------------------------------------------------
FWContainer TimingTranslator::now() const {
   return fw::lc("std::chrono::steady_clock::now();");
}
// ---------------------------------------------------------------------------------------------------
FWContainer TimingTranslator::endName() const {
   return timerName(cur_idx - 1);
}
// ---------------------------------------------------------------------------------------------------
FWContainer TimingTranslator::startName() const {
   return previousTimer == nullptr ? fw::lc(startPrefix, name) : previousTimer->endName();
}
// ---------------------------------------------------------------------------------------------------
FWContainer TimingTranslator::timerName(size_t idx) const {
   return fw::lc(name, idx);
}
// ---------------------------------------------------------------------------------------------------
FWContainer TimingTranslator::record(TimingTranslator::TimerRecordReference& ref) noexcept {
   if (requiresTimer()) {
      ref.idx = cur_idx++;
      return fw::fmt("auto {} = {}", timerName(0), now());
   } else {
      return fw::nop();
   }
}
// ---------------------------------------------------------------------------------------------------
FWContainer TimingTranslator::end() noexcept {
   assert(cur_idx != 0);
   TimerRecordReference ref{};
   return record(ref);
}
// ---------------------------------------------------------------------------------------------------
FWContainer TimingTranslator::print(const FWContainer& ostream) const noexcept {
   auto unitString = factDB::queryc::TimeUnits::getUnitString();
   return requiresTimer() ? fw::fmt(R"({} << "{}: " << factDB::queryc::TimeUnits::scaleTime({}, {}) << "{}" << std::endl;)", ostream, name, startName(), endName(), unitString) : fw::nop();
}
// ---------------------------------------------------------------------------------------------------
FWContainer TimingTranslator::print(const FWContainer& ostream, TimingTranslator::TimerRecordReference start, TimingTranslator::TimerRecordReference end) const noexcept {
   auto unitString = factDB::queryc::TimeUnits::getUnitString();
   return requiresTimer() ? fw::fmt(R"({} << "{}: " << factDB::queryc::TimeUnits::scaleTime({}, {}) << "{}" << std::endl;)", ostream, name, timerName(start.idx), timerName(end.idx), unitString) : fw::nop();
}
// ---------------------------------------------------------------------------------------------------
FWContainer TimingTranslator::getTimeDiff(const FWContainer& varName) const noexcept {
   return requiresTimer() ? fw::fmt("auto {} = std::chrono::duration<double>({} - {}{}).count()", varName, endName(), startName(), name) : fw::nop();
}
// ---------------------------------------------------------------------------------------------------
FWContainer TimingTranslator::getTimeDiff(TimingTranslator::TimerRecordReference start, TimingTranslator::TimerRecordReference end, const FWContainer& varName) const noexcept {
   return requiresTimer() ? fw::fmt("auto {} = std::chrono::duration<double>({} - {}{}).count()", varName, timerName(end.idx), timerName(start.idx), name) : fw::nop();
}
// ---------------------------------------------------------------------------------------------------
FWContainer TimingTranslator::start() noexcept {
   assert(cur_idx++ == 0);
   return requiresTimer() ? fw::fmt("auto {} = {}", timerName(0), now()) : fw::nop();
}
// ---------------------------------------------------------------------------------------------------
} // namespace factDB