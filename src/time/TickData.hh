
#pragma once

#include "Tick.hh"
#include "TickDuration.hh"

namespace swarms::time {

struct TickData
{
  Tick tick{};
  TickDuration elapsed{};
};

} // namespace swarms::time
