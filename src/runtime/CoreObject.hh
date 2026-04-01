
#pragma once

#include "CoreException.hh"
#include "ILogger.hh"
#include <functional>
#include <string>

namespace swarms::runtime {

class CoreObject
{
  public:
  /// @brief - Creates a new core object with the specified module name. The
  /// module represents a unit that further specify the purpose of the class.
  /// Typical examples include "network", "client", "input", etc. This helps
  /// triage the logs per source.
  /// @param module - the module describing the broad category of the object.
  CoreObject(const std::string &module);

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
  std::string m_module{};

  auto getLogger() const -> log::ILogger &;
};

} // namespace swarms::runtime
