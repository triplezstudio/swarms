
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