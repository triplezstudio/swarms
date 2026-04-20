
#include "Environment.hh"
#include "MotionSystem.hh"
#include "TransformComponent.hh"
#include "VelocityComponent.hh"
#include <utility>

namespace swarms::core {

Environment::Environment()
  : AbstractEnvironment()
{
  initialize();
}

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
    case ComponentType::VELOCITY:
      registerComponent(m_registry, entityId, std::move(component.as<VelocityComponent>()));
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

void Environment::computePostAgentsStep(const time::TickData &data)
{
  std::for_each(m_systems.begin(), m_systems.end(), [this, &data](const ISystemPtr &systemPtr) {
    systemPtr->update(data, m_registry);
  });
}

void Environment::initialize()
{
  m_systems.emplace_back(std::make_unique<MotionSystem>());
}

} // namespace swarms::core