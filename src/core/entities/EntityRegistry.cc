
#include "EntityRegistry.hh"

namespace swarms::core {

auto EntityRegistry::createEntity() -> Uuid
{
  const auto entity = m_registry.create();

  const auto uuid = m_nextEntity;
  ++m_nextEntity;

  m_entityToId.emplace(uuid, entity);
  m_idToEntity.emplace(entity, uuid);

  return uuid;
}

} // namespace swarms::core
