
#include "FrustumComponent.hh"

namespace swarms::core {

FrustumComponent::FrustumComponent(Frustum frustum)
  : AbstractComponent(ComponentType::FRUSTUM)
  , m_frustum(std::move(frustum))
{}

auto FrustumComponent::frustum() const -> const Frustum &
{
  return m_frustum;
}

} // namespace swarms::core
