
#include "AbstractEnvironment.hh"

namespace swarms::core {

void AbstractEnvironment::simulate(const time::TickData &data)
{
  computePreAgentsStep(data);
  computeAgentsStep(data);
  computePostAgentsStep(data);
}

} // namespace swarms::core
