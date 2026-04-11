
#pragma once

#include "IComponent.hh"
#include "Uuid.hh"
#include <entt/entt.hpp>
#include <unordered_map>

namespace swarms::core {

class EntityRegistry
{
  public:
  EntityRegistry()  = default;
  ~EntityRegistry() = default;

  /// @brief - Creates a new (empty) entity and returns it. The returned identifier can
  /// be used later on to modify the entity for example when adding components.
  /// This is currently not thread-safe.
  /// @return - the identifier of the entity
  auto createEntity() -> Uuid;

  template<typename Component>
  void addComponent(const Uuid entityId, Component component);

  template<typename... Components, typename Func>
  void apply(Func &&modifier);

  private:
  /// @brief - The registry used by entt to store entities.
  entt::registry m_registry{};

  /// @brief - A table of association between the identifier of the entities as defined
  /// by `entt` to agnostic identifier that can be communicated to the rest of the app
  /// without leaking the library details.
  std::unordered_map<Uuid, entt::entity> m_entities{};

  Uuid m_nextEntity{Uuid(0)};
};

} // namespace swarms::core

#include "EntityRegistry.hxx"
