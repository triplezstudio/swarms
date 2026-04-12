
#pragma once

#include "IComponent.hh"
#include "TickData.hh"
#include "Uuid.hh"
#include <memory>

namespace swarms::core {

class IEnvironment
{
  public:
  IEnvironment()          = default;
  virtual ~IEnvironment() = default;

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
  virtual void addComponent(const Uuid entityId, const IComponent &component) = 0;

  /// @brief - Convenience helper allowing to create a component from a list of
  /// arguments and call the `addComponent` interface method with the newly
  /// created component.
  /// This method is meant as a flexible interface allowing to automatically
  /// handle the creation of the component.
  /// @param entityId - the identifier of the entity to which the component is
  /// added to
  /// @param args - the arguments needed to construct the component
  template<class Component, class... Args>
    requires std::derived_from<Component, IComponent>
  void addComponent(const Uuid entityId, Args &&...args);

  /// @brief - Ticks the world described by this environment one step forward
  /// in time. The tick data is used to determine how much time has elapsed
  /// since the last call.
  /// @param data - how much time has elapsed in the simulation since the last
  /// call to this method
  virtual void simulate(const time::TickData &data) = 0;
};

using IEnvironmentShPtr = std::shared_ptr<IEnvironment>;

} // namespace swarms::core

#include "IEnvironment.hxx"
