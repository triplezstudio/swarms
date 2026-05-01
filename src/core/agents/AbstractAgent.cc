
#include "AbstractAgent.hh"

namespace swarms::core {

auto AbstractAgent::getAnimat() const -> AnimatShPtr
{
  return m_animat;
}

} // namespace swarms::core
