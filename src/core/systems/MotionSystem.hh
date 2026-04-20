
#pragma once

#include "ISystem.hh"

namespace swarms::core {

class MotionSystem : public ISystem
{
  public:
  MotionSystem()           = default;
  ~MotionSystem() override = default;

  void update(const time::TickData &data) const override;
};

} // namespace swarms::core
