
#pragma once

#include "TickData.hh"

namespace swarms::core {

class IEnvironment
{
  public:
  IEnvironment()          = default;
  virtual ~IEnvironment() = default;

  /// @brief - Ticks the world described by this environment one step forward
  /// in time. The tick data is used to determine how much time has elapsed
  /// since the last call.
  /// @param data - how much time has elapsed in the simulation since the last
  /// call to this method
  virtual void simulate(const time::TickData &data) = 0;
};

} // namespace swarms::core
