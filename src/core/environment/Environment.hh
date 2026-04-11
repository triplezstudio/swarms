
#pragma once

#include "AbstractEnvironment.hh"
#include "EntityRegistry.hh"
#include "IAgent.hh"
#include "Uuid.hh"
#include <unordered_map>

namespace swarms::core {

class Environment : public AbstractEnvironment
{
  public:
  Environment()           = default;
  ~Environment() override = default;

  protected:
  void computePreAgentsStep(const time::TickData &data) override;
  void computeAgentsStep(const time::TickData &data) override;
  void computePostAgentsStep(const time::TickData &data) override;

  private:
  /// @brief - Holds the collection of agents currently living in the world.
  std::unordered_map<Uuid, IAgentShPtr> m_agents{};
};

} // namespace swarms::core