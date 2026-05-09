
#include "AbstractAgent.hh"

namespace swarms::core {

auto AbstractAgent::getAnimat() const -> AnimatShPtr
{
  return m_animat;
}

void AbstractAgent::addInfluence(IInfluencePtr influence)
{
  m_animat->addInfluence(std::move(influence));
}

} // namespace swarms::core
