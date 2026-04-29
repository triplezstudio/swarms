
#pragma once

#include "AbstractComponent.hh"
#include "Frustum.hh"

namespace swarms::core {

class FrustumComponent : public AbstractComponent
{
  public:
  FrustumComponent(Frustum frustum);
  ~FrustumComponent() override = default;

  auto frustum() const -> const Frustum &;

  private:
  Frustum m_frustum;
};

} // namespace swarms::core
