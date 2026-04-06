
#include "RNG.hh"

namespace swarms::core {

RNG::RNG(int seed)
  : IRandomNumberGenerator()
  , m_device(seed)
{}

auto RNG::randomInt(const int min, const int max) -> int
{
  std::uniform_int_distribution<int> distribution(min, max);
  return distribution(m_device);
}

auto RNG::randomDouble(const double min, const double max) -> double
{
  std::uniform_real_distribution<double> distribution(min, max);
  return distribution(m_device);
}

auto RNG::randomAngle(const double min, const double max) -> double
{
  return randomDouble(min, max);
}

auto RNG::create() -> std::pair<RNG, int>
{
  // Create a random device. This will be used to provide the seed of the RNG.
  // See this article:
  // https://diego.assencio.com/?index=6890b8c50169ef45b74db135063c227c
  std::random_device device{};
  const auto seed = device();
  return std::make_pair(RNG(seed), seed);
}

} // namespace swarms::core
