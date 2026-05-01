
#pragma once

#include "AbstractEnvironment.hh"
#include "EntityRegistry.hh"
#include "IAgent.hh"
#include "ISystem.hh"
#include "Uuid.hh"
#include <unordered_map>

namespace swarms::core {

class Environment : public AbstractEnvironment
{
  public:
  Environment();
  ~Environment() override = default;

  auto createEntity() -> Uuid override;
  void addComponent(const Uuid entityId, IComponent &&component) override;

  /// @brief - Implementation of the interface method to attach an agent to an
  /// entity. The process to attach an agent is:
  ///  - create an animat component for the entity
  ///  - attach the agent's animat to this component
  ///  - register the agent in the internal list
  /// In case the entity is already assigned to an agent, an error is raised.
  /// @param entityId - the identifier of the entity to which the agent should
  /// be attached to
  /// @param agent - the agent to create
  void attachAgent(const Uuid entityId, IAgentShPtr agent) override;

  protected:
  void computePreAgentsStep(const time::TickData &data) override;
  void computeAgentsStep(const time::TickData &data) override;
  void computePostAgentsStep(const time::TickData &data) override;

  private:
  /// @brief - Holds the collection of agents currently living in the world.
  std::unordered_map<Uuid, IAgentShPtr> m_agents{};

  /// @brief - The registry storing entities and components living  in the
  /// environment.
  EntityRegistry m_registry{};

  /// @brief - Holds the collection of systems used to update entities in
  /// each tick of the simulation.
  std::vector<ISystemPtr> m_systems{};

  void initialize();
};

} // namespace swarms::core