
#pragma once

#include "Unit.hh"
#include <string>

namespace swarms::time {

struct Duration
{
  Unit unit{Unit::SECONDS};
  double elapsed{0.0};

  auto str() const -> std::string;
  auto convert(const Unit out) const -> Duration;

  static auto fromSeconds(const double duration) -> Duration;
  static auto fromMilliseconds(const double duration) -> Duration;
};

} // namespace swarms::time
