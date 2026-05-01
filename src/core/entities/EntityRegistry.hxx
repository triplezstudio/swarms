
#pragma once

#include "EntityRegistry.hh"

namespace swarms::core {

template<typename Component>
inline void EntityRegistry::addComponent(const Uuid entityId, Component &&component)
{
  const auto maybeEntity = m_entityToId.find(entityId);
  if (maybeEntity == m_entityToId.end())
  {
    throw std::invalid_argument("No such entity " + str(entityId));
  }

  m_registry.emplace<Component>(maybeEntity->second, std::forward<Component>(component));
}

template<typename... Components, typename Func>
inline void EntityRegistry::apply(Func &&modifier)
{
  auto view = m_registry.view<Components...>();
  view.each([&](Components &...comps) { modifier(comps...); });
}

template<typename... Components, typename Func>
inline void EntityRegistry::applyWithId(Func &&modifier)
{
  auto view = m_registry.view<Components...>();
  view.each([&](entt::entity entity, Components &...comps) {
    modifier(m_idToEntity.at(entity), comps...);
  });
}

} // namespace swarms::core
