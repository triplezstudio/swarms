
#pragma once

#include "IAgent.hh"
#include "IInfluence.hh"

namespace swarms::core {

class AbstractAgent : public IAgent
{
  public:
  AbstractAgent()           = default;
  ~AbstractAgent() override = default;

  auto getAnimat() const -> AnimatShPtr override;

  protected:
  /// @brief - Used by derived classes to register a new influence from the
  /// agent. This influence will be made available to the environment and
  /// processed in the next simulation step.
  /// An agent can register multiple influences per simulation tick.
  /// @param influence - the influence to register
  void addInfluence(IInfluencePtr influence);

  private:
  AnimatShPtr m_animat{std::make_shared<Animat>()};
};

} // namespace swarms::core
