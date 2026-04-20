
#include "TimeStep.hh"

namespace swarms::time {

TimeStep::TimeStep(const int ticks, const Duration &inDuration)
  : m_ticks(ticks)
  , m_duration(inDuration)
{}

auto TimeStep::count(const Duration &elapsed) const -> TickDuration
{
  const auto in     = elapsed.convert(m_duration.unit);
  const auto epochs = in.elapsed / static_cast<double>(m_duration.elapsed);

  return TickDuration(epochs * static_cast<double>(m_ticks));
}

} // namespace swarms::time
