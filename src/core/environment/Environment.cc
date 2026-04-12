
#include "Environment.hh"

namespace swarms::core {

auto Environment::createEntity() -> Uuid
{
  return m_registry.createEntity();
}

void Environment::computePreAgentsStep(const time::TickData & /*data*/) {}

void Environment::computeAgentsStep(const time::TickData &data)
{
  for (const auto &[_, agent] : m_agents)
  {
    agent->live(data);
  }
}

void Environment::computePostAgentsStep(const time::TickData & /*data*/) {}

} // namespace swarms::core