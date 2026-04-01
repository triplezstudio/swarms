
#pragma once

#include "ILogger.hh"

namespace swarms::log {

class NullLogger : public ILogger
{
  public:
  void setAllowLog(const bool allowLog) override;
  void setLevel(const Severity severity) override;

  void verbose(const std::string_view message) const override;
  void debug(const std::string_view message) const override;
  void info(const std::string_view message) const override;
  void notice(const std::string_view message) const override;
  void warn(const std::string_view message,
            const std::optional<std::string> &cause = {}) const override;
  void error(const std::string_view message,
             const std::optional<std::string> &cause = {}) const override;
};

} // namespace swarms::log
