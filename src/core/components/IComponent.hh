
#pragma once

#include "ComponentType.hh"
#include "TickData.hh"
#include <memory>

namespace swarms::core {

class IComponent
{
  public:
  IComponent()          = default;
  virtual ~IComponent() = default;

  virtual auto type() const -> ComponentType = 0;

  /// @brief - Attempts to cast this component to the specified derived
  /// type. If the component is not an instance of the derived type, an
  /// error will be raised.
  /// This function can be used to access the derived types from the
  /// interface.
  /// @return - a reference to the derived component
  template<typename Component>
    requires std::derived_from<Component, IComponent>
  auto as() -> Component &;

  /// @brief - Attempts to cast this component to the specified derived
  /// type. If the component is not an instance of the derived type, an
  /// error will be raised.
  /// This function can be used to access the derived types from the
  /// interface.
  /// @return - a const reference to the derived component
  template<typename Component>
    requires std::derived_from<Component, IComponent>
  auto as() const -> const Component &;

  /// @brief - Returns true when the component matches the specified derived
  /// component type. This can be used to determine whether it's safe to use
  /// `as` on this component.
  /// @return - true if this component is an instance of the derived class.
  template<typename Component>
    requires std::derived_from<Component, IComponent>
  bool isA() const;
};

using IComponentPtr = std::unique_ptr<IComponent>;

} // namespace swarms::core

#include "IComponent.hxx"
