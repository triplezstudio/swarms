
#pragma once

#include "IAgent.hh"

namespace swarms::core {

class AbstractAgent : public IAgent
{
  public:
  AbstractAgent()           = default;
  ~AbstractAgent() override = default;

  auto getAnimat() const -> AnimatShPtr override;

  private:
  AnimatShPtr m_animat{std::make_shared<Animat>()};
};

} // namespace swarms::core
