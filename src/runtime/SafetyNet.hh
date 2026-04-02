
#pragma once

#include "ILogger.hh"
#include "PrefixedLogger.hh"
#include <functional>
#include <string>

namespace swarms::runtime {

/// @brief - Executes the function provided as input and catches all exceptions
/// that might occur, logging an error when this happens. The log message includes
/// the function name provided as input.
/// @param func - the function to execute
/// @param module - a string describing the module of the caller, e.g. "network"
/// @param functionName - the name of the function to execute: this provides a
/// simple way for the caller to identify which call failed by printing a known
/// handle in the log line.
/// @return - true if the function was executed successfully (i.e. did not throw
/// any exception) and false otherwise.
bool launchProtected(std::function<void(void)> func,
                     const std::string_view module,
                     const std::string &functionName);

/// @brief - Executes the function provided as input and catches all exceptions
/// that might occur, logging an error when this happens. The logger provided in
/// input is used for logging. This allows to keep consistent logging when a
/// specific logger is available in the context calling this function. Typically
/// this allows `CoreObject`s to enforce consistent logging through by passing
/// their internal logger as an argument.
/// @param func - the function to execute
/// @param functionName - a string describing the module of the caller
/// @param logger - the logger to use to print error messages
/// @return - true if the function was executed without exceptions, false otherwise.
bool launchProtected(std::function<void(void)> func,
                     const std::string &functionName,
                     const log::PrefixedLogger &logger);

} // namespace swarms::runtime
