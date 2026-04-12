
#pragma once

#include "CoreObject.hh"
#include "IEnvironmentInitializer.hh"
#include <eigen3/Eigen/Eigen>

namespace swarms::simulation {

struct InitializationConfig
{
  /// @brief - The dimensions of the world in three coordinates.
  Eigen::Vector3f dimensions{Eigen::Vector3f::Ones()};

  /// @brief - Defines how many agents needs to be spawned.
  unsigned agentsCount{10u};
};

class RandomInitializer : public core::IEnvironmentInitializer, public runtime::CoreObject
{
  public:
  RandomInitializer(InitializationConfig config);
  ~RandomInitializer() override = default;

  /// @brief - Initializes the environment using the configuration provided at
  /// creation time. This will create `agentsCount` agents randomly positioned
  /// around the origin in a circle with the requested dimensions.
  //// Note: any existing entity will be preserved by this function.
  /// @param env - the environment to initialize.
  void setup(core::IEnvironment &env) override;

  private:
  InitializationConfig m_config{};

  void spawnAgent(core::IEnvironment &env);
};

} // namespace swarms::simulation
