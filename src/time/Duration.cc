
#include "Duration.hh"
#include <stdexcept>

namespace swarms::time {

auto Duration::str() const -> std::string
{
  return std::to_string(elapsed) + asTimeString(unit);
}

namespace {
constexpr auto MS_IN_A_SECOND = 1000.0;

auto toMilliseconds(const Unit unit, const double in) -> double
{
  switch (unit)
  {
    case Unit::MILLISECONDS:
      return in;
    case Unit::SECONDS:
      return MS_IN_A_SECOND * in;
    default:
      throw std::invalid_argument("Unsupported time unit " + time::str(unit));
  }
}

auto toSeconds(const Unit unit, const double in) -> double
{
  switch (unit)
  {
    case Unit::MILLISECONDS:
      return in / MS_IN_A_SECOND;
    case Unit::SECONDS:
      return in;
    default:
      throw std::invalid_argument("Unsupported time unit " + time::str(unit));
  }
}
} // namespace

auto Duration::convert(const Unit out) const -> Duration
{
  double value{0.0};

  switch (out)
  {
    case Unit::MILLISECONDS:
      value = toMilliseconds(unit, elapsed);
      break;
    case Unit::SECONDS:
      value = toSeconds(unit, elapsed);
      break;
    default:
      throw std::invalid_argument("Unsupported time unit " + time::str(out));
  }

  return Duration{.unit = out, .elapsed = value};
}

auto Duration::fromSeconds(const double duration) -> Duration
{
  return Duration{.unit = Unit::SECONDS, .elapsed = duration};
}

auto Duration::fromMilliseconds(const double duration) -> Duration
{
  return Duration{.unit = Unit::MILLISECONDS, .elapsed = duration};
}

} // namespace swarms::time
