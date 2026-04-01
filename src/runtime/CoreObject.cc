
#include "CoreObject.hh"
#include "Locator.hh"
#include "SafetyNet.hh"

namespace swarms::runtime {

CoreObject::CoreObject(const std::string &module)
  : m_module(module)
{}

void CoreObject::verbose(const std::string_view message) const
{
  getLogger().verbose(m_module, message);
}

void CoreObject::debug(const std::string_view message) const
{
  getLogger().debug(m_module, message);
}

void CoreObject::info(const std::string_view message) const
{
  getLogger().info(m_module, message);
}

void CoreObject::notice(const std::string_view message) const
{
  getLogger().notice(m_module, message);
}

void CoreObject::warn(const std::string_view message, const std::optional<std::string> &cause) const
{
  getLogger().warn(m_module, message, cause);
}

void CoreObject::error(const std::string &message, const std::optional<std::string> &cause) const
{
  throw CoreException(m_module, message, cause);
}

void CoreObject::error(const std::string &message, const CoreException &cause) const
{
  throw CoreException(m_module, message, cause);
}

bool CoreObject::withSafetyNet(std::function<void(void)> func, const std::string &functionName) const
{
  return launchProtected(func, m_module, functionName);
}

auto CoreObject::getLogger() const -> log::ILogger &
{
  return swarms::log::Locator::getLogger();
}

} // namespace swarms::runtime
