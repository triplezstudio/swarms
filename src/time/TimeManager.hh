
#pragma once

#include "ITimeManager.hh"

namespace swarms::time {

class TimeManager : public ITimeManager
{
  public:
  TimeManager(const Tick tick, const TimeStep step);
  ~TimeManager() override = default;

  auto tick(const Duration elapsed) -> TickData override;

  private:
  Tick m_currentTick{};
  TimeStep m_step{};
};

} // namespace swarms::time
