
#include "Uuid.hh"

namespace swarms::core {

auto str(const Uuid id) -> std::string
{
  return std::to_string(id);
}

} // namespace swarms::core
