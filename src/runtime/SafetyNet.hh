
#pragma once

#include "ILogger.hh"
#include <functional>
#include <string>

namespace swarms::runtime {

/// @brief - Executes the function provided as input and catches all exceptions
/// that might occur, logging an error when this happens. The log message includes
/// the function name provided as input.
/// @param func - the function to execute
/// @param functionName - the name of the function to execute: this provides a
/// simple way for the caller to identify which call failed by printing a known
/// handle in the log line.
/// @return - true if the function was executed successfully (i.e. did not throw
/// any exception) and false otherwise.
bool launchProtected(std::function<void(void)> func, const std::string &functionName);

} // namespace swarms::runtime
