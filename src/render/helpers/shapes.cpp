
#include <array>
#include <render_helpers.hh>

constexpr static std::array<uint32_t, 6> quadIndices =
  {
    0, 1, 2,
    0, 2, 3
};

static std::array<uint32_t, 36> cubeIndices =
  {
    0,  1,  2,   2,  3,  0, // Front
    4,  5,  6,   6,  7,  4, // Back
    8,  9, 10,  10, 11,  8, // Left
    12, 13, 14,  14, 15, 12, // Right
    16, 17, 18,  18, 19, 16, // Top
    20, 21, 22,  22, 23, 20  // Bottom
};

static std::array<uint32_t, 36> cubeIndicesPosTex =
  {
    0,  1,  2,   2,  3,  0, // Front
    4,  5,  6,   6,  7,  4, // Back
    8,  9, 10,  10, 11,  8, // Left
    12, 13, 14,  14, 15, 12, // Right
    16, 17, 18,  18, 19, 16, // Top
    20, 21, 22,  22, 23, 20  // Bottom
};


std::array<uint32_t, 6> tz::getQuadIndices()
{
  return quadIndices;

}

std::array<uint32_t, 36> tz::getCubeIndices()
{
  return cubeIndices;
}

std::array<uint32_t, 36> tz::getCubeIndicesPosTex()
{
  return cubeIndicesPosTex;
}