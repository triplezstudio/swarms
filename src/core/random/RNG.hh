
#pragma once

#include "IRandomNumberGenerator.hh"
#include <random>

namespace swarms::core {

class RNG : public IRandomNumberGenerator
{
  public:
  /// @brief - Creates a new random number generator with the provided seed.
  /// This generator uses the mersenne twister engine as a source of entropy.
  /// @param seed - the value to use to see the source of entropy
  RNG(int seed = 0);

  ~RNG() override = default;

  auto randomInt(const int min, const int max) -> int override;
  auto randomDouble(const double min, const double max) -> double override;
  auto randomAngle(const double min = 0.0, const double max = 6.283185307) -> double override;

  /// @brief - Creates a random number generator initialized with a seed
  /// itself picked at random. This allows to abstract the generation of
  /// a seed.
  /// @return - the random number generator along with the seed
  static auto create() -> std::pair<RNG, int>;

  private:
  /// @brief - The source of entropy used to generate random numbers.
  std::mt19937 m_device;
};

} // namespace swarms::core
