
#pragma once

#include "IInfluence.hh"
#include "IPerception.hh"
#include <memory>
#include <vector>

namespace swarms::core {

// Note: not adding constructors and destructors allows the class to be movable.
// This is necessary to work with entt machinery due to the unique pointers in
// the vectors.
class Animat
{
  public:
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
  std::vector<IPerceptionPtr> m_perceptions{};
  std::vector<IInfluencePtr> m_influences{};
};

using AnimatShPtr = std::shared_ptr<Animat>;

} // namespace swarms::core
