
#pragma once

#include "TickData.hh"
#include <memory>

namespace swarms::core {

class IAgent
{
  public:
  IAgent()          = default;
  virtual ~IAgent() = default;

  /// @brief - Requests the agent to take a decision with the perceptions
  /// made available to it. The agent is expected to produce influences
  /// that will be resolved by the environment.
  /// @param data - describes the time elapsed since the last call to this
  /// method.
  virtual void live(const time::TickData &data) = 0;

  private:
};

using IAgentShPtr = std::shared_ptr<IAgent>;

} // namespace swarms::core
