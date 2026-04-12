
#pragma once

#include "IEnvironment.hh"

namespace swarms::core {

template<class Component, class... Args>
  requires std::derived_from<Component, IComponent>
inline void IEnvironment::addComponent(const Uuid entityId, Args &&...args)
{
  Component component(std::forward<Args>(args)...);
  addComponent(entityId, std::move(component));
}

} // namespace swarms::core
