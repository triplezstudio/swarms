
#pragma once

#include "AbstractComponent.hh"
#include "Animat.hh"
#include "IAgent.hh"
#include <vector>

namespace swarms::core {

class AnimatComponent : public AbstractComponent
{
  public:
  AnimatComponent(AnimatShPtr animat);
  ~AnimatComponent() override = default;

  auto animat() -> Animat &;

  private:
  AnimatShPtr m_animat{};
};

} // namespace swarms::core
