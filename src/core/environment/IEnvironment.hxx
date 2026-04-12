
#pragma once

#include "IEnvironment.hh"

namespace swarms::core {

template<class Component, class... Args>
  requires std::derived_from<Component, IComponent>
inline void IEnvironment::addComponent(const Uuid entityId, Args &&...args)
{
  addComponent(entityId, Component(std::forward<Args>(args)...));
}

} // namespace swarms::core
