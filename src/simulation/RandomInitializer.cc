
#include "RandomInitializer.hh"
#include <ranges>

namespace swarms::simulation {

RandomInitializer::RandomInitializer(InitializationConfig config)
  : core::IEnvironmentInitializer()
  , runtime::CoreObject("initializer")
  , m_config(std::move(config))
{
  addModule("random");
}

void RandomInitializer::setup(core::IEnvironment &env)
{
  for (unsigned id = 0u; id < m_config.agentsCount; ++id)
  {
    spawnAgent(env);
  }
}

void RandomInitializer::spawnAgent(core::IEnvironment &env)
{
  const auto entityId = env.createEntity();
  debug("Spawned entity " + core::str(entityId));
}

} // namespace swarms::simulation
