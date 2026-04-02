
#include "CoreObject.hh"
#include "Locator.hh"
#include "SafetyNet.hh"

namespace swarms::runtime {

CoreObject::CoreObject(const std::string &module)
  : m_logger(module)
{}

void CoreObject::verbose(const std::string_view message) const
{
  m_logger.verbose(message);
}

void CoreObject::debug(const std::string_view message) const
{
  m_logger.debug(message);
}

void CoreObject::info(const std::string_view message) const
{
  m_logger.info(message);
}

void CoreObject::notice(const std::string_view message) const
{
  m_logger.notice(message);
}

void CoreObject::warn(const std::string_view message, const std::optional<std::string> &cause) const
{
  m_logger.warn(message, cause);
}

void CoreObject::error(const std::string &message, const std::optional<std::string> &cause) const
{
  throw CoreException(m_logger.getModule(), message, cause);
}

void CoreObject::error(const std::string &message, const CoreException &cause) const
{
  throw CoreException(m_logger.getModule(), message, cause);
}

bool CoreObject::withSafetyNet(std::function<void(void)> func, const std::string &functionName) const
{
  return launchProtected(func, m_logger.getModule(), functionName);
}

void CoreObject::addModule(const std::string &module)
{
  m_logger.addModule(module);
}

} // namespace swarms::runtime
