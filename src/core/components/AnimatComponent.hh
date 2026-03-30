
#pragma once

#include "AbstractComponent.hh"
#include "IAgent.hh"
#include "IInfluence.hh"
#include "IPerception.hh"
#include <vector>

namespace swarms::core {

class AnimatComponent : public AbstractComponent
{
  public:
  AnimatComponent();
  ~AnimatComponent() override = default;

  void simulate(const time::TickData &data) override;

  /// @brief - Assigns the provided agent as the brain of the animat. This means that
  /// any decision taken by the agent will be received by the animat and exposed to
  /// the environment so that it can be applied.
  /// In case an agent is already attached to this animat, calling this function again
  /// will override it and any decision taken by the old agent will not be applied to
  /// this animat anymore.
  /// @param agent - the agent to plug to this animat
  void plug(IAgentShPtr agent);

  /// @brief - Informs the animat of a new set of perceptions available to the agent.
  /// This function is typically called at each simulation step by the environment to
  /// allow the agents to perceive their world.
  /// @param perceptions - the perceptions currently available to the agent.
  void setPerceptions(std::vector<IPerceptionPtr> perceptions);

  /// @brief - Collects the influences available in this animat after the agent has
  /// executed its behavior. After this function is called, the animat will clear its
  /// list of influences, making it the responsibility of the caller to store them
  /// for further processing.
  /// This function is typically called each simulation step by the environment.
  /// @return - the list of influences produced by the agent
  auto consumeInfluences() -> std::vector<IInfluencePtr>;

  private:
  IAgentShPtr m_agent{};
  std::vector<IPerceptionPtr> m_perceptions{};
  std::vector<IInfluencePtr> m_influences{};
};

} // namespace swarms::core
