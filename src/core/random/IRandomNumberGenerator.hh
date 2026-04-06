
#pragma once

namespace swarms::core {

class IRandomNumberGenerator
{
  public:
  IRandomNumberGenerator()          = default;
  virtual ~IRandomNumberGenerator() = default;

  /// @brief - Generates a random integer in the interval [min; max].
  /// In case `min` and `max` do not define a valid interval calling
  /// this function is undefined behavior.
  /// @param min - the minimum value which can be generated (inclusive)
  /// @param max - the maximum value which can be generated (inclusive)
  /// @return - a random integer in the interval
  virtual auto randomInt(const int min, const int max) -> int = 0;

  /// @brief - Generates a random double value in the interval [min; max].
  /// In case `min` and `max` do not define a valid interval calling
  /// this function is undefined behavior.
  /// @param min - the minimum value which can be generated (inclusive)
  /// @param max - the maximum value which can be generated (inclusive)
  /// @return - a random double in the interval
  virtual auto randomDouble(const double min, const double max) -> double = 0;

  /// @brief - Generates a random double value in the interval [0; 2pi].
  /// It is recommended for this function to use random double with an
  /// interval which corresponds to the desired bounds for the angle.
  /// @return - a random angle
  virtual auto randomAngle(const double min = 0.0, const double max = 6.283185307) -> double = 0;
};

} // namespace swarms::core
