
#include "TimeManager.hh"
#include <cmath>

namespace swarms::time {

TimeManager::TimeManager(const Tick tick, const TimeStep step)
  : ITimeManager()
  , m_currentTick(tick)
  , m_step(step)
{}

auto TimeManager::tick(const Duration elapsed) -> TickData
{
  const auto duration = m_step.count(elapsed);
  m_currentTick += duration;

  return TickData{
    .tick    = m_currentTick,
    .elapsed = duration,
  };
}

} // namespace swarms::time
