
#include "AnimatComponent.hh"

namespace swarms::core {

AnimatComponent::AnimatComponent(AnimatShPtr animat)
  : AbstractComponent(ComponentType::ANIMAT)
  , m_animat(std::move(animat))
{
  if (m_animat == nullptr)
  {
    throw std::invalid_argument("Expected non null animat");
  }
}

auto AnimatComponent::animat() -> Animat &
{
  return *m_animat;
}

} // namespace swarms::core
