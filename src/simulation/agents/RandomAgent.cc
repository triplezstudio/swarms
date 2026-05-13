
#include "RandomAgent.hh"
#include "MotionInfluence.hh"

namespace swarms::simulation {

void RandomAgent::live(const core::BehaviorData & /*data*/)
{
  // TODO: This behavior should be refined.
  addInfluence(std::make_unique<core::MotionInfluence>(Eigen::Vector3d::Zero()));
}

} // namespace swarms::simulation
