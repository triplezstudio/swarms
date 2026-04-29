
#include "NullLogger.hh"

namespace swarms::log {

void NullLogger::setAllowLog(const bool /*allowLog*/)
{
  // Intentionally empty
}

void NullLogger::setLevel(const Severity /*severity*/)
{
  // Intentionally empty
}

void NullLogger::verbose(const std::string_view /*module*/, const std::string_view /*message*/) const
{
  // Intentionally empty
}

void NullLogger::debug(const std::string_view /*module*/, const std::string_view /*message*/) const
{
  // Intentionally empty
}

void NullLogger::info(const std::string_view /*module*/, const std::string_view /*message*/) const
{
  // Intentionally empty
}

void NullLogger::notice(const std::string_view /*module*/, const std::string_view /*message*/) const
{
  // Intentionally empty
}

void NullLogger::warn(const std::string_view /*module*/,
                      const std::string_view /*message*/,
                      const std::optional<std::string> & /*cause*/) const
{
  // Intentionally empty
}

void NullLogger::error(const std::string_view /*module*/,
                       const std::string_view /*message*/,
                       const std::optional<std::string> & /*cause*/) const
{
  // Intentionally empty
}

} // namespace swarms::log
