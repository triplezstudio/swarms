
#pragma once

#include "AbstractAgent.hh"

namespace swarms::simulation {

class RandomAgent : public core::AbstractAgent
{
  public:
  void live(const core::BehaviorData &data) override;
};

} // namespace swarms::simulation
