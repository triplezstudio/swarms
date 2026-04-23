
#include "AnimatComponent.hh"

namespace swarms::core {

AnimatComponent::AnimatComponent()
  : AbstractComponent(ComponentType::ANIMAT)
{}

void AnimatComponent::plug(IAgentShPtr agent)
{
  m_agent = std::move(agent);
}

void AnimatComponent::setPerceptions(std::vector<IPerceptionPtr> perceptions)
{
  m_perceptions = std::move(perceptions);
}

auto AnimatComponent::consumeInfluences() -> std::vector<IInfluencePtr>
{
  std::vector<IInfluencePtr> out{};
  out.swap(m_influences);
  return out;
}

} // namespace swarms::core
