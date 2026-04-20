
#pragma once

#include "TickData.hh"
#include <memory>

namespace swarms::core {

class ISystem
{
  public:
  ISystem()          = default;
  virtual ~ISystem() = default;

  virtual void update(const time::TickData &data) const = 0;
};

using ISystemPtr = std::unique_ptr<ISystem>;

} // namespace swarms::core
