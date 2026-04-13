
#include "EntityRegistry.hh"

namespace swarms::core {

auto EntityRegistry::createEntity() -> Uuid
{
  const auto entity = m_registry.create();

  const auto uuid = m_nextEntity;
  ++m_nextEntity;

  m_entities.emplace(uuid, entity);

  return uuid;
}

} // namespace swarms::core
