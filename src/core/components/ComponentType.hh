
#pragma once

#include <string>

namespace swarms::core {

enum class ComponentType
{
  ANIMAT,
  FRUSTUM,
  TRANSFORM,
  VELOCITY,
};

auto str(const ComponentType &type) -> std::string;

} // namespace swarms::core
