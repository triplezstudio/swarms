
#include "MotionSystem.hh"
#include "VectorUtils.hh"

namespace swarms::core {
// namespace {
// bool isEntityRelevant(const Entity &ent)
// {
//   return ent.exists<TransformComponent>() && ent.exists<VelocityComponent>();
// }
// } // namespace

void MotionSystem::update(const time::TickData & /*data*/) const
{
  // auto &velocity  = entity.velocityComp();
  // auto &transform = entity.transformComp();

  // velocity.update(data);

  // const Eigen::Vector3f speed = velocity.speed();
  // Eigen::Vector3f dv          = speed * data.elapsed;
  // transform.translate(dv);
  // if (!speed.isZero())
  // {
  //   transform.setHeading(std::atan2(speed(0), speed(1)));
  // }
}

} // namespace swarms::core
