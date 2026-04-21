
#pragma once

#include "ISystem.hh"

namespace swarms::core {

class MotionSystem : public ISystem
{
  public:
  MotionSystem()           = default;
  ~MotionSystem() override = default;

  void update(const time::TickData &data, EntityRegistry &registry) const override;
};

} // namespace swarms::core
