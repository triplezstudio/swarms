
#include "AbstractComponent.hh"

namespace swarms::core {

AbstractComponent::AbstractComponent(const ComponentType type)
  : IComponent()
  , m_componentType(type)
{}

auto AbstractComponent::type() const -> ComponentType
{
  return m_componentType;
}

} // namespace swarms::core
