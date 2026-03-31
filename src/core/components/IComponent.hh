
#pragma once

#include "ComponentType.hh"
#include "TickData.hh"
#include <memory>

namespace swarms::core {

class IComponent
{
  public:
  IComponent()          = default;
  virtual ~IComponent() = default;

  virtual auto type() const -> ComponentType = 0;

  /// @brief - Updates the internal data of the component based on the elapsed
  /// time since the last frame. A typical example of the use of this behavior
  /// include depleting a stock of a resource that gets used over time.
  /// @param data - the elapsed time since the last call to this method
  virtual void simulate(const time::TickData &data) = 0;
};

using IComponentShPtr = std::shared_ptr<IComponent>;

} // namespace swarms::core
