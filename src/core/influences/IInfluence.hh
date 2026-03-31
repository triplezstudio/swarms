
#pragma once

#include "Uuid.hh"
#include <memory>
#include <optional>

namespace swarms::core {

class IInfluence
{
  public:
  IInfluence()          = default;
  virtual ~IInfluence() = default;

  /// @brief - Returns the identifier of the source entity of this influence
  /// @return - the identifier of the source
  virtual auto getEmitter() const -> Uuid = 0;

  /// @brief - Returns the identifier of the entity which should receive this
  /// influence. In case the return value is empty, it means that the influence
  /// does not apply to a specific entity.
  /// @return - the identifier of the receiver
  virtual auto tryGetReceiver() const -> std::optional<Uuid> = 0;
};

using IInfluencePtr = std::unique_ptr<IInfluence>;

} // namespace swarms::core
