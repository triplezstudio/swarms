
#pragma once

#include "Duration.hh"
#include "TickDuration.hh"

namespace swarms::time {

class TimeStep
{
  public:
  /// @brief - Creates a default time step in which every 100 milliseconds in
  /// the real world amounts to 1 tick.
  TimeStep() = default;

  /// @brief - Creates a time step where the specified amount of `ticks` are
  /// represented by the real world duration.
  /// @param ticks - the number of ticks
  /// @param inDuration - existing in this real world duration
  TimeStep(const int ticks, const Duration &inDuration);

  ~TimeStep() = default;

  /// @brief - Converts the duration in real world seconds to a duration
  /// expressed in ticks.
  /// @param elapsed - the real world duration
  /// @return - the equivalent duration in ticks
  auto count(const Duration &elapsed) const -> TickDuration;

  bool operator==(const TimeStep &rhs) const;

  private:
  int m_ticks{1};
  Duration m_duration{.unit = Unit::MILLISECONDS, .elapsed = 100.0};
};

} // namespace swarms::time
