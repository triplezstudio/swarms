
#include "VectorUtils.hh"
#include "StringUtils.hh"
#include <format>

namespace swarms::core {

auto str(const Eigen::Vector3f &v) -> std::string
{
  constexpr auto DECIMALS = 3;
  return std::format("%sx%sx%s",
                     doubleToStr(v(0), DECIMALS),
                     doubleToStr(v(1), DECIMALS),
                     doubleToStr(v(2), DECIMALS));
}

} // namespace swarms::core
