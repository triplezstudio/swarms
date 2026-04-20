
#include "RandomInitializer.hh"
#include "CircleBox.hh"
#include "TransformComponent.hh"
#include "VectorUtils.hh"
#include "VelocityComponent.hh"

namespace swarms::simulation {

RandomInitializer::RandomInitializer(InitializationConfig config)
  : core::IEnvironmentInitializer()
  , runtime::CoreObject("initializer")
  , m_config(std::move(config))
{
  addModule("random");
}

void RandomInitializer::setup(core::IEnvironment &env, core::IRandomNumberGenerator &rng)
{
  for (unsigned id = 0u; id < m_config.agentsCount; ++id)
  {
    const auto x = rng.randomDouble(0.0, m_config.dimensions(0));
    const auto y = rng.randomDouble(0.0, m_config.dimensions(1));
    const auto z = rng.randomDouble(0.0, m_config.dimensions(2));

    const auto r = rng.randomDouble(0.0, m_config.maxRadius);

    AgentProps config{
      .position = Eigen::Vector3d(x, y, z),
      .radius   = r,
    };
    spawnAgent(env, std::move(config));
  }
}

void RandomInitializer::spawnAgent(core::IEnvironment &env, AgentProps config)
{
  const auto entityId = env.createEntity();

  auto box = std::make_shared<core::CircleBox>(config.position, config.radius);
  env.addComponent<core::TransformComponent>(entityId, std::move(box));

  core::VelocityData data{
    .maxAcceleration = 2.0,
    .maxSpeed        = 3.0,
    .initialSpeed    = {},
    .speedMode       = core::SpeedMode::VARIABLE,
  };
  env.addComponent<core::VelocityComponent>(entityId, data);

  debug("Spawned entity " + core::str(entityId) + " at " + core::str(config.position));
}

} // namespace swarms::simulation
