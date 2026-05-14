
#pragma once

#include "IRandomNumberGenerator.hh"
#include "TickData.hh"

namespace swarms::core {

struct BehaviorData
{
  time::TickData data{};
  IRandomNumberGenerator &rng;
};

} // namespace swarms::core
