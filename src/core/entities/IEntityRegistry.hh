
#pragma once

#include "IComponent.hh"
#include "Uuid.hh"

namespace swarms::core {

class IEntityRegistry
{
  public:
  IEntityRegistry()          = default;
  virtual ~IEntityRegistry() = default;

  /// @brief - Creates a new empty entity in the environment. The identifer
  /// returned can be used to access the entity in the future.
  /// @return - the identifier of the created entity.
  virtual auto createEntity() -> Uuid = 0;

  /// @brief - Registers a new component and attaches it to the entity pointed
  /// at by the identifier.
  /// Implementation are free to choose how to handle cases where the entity
  /// does not exist or when the component is not valid.
  /// @param entityId - the identifier of the entity to attach the component to
  /// @param component - the component to attach
  virtual void attachComponent(const Uuid entityId, IComponent &&component) = 0;
};

} // namespace swarms::core
