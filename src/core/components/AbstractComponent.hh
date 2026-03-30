
#pragma once

#include "IComponent.hh"

namespace swarms::core {

class AbstractComponent : public IComponent
{
  public:
  ~AbstractComponent() override = default;

  auto type() const -> ComponentType override;

  protected:
  AbstractComponent(const ComponentType type);

  private:
  ComponentType m_componentType{};
};

} // namespace swarms::core
