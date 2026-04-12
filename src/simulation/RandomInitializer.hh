
#pragma once

#include "CoreObject.hh"
#include "IEnvironmentInitializer.hh"
#include <eigen3/Eigen/Eigen>

namespace swarms::simulation {

struct InitializationConfig
{
  /// @brief - The dimensions of the world in three coordinates. This value
  /// will be used to generate agents within a parallelepiped centered at
  /// the origin and which dimensions along each axis are expressed by this
  /// field.
  Eigen::Vector3d dimensions{Eigen::Vector3d::Ones()};

  /// @brief - Defines the maximum radius occupied by agents. This value is
  /// used to assigna size to each agent with a value between 0 and this
  /// maximum value.
  double maxRadius{2.0};

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
  /// @param rng - the source of entropy for generating random positions
  void setup(core::IEnvironment &env, core::IRandomNumberGenerator &rng) override;

  private:
  InitializationConfig m_config{};

  struct AgentProps
  {
    Eigen::Vector3d position{};
    double radius{};
  };

  void spawnAgent(core::IEnvironment &env, AgentProps config);
};

} // namespace swarms::simulation
