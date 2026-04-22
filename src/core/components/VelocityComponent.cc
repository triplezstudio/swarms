
#include "VelocityComponent.hh"

namespace swarms::core {

VelocityComponent::VelocityComponent(const VelocityData &data)
  : AbstractComponent(ComponentType::VELOCITY)
  , m_speedMode(data.speedMode)
  , m_maxAcceleration(data.maxAcceleration)
  , m_maxSpeed(data.maxSpeed)
{
  if (data.initialSpeed)
  {
    m_speed = *data.initialSpeed;
  }
}

auto VelocityComponent::speedMode() const -> SpeedMode
{
  return m_speedMode;
}

auto VelocityComponent::acceleration() const -> Eigen::Vector3d
{
  return m_acceleration;
}

auto VelocityComponent::speed() const -> Eigen::Vector3d
{
  return m_speed;
}

auto VelocityComponent::maxSpeed() const -> double
{
  return m_maxSpeed;
}

void VelocityComponent::accelerate(const Eigen::Vector3d &direction)
{
  const Eigen::Vector3d nDir = direction.normalized();
  m_acceleration             = nDir * m_maxAcceleration;
}

void VelocityComponent::overrideAcceleration(const Eigen::Vector3d &acceleration)
{
  m_acceleration = acceleration;
}

void VelocityComponent::setSpeed(const Eigen::Vector3d &direction)
{
  const Eigen::Vector3d nDir = direction.normalized();
  m_speed                    = nDir * m_maxSpeed;
}

void VelocityComponent::overrideSpeed(const Eigen::Vector3d &speed)
{
  m_speed = speed;
}

void VelocityComponent::immobilize()
{
  m_acceleration = Eigen::Vector3d::Zero();
  m_speed        = Eigen::Vector3d::Zero();
}

} // namespace swarms::core
