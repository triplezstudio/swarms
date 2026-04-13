
#include "Environment.hh"
#include "TransformComponent.hh"
#include <utility>

namespace swarms::core {

auto Environment::createEntity() -> Uuid
{
  return m_registry.createEntity();
}

namespace {
template<typename Component>
void registerComponent(EntityRegistry &registry, const Uuid entityId, Component &&component)
{
  registry.addComponent(entityId, std::forward<Component>(component));
}
} // namespace

void Environment::addComponent(const Uuid entityId, IComponent &&component)
{
  switch (component.type())
  {
    case ComponentType::TRANSFORM:
      registerComponent(m_registry, entityId, std::move(component.as<TransformComponent>()));
      break;
    default:
      throw std::invalid_argument("Unuspported component type " + str(component.type()));
  }
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