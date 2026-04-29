
#include "MotionSystem.hh"
#include "EntityRegistry.hh"
#include "TransformComponent.hh"
#include "VectorUtils.hh"
#include "VelocityComponent.hh"

#include <iostream>

namespace swarms::core {
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

void updateVelocity(VelocityComponent &velocity, const time::TickData &data)
{
  // https://gamedev.stackexchange.com/questions/69404/how-should-i-implement-basic-spaceship-physics
  Eigen::Vector3d acceleration = velocity.acceleration();
  Eigen::Vector3d speed        = velocity.speed();
  speed += acceleration * data.elapsed;

  Eigen::Vector3d friction = -FRICTION_ACCELERATION * data.elapsed * speed.normalized();
  speed += friction;

  const auto speedNorm = speed.norm();
  if (speedNorm >= velocity.maxSpeed())
  {
    speed.normalize();
    speed *= velocity.maxSpeed();
  }

  if (acceleration.isZero() && speed.norm() < SLOW_SPEED_STOP_THRESHOLD)
  {
    speed = Eigen::Vector3d::Zero();
  }

  velocity.overrideSpeed(speed);
}

void simulate(TransformComponent &transform, VelocityComponent &velocity, const time::TickData &data)
{
  if (velocity.speedMode() != SpeedMode::FIXED)
  {
    updateVelocity(velocity, data);
  }

  const Eigen::Vector3d speed = velocity.speed();
  Eigen::Vector3d dv          = speed * data.elapsed;
  transform.translate(dv);
}
} // namespace

void MotionSystem::update(const time::TickData &data, EntityRegistry &registry) const
{
  registry.apply<TransformComponent, VelocityComponent>(
    [&data](TransformComponent &transform, VelocityComponent &velocity) {
      simulate(transform, velocity, data);
    });
}

} // namespace swarms::core
