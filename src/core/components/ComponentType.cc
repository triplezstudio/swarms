
#include "ComponentType.hh"

namespace swarms::core {

auto str(const ComponentType &type) -> std::string
{
  switch (type)
  {
    case ComponentType::ANIMAT:
      return "animat";
    case ComponentType::TRANSFORM:
      return "transform";
    case ComponentType::VELOCITY:
      return "velocity";
    default:
      return "unknown";
  }
}

} // namespace swarms::core
