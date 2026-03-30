
#pragma once

#include <string>

namespace swarms::core {

enum class ComponentType
{
  ANIMAT,
  TRANSFORM,
};

auto str(const ComponentType &type) -> std::string;

} // namespace swarms::core
