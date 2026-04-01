
#pragma once

#include "CoreException.hh"
#include "ILogger.hh"
#include <functional>
#include <string>

namespace swarms::runtime {

class CoreObject
{
  public:
  CoreObject()          = default;
  virtual ~CoreObject() = default;

  protected:
  void verbose(const std::string_view message) const;
  void debug(const std::string_view message) const;
  void info(const std::string_view message) const;
  void notice(const std::string_view message) const;
  void warn(const std::string_view message, const std::optional<std::string> &cause = {}) const;
  void error(const std::string &message, const std::optional<std::string> &cause = {}) const;
  void error(const std::string &message, const CoreException &cause) const;

  /// @brief - Runs `func` and catch exceptions that might be thrown by it.
  /// In case an exception is caught, a message is logged with the function
  /// name provided in argument.
  /// @param func - the function to execute
  /// @param functionName - the name of the function, used to log a message
  /// in case an exception is caught
  /// @return - true if the function ran successfully, false otherwise
  bool withSafetyNet(std::function<void(void)> func, const std::string &functionName) const;

  private:
  std::string m_name{};

  auto getLogger() const -> log::ILogger &;
};

} // namespace swarms::runtime
