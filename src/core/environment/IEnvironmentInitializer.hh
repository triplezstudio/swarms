
#pragma once

#include "IEnvironment.hh"
#include "IRandomNumberGenerator.hh"

namespace swarms::core {

class IEnvironmentInitializer
{
  public:
  IEnvironmentInitializer()          = default;
  virtual ~IEnvironmentInitializer() = default;

  /// @brief - Initializes the entities present in the environment. This step is
  /// meant as a way to prepare the environment for the simulation. Typical use
  /// cases include loading the data from the database, receiving it from a remote
  /// server, etc.
  /// To allow some randomness when setting up the environment, a random number
  /// generator is provided as an argument.
  /// @param env - the environment to initialize
  /// @param rng - the random number generator to use to setup the environment.
  /// Providing this value allows to make the initialization process repeatable.
  virtual void setup(IEnvironment &env, IRandomNumberGenerator &rng) = 0;
};

} // namespace swarms::core
