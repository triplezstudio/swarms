
#pragma once

#include "TickData.hh"
#include "Uuid.hh"
#include <memory>

namespace swarms::core {

class IEnvironment
{
  public:
  IEnvironment()          = default;
  virtual ~IEnvironment() = default;

  /// @brief - Creates a new empty entity in the environment. The identifer
  /// returned can be used to access the entity in the future.
  /// @return - the identifier of the created entity.
  virtual auto createEntity() -> Uuid = 0;

  /// @brief - Ticks the world described by this environment one step forward
  /// in time. The tick data is used to determine how much time has elapsed
  /// since the last call.
  /// @param data - how much time has elapsed in the simulation since the last
  /// call to this method
  virtual void simulate(const time::TickData &data) = 0;
};

using IEnvironmentShPtr = std::shared_ptr<IEnvironment>;

} // namespace swarms::core
