
#include "MotionInfluence.hh"
#include "IEnvironment.hh"

namespace swarms::core {

MotionInfluence::MotionInfluence(Eigen::Vector3d acceleration)
  : m_acceleration(std::move(acceleration))
{}

void MotionInfluence::apply(const Uuid /*emitterId*/, IEnvironment & /*environment*/)
{
  // TODO: This should modify the agent's body.
  // We need a `applyTo(const Uuid entityId, lambda) function or simular in the environment.
}

} // namespace swarms::core
