
#include "StdLogger.hh"
#include "StreamFormatter.hh"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace swarms::log {

void StdLogger::setAllowLog(const bool allowLog)
{
  const std::lock_guard guard(m_locker);
  m_allowLog = allowLog;
}

void StdLogger::setLevel(const Severity severity)
{
  const std::lock_guard guard(m_locker);
  m_severity = severity;
}

void StdLogger::verbose(const std::string_view message) const
{
  logTrace(Severity::VERBOSE, message, {});
}

void StdLogger::debug(const std::string_view message) const
{
  logTrace(Severity::DEBUG, message, {});
}

void StdLogger::info(const std::string_view message) const
{
  logTrace(Severity::INFO, message, {});
}

void StdLogger::notice(const std::string_view message) const
{
  logTrace(Severity::NOTICE, message, {});
}

void StdLogger::warn(const std::string_view message, const std::optional<std::string> &cause) const
{
  logTrace(Severity::WARNING, message, cause);
}

void StdLogger::error(const std::string_view message, const std::optional<std::string> &cause) const
{
  logTrace(Severity::ERROR, message, cause);
}

namespace {
bool canBeDisplayed(const Severity severity, const Severity reference)
{
  return reference <= severity;
}

auto getTimestampAsStr() -> std::string
{
  auto currentTime      = std::time(nullptr);
  const auto *localTime = std::localtime(&currentTime);

  std::stringstream out;
  out << std::put_time(localTime, "%d-%m-%Y %H:%M:%S");

  return out.str();
}
} // namespace

void StdLogger::logTrace(const Severity severity,
                         const std::string_view message,
                         const std::optional<std::string> &cause) const
{
  const std::lock_guard guard(m_locker);
  if (!m_allowLog || !canBeDisplayed(severity, m_severity))
  {
    return;
  }

  std::stringstream out;

  setStreamColor(out, Color::MAGENTA);
  out << getTimestampAsStr() << " ";

  setStreamColorFromSeverity(out, severity);
  out << "[" << str(severity) << "] ";
  clearStreamFormat(out);

  out << message;

  if (cause.has_value())
  {
    out << " (cause: \"" << *cause << "\")";
  }

  std::cout << out.str() << std::endl;
}

} // namespace swarms::log
