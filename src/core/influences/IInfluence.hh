
#pragma once

#include "Uuid.hh"
#include <memory>
#include <optional>

namespace swarms::core {

/// Forward declaration of the environment as the influence interface is needed to define
/// the environment, which results in include cycles.
class IEnvironment;

class IInfluence
{
  public:
  IInfluence()          = default;
  virtual ~IInfluence() = default;

  /// @brief - Allows the influence to produce its impact on the world. Derived
  /// classes are expected to provide the logic of what the influence does in
  /// this method.
  /// @param emitterId - the identifier of the entity which emitted this influence.
  /// This is provided by the environment and allows to identify the source of the
  /// influence.
  /// @param environment - the environment to apply the influence in
  virtual void apply(const Uuid emitterId, IEnvironment &environment) = 0;
};

using IInfluencePtr = std::unique_ptr<IInfluence>;

} // namespace swarms::core
