
#pragma once

#include <chrono>
#include <string>

namespace swarms::time {

/// @brief - Convenience define to refer to a time point. Allows to keep track
/// of absolute timestamps to trigger some behaviors.
using TimeStamp = std::chrono::system_clock::time_point;

/// @brief - Conveniently represent a duration. It can be added or subtracted to
/// a timestamp in an easy way.
using ClockDuration = std::chrono::system_clock::duration;

/// @brief - Convenience define to represent a single millisecond.
using Milliseconds = std::chrono::milliseconds;

/// @brief - Used to retrieve the timestamp as of now.
/// @return - a timepoint at the moment of the call to this function.
auto now() noexcept -> TimeStamp;

/// @brief - Used to convert the input integer as a time duration expressed in
/// milliseconds. Note that this will loose precision for cases where the duration
/// does not represent a round number of milliseconds.
/// @param ms - the number of milliseconds to convert.
/// @return - a duration representing the input number of milliseconds.
auto fromMilliseconds(const int ms) noexcept -> ClockDuration;

/// @brief - Used to convert the input duration to the corresponding floating
/// point number of milliseconds.
/// @param d - the duration to convert.
/// @return - the number of milliseconds in the input duration.
auto toMilliseconds(const ClockDuration &d) -> float;

/// @brief - Used to convert the input duration to the corresponding floating
/// point number of seconds.
/// @param d - the duration to convert.
/// @return - the number of seconds in the input duration.
auto toSeconds(const ClockDuration &d) -> float;

/// @brief - Converts a timestamp to a human readable string.
/// @param t - the time to convert.
/// @return - a string representing this time.
auto timeToString(const TimeStamp &t) -> std::string;

/// @brief - Converts the duration to a human readable string expressed
/// in milliseconds.
/// @param d - the duration to convert to string. Be aware that it should
/// not be 'too far' from one millisecond in order to get a display that
/// is not a huge batch of numbers.
/// @return - the corresponding string.
auto durationToMsString(const ClockDuration &d) -> std::string;

/// @brief - Convert the input duration to a human readable string similar to
/// Xh Ym Zs.
/// @param d - the duration to convert.
/// @param includeFractionalSeconds - whether or not milliseconds should be
/// included in the output string.
/// @return - a string representing this duration.
auto durationToPrettyString(ClockDuration d, const bool includeFractionalSeconds = false)
  -> std::string;

/// @brief - Return the difference in milliseconds between the two input
/// timestamps using a float value.
/// @param start - the start of the time interval.
/// @param end - the end of the time interval.
/// @return - a float value for the interval in milliseconds.
auto diffInMs(const TimeStamp &start, const TimeStamp &end) -> float;

} // namespace swarms::time

#include "TimeStamp.hxx"
