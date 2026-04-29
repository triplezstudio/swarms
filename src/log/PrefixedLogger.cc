
#include "PrefixedLogger.hh"
#include "Locator.hh"

namespace swarms::log {
namespace {
auto consolidate(const std::string &str) -> std::string
{
  if (str.empty() || str[0] == '[')
  {
    return str;
  }

  return "[" + str + "]";
}
} // namespace

PrefixedLogger::PrefixedLogger(const std::string &module)
  : m_modules(consolidate(module))
{}

auto PrefixedLogger::getModule() const -> std::string
{
  return m_modules;
}

void PrefixedLogger::setModule(const std::string &module)
{
  if (!module.empty())
  {
    m_modules = consolidate(module);
  }
}

void PrefixedLogger::addModule(const std::string &module)
{
  if (!module.empty())
  {
    m_modules += " " + consolidate(module);
  }
}

void PrefixedLogger::verbose(const std::string_view message) const
{
  Locator::getLogger().verbose(m_modules, message);
}

void PrefixedLogger::debug(const std::string_view message) const
{
  Locator::getLogger().debug(m_modules, message);
}

void PrefixedLogger::info(const std::string_view message) const
{
  Locator::getLogger().info(m_modules, message);
}

void PrefixedLogger::notice(const std::string_view message) const
{
  Locator::getLogger().notice(m_modules, message);
}

void PrefixedLogger::warn(const std::string_view message,
                          const std::optional<std::string> &cause) const
{
  Locator::getLogger().warn(m_modules, message, cause);
}

void PrefixedLogger::error(const std::string_view message,
                           const std::optional<std::string> &cause) const
{
  Locator::getLogger().error(m_modules, message, cause);
}

} // namespace swarms::log
