
#pragma once

#include "ILogger.hh"

namespace swarms::log {

/// @brief - Convenience logging class allowing to configure the logger
/// independently from the core logger provided by the locator pattern.
class PrefixedLogger
{
  public:
  /// @brief - Create a wrapper around the logger available through the locator
  /// class where all logs will be prefixed by the module set as argument.
  /// @param module - the name of the module for this logger
  PrefixedLogger(const std::string &module);

  ~PrefixedLogger() = default;

  /// @brief - Return the current modules string for this logger.
  /// @return - a string representing the list of modules attached to the logger.
  auto getModule() const -> std::string;

  /// @brief - Replaces all previously set modules by the single value provided
  /// to this function.
  /// This function will ignore an attempt to set an empty string as a module.
  /// @param module - the new module to assign to the logs produced by the logger
  void setModule(const std::string &module);

  /// @brief - Add a new module to the list. It will be added at the end of the
  /// list of available modules and will thus show up at the end of the prefix
  /// attached to log messages.
  /// If the provided module string is empty, this function will be a no op.
  /// @param module - the new module to register.
  void addModule(const std::string &module);

  void verbose(const std::string_view message) const;
  void debug(const std::string_view message) const;
  void info(const std::string_view message) const;
  void notice(const std::string_view message) const;
  void warn(const std::string_view message, const std::optional<std::string> &cause = {}) const;
  void error(const std::string_view message, const std::optional<std::string> &cause = {}) const;

  private:
  /// @brief - Consolidated string containing the list of modules that will
  /// be attached to the logs produced by this logger. The string looks like
  /// the following:
  /// "[module 1] [module 2]"
  std::string m_modules{};
};

} // namespace swarms::log
