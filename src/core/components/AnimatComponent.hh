
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
  AnimatComponent(AnimatComponent &&rhs) = default;
  ~AnimatComponent() override            = default;

  /// @brief - Assigns the provided agent as the brain of the animat. This means that
  /// any decision taken by the agent will be received by the animat and exposed to
  /// the environment so that it can be applied.
  /// In case an agent is already attached to this animat, calling this function again
  /// will override it and any decision taken by the old agent will not be applied to
  /// this animat anymore.
  /// @param agent - the agent to plug to this animat
  void plug(IAgentShPtr agent);

  auto animat() -> Animat &;

  private:
  AnimatShPtr m_animat{};
};

} // namespace swarms::core
