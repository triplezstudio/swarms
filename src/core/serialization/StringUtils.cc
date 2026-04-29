
#include "StringUtils.hh"
#include <format>

namespace swarms::core {

auto doubleToStr(const double value, const int decimals) -> std::string
{
  // https://en.cppreference.com/w/cpp/utility/format/spec.html
  return std::format("{:{}f}", value, decimals);
}

} // namespace swarms::core
