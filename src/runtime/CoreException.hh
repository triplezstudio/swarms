
#pragma once

#include <optional>
#include <stdexcept>
#include <string>

namespace swarms::runtime {

class CoreException : public std::exception
{
  public:
  /// @brief - Creates a new exception with the specified message and an optional
  /// cause. Upon being created, this exception will log an error message including
  /// the cause and print the stack trace of where it was thrown.
  /// @param module - a string describing the source of the error, e.g. "network"
  /// @param message - the error message
  /// @param cause - an optional message indicating the cause of the exception
  CoreException(const std::string_view module,
                const std::string &message,
                const std::optional<std::string> &cause = {}) noexcept;

  /// @brief - Creates a new exception with the specified message and an exception
  /// which caused this one to be thrown.
  /// Upon being created, this exception will log an error message including the
  /// message of the exception which caused it and a stack trace.
  /// @param module - a string describing the source of the error, e.g. "network"
  /// @param message - the error message
  /// @param cause - an exception which caused this exception to be thrown
  CoreException(const std::string_view module,
                const std::string &message,
                const CoreException &cause) noexcept;

  ~CoreException() noexcept override = default;

  auto what() const throw() -> const char * override;

  private:
  std::string m_message{};
};

} // namespace swarms::runtime
