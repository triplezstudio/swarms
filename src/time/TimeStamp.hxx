
#pragma once

#include "TimeStamp.hh"
#include <ctime>
#include <iomanip>
#include <sstream>

namespace swarms::time {

inline auto now() noexcept -> TimeStamp
{
  return std::chrono::system_clock::now();
}

inline auto fromMilliseconds(const int ms) noexcept -> ClockDuration
{
  return Milliseconds(ms);
}

inline auto toMilliseconds(const ClockDuration &d) -> float
{
  const auto time = std::chrono::duration_cast<Milliseconds>(d);
  const auto ms   = time.count();
  return 1.0f * ms;
}

inline auto toSeconds(const ClockDuration &d) -> float
{
  const auto ms                       = toMilliseconds(d);
  constexpr auto MILLIS_IN_ONE_SECOND = 1000.0f;
  return 1.0f * ms / MILLIS_IN_ONE_SECOND;
}

inline auto timeToString(const TimeStamp &t) -> std::string
{
  // See here:
  // https://stackoverflow.com/questions/34857119/how-to-convert-stdchronotime-point-to-string/34858704
  // And here:
  // https://stackoverflow.com/questions/32873659/how-can-i-get-current-time-of-day-in-milliseconds-in-c/32874098#32874098
  const auto tt = std::chrono::system_clock::to_time_t(t);
  const auto tm = *std::gmtime(&tt); // GMT (UTC)

  const auto ms = std::chrono::duration_cast<Milliseconds>(t.time_since_epoch())
                  - std::chrono::duration_cast<std::chrono::seconds>(t.time_since_epoch());

  std::stringstream ss;
  ss << std::put_time(&tm, "UTC: %Y-%m-%d %H:%M:%S");
  ss << "." << ms.count() << "ms";

  return ss.str();
}

inline auto durationToMsString(const ClockDuration &d) -> std::string
{
  // https://stackoverflow.com/questions/22590821/convert-stdduration-to-human-readable-time
  const auto s  = std::chrono::duration_cast<Milliseconds>(d);
  const auto ms = s.count();
  return std::to_string(ms) + "ms";
}

inline auto durationToPrettyString(ClockDuration d, const bool includeFractionalSeconds)
  -> std::string
{
  // https://stackoverflow.com/questions/22590821/convert-stdduration-to-human-readable-time
  const auto h = std::chrono::duration_cast<std::chrono::hours>(d);
  d -= h;
  const auto m = std::chrono::duration_cast<std::chrono::minutes>(d);
  d -= m;
  const auto s = std::chrono::duration_cast<std::chrono::seconds>(d);
  d -= s;
  const auto ms = std::chrono::duration_cast<Milliseconds>(d);

  const auto hours   = h.count();
  const auto minutes = m.count();
  const auto seconds = s.count();
  const auto millis  = ms.count();

  std::stringstream ss;
  ss.fill('0');
  if (hours)
  {
    ss << hours << "h";
  }
  if (0 != hours || 0 != minutes)
  {
    if (hours)
    {
      ss << std::setw(2);
    }
    ss << minutes << "m";
  }
  if (0 != hours || 0 != minutes || 0 != seconds)
  {
    if (hours || minutes)
    {
      ss << std::setw(2);
    }
    ss << seconds;
    if (0 == millis || !includeFractionalSeconds)
    {
      ss << 's';
    }
  }

  if (0 != millis && includeFractionalSeconds)
  {
    if (ss.str().empty())
    {
      ss << "0";
    }
    ss << "." << std::setw(3) << millis << "ms";
  }

  if (0 == hours && 0 == minutes && 0 == seconds && (0 == millis || !includeFractionalSeconds))
  {
    return "0s";
  }

  return ss.str();
}

inline auto diffInMs(const TimeStamp &start, const TimeStamp &end) -> float
{
  const auto elapsed = end - start;
  const auto ms      = std::chrono::duration_cast<Milliseconds>(elapsed);
  return ms.count();
}

} // namespace swarms::time
