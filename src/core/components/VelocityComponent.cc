
#include "VelocityComponent.hh"
#include "VectorUtils.hh"

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

auto VelocityComponent::acceleration() const noexcept -> Eigen::Vector3d
{
  return m_acceleration;
}

auto VelocityComponent::speed() const noexcept -> Eigen::Vector3d
{
  return m_speed;
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

void VelocityComponent::simulate(const time::TickData &data)
{
  switch (m_speedMode)
  {
    case SpeedMode::FIXED:
      updateFixedSpeed(data);
      break;
    case SpeedMode::VARIABLE:
      updateVariableSpeed(data);
      break;
    default:
      throw std::invalid_argument("Unsupported speed mode "
                                  + std::to_string(static_cast<int>(m_speedMode)));
      break;
  }
}

void VelocityComponent::updateFixedSpeed(const time::TickData & /*data*/)
{
  // Intentionally empty: fixed speed means no changes to the speed.
}

namespace {
/// @brief - Defines a constant acceleration applied to moving objects in
/// the opposite direction of their current acceleration vector. This is
/// analogous to what friction would be in the real world. This value is
/// not based on physical process but rather adjusted to make the simulation
/// 'look good'.
/// It can be tweaked as needed. If needed it could also be a property of
/// the floor on which objects are evolving to account for different types
/// of terrains.
constexpr auto FRICTION_ACCELERATION = 0.5;

/// @brief - An arbitrary threshold below which a moving objects will be
/// forcibly stopped by the simulation. This allows to make objects stop
/// rather than continuing with an ever decreasing velocity. It makes for
/// cleaner animations.
constexpr auto SLOW_SPEED_STOP_THRESHOLD = 0.2;
} // namespace

void VelocityComponent::updateVariableSpeed(const time::TickData &data)
{
  // https://gamedev.stackexchange.com/questions/69404/how-should-i-implement-basic-spaceship-physics
  m_speed += m_acceleration * data.elapsed;

  Eigen::Vector3d friction = -FRICTION_ACCELERATION * data.elapsed * m_speed.normalized();
  m_speed += friction;

  const auto speedNorm = m_speed.norm();
  if (speedNorm >= m_maxSpeed)
  {
    m_speed.normalize();
    m_speed *= m_maxSpeed;
  }

  if (m_acceleration.isZero() && m_speed.norm() < SLOW_SPEED_STOP_THRESHOLD)
  {
    m_speed = Eigen::Vector3d::Zero();
  }
}

} // namespace swarms::core
