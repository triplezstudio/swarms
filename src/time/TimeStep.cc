
#include "TimeStep.hh"

namespace swarms::time {

TimeStep::TimeStep(const int ticks, const Duration &inDuration)
  : m_ticks(ticks)
  , m_duration(inDuration)
{}

auto TimeStep::count(const Duration &elapsed) const -> TickDuration
{
  const auto in     = elapsed.convert(m_duration.unit);
  const auto epochs = in.elapsed / static_cast<float>(m_duration.elapsed);

  return TickDuration(epochs * static_cast<float>(m_ticks));
}

auto TimeStep::operator==(const TimeStep &rhs) const -> bool
{
  const auto sameDuration = (m_duration.unit == rhs.m_duration.unit)
                            && (m_duration.elapsed == rhs.m_duration.elapsed);

  return sameDuration && (m_ticks == rhs.m_ticks);
}

} // namespace swarms::time
