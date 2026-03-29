
#pragma once

#include "IBoundingBox.hh"
#include "Uuid.hh"
#include <memory>

namespace swarms::core {

class IPerception
{
  public:
  IPerception()          = default;
  virtual ~IPerception() = default;

  /// @brief - Returns the identifier of the entity represented by this
  /// perception. This allows agents to emit influences targeting the
  /// entity described by this perception.
  /// @return - the identifier of the entity described by this perception
  auto getUuid() const -> Uuid;

  /// @brief - Returns a reference to the bounding box of this perception
  /// in the world. This allows the agents perceiving this object to get
  /// information about its extend, position and orientation.
  /// @return - a reference to the bounding box of the entity represented
  /// by this perception
  auto bbox() const -> const IBoundingBox &;
};

using IPerceptionPtr = std::unique_ptr<IPerception>;

} // namespace swarms::core
