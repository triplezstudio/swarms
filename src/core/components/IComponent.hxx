
#pragma once

#include "IComponent.hh"

namespace swarms::core {

template<typename Component>
  requires std::derived_from<Component, IComponent>
inline auto IComponent::as() -> Component &
{
  return dynamic_cast<Component &>(*this);
}

template<typename Component>
  requires std::derived_from<Component, IComponent>
inline auto IComponent::as() const -> const Component &
{
  return dynamic_cast<const Component &>(*this);
}

template<typename Component>
  requires std::derived_from<Component, IComponent>
inline bool IComponent::isA() const
{
  return dynamic_cast<const Component *>(this) != nullptr;
}

} // namespace swarms::core
