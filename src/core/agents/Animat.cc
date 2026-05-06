
#include "Animat.hh"

namespace swarms::core {

void Animat::setPerceptions(std::vector<IPerceptionPtr> perceptions)
{
  m_perceptions = std::move(perceptions);
}

auto Animat::consumeInfluences() -> std::vector<IInfluencePtr>
{
  std::vector<IInfluencePtr> out{};
  out.swap(m_influences);
  return out;
}

void Animat::addInfluence(IInfluencePtr influence)
{
  m_influences.emplace_back(std::move(influence));
}

} // namespace swarms::core
