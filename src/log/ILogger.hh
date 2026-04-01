
#pragma once

#include "Severity.hh"
#include <optional>
#include <string>

namespace swarms::log {

class ILogger
{
  public:
  ILogger()          = default;
  virtual ~ILogger() = default;

  virtual void setAllowLog(const bool allowLog)  = 0;
  virtual void setLevel(const Severity severity) = 0;

  virtual void verbose(const std::string_view message) const = 0;
  virtual void debug(const std::string_view message) const   = 0;
  virtual void info(const std::string_view message) const    = 0;
  virtual void notice(const std::string_view message) const  = 0;
  virtual void warn(const std::string_view message,
                    const std::optional<std::string> &cause = {}) const
    = 0;
  virtual void error(const std::string_view message,
                     const std::optional<std::string> &cause = {}) const
    = 0;
};

} // namespace swarms::log
