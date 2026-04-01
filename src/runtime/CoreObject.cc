
#include "CoreObject.hh"
#include "Locator.hh"
#include "SafetyNet.hh"

namespace swarms::runtime {

void CoreObject::verbose(const std::string_view message) const
{
  getLogger().verbose(message);
}

void CoreObject::debug(const std::string_view message) const
{
  getLogger().debug(message);
}

void CoreObject::info(const std::string_view message) const
{
  getLogger().info(message);
}

void CoreObject::notice(const std::string_view message) const
{
  getLogger().notice(message);
}

void CoreObject::warn(const std::string_view message, const std::optional<std::string> &cause) const
{
  getLogger().warn(message, cause);
}

void CoreObject::error(const std::string &message, const std::optional<std::string> &cause) const
{
  throw CoreException(message, cause);
}

void CoreObject::error(const std::string &message, const CoreException &cause) const
{
  throw CoreException(message, cause);
}

bool CoreObject::withSafetyNet(std::function<void(void)> func, const std::string &functionName) const
{
  return launchProtected(func, functionName);
}

auto CoreObject::getLogger() const -> log::ILogger &
{
  return swarms::log::Locator::getLogger();
}

} // namespace swarms::runtime
