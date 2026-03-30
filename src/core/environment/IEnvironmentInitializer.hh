
#pragma once

#include "IEnvironment.hh"

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
  /// @param env - the environment to initialize
  virtual void setup(IEnvironment &env) = 0;
};

} // namespace swarms::core
