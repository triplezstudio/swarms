
#pragma once

#include "IInfluence.hh"
#include <eigen3/Eigen/Eigen>

namespace swarms::core {

class MotionInfluence : public IInfluence
{
  public:
  MotionInfluence(Eigen::Vector3d acceleration);
  ~MotionInfluence() override = default;

  void apply(const Uuid emitterId, IEnvironment &environment) override;

  private:
  Eigen::Vector3d m_acceleration{};
};

} // namespace swarms::core
