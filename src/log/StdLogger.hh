
#pragma once

#include "ILogger.hh"
#include <mutex>

namespace swarms::log {

class StdLogger : public ILogger
{
  public:
  StdLogger()           = default;
  ~StdLogger() override = default;

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

  private:
  mutable std::mutex m_locker{};
  bool m_allowLog{true};
  Severity m_severity{Severity::DEBUG};

  void logTrace(const Severity severity,
                const std::string_view message,
                const std::optional<std::string> &cause) const;
};

} // namespace swarms::log
