
#include "MotionSystem.hh"
#include "EntityRegistry.hh"
#include "TransformComponent.hh"
#include "VectorUtils.hh"
#include "VelocityComponent.hh"

namespace swarms::core {
namespace {
void simulate(TransformComponent &transform, VelocityComponent &velocity, const time::TickData &data)
{
  velocity.simulate(data);
  transform.simulate(data);

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
