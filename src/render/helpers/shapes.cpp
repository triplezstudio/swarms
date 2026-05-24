
#include <array>
#include <render_helpers.hh>

constexpr static std::array<uint32_t, 6> quadIndices =
  {
    0, 1, 2,
    0, 2, 3
};

static std::array<uint32_t, 36> cubeIndices =
  {
    // Front face
    0, 1, 2,  2, 3, 0,
    // Right face
    1, 5, 6,  6, 2, 1,
    // Back face
    5, 4, 7,  7, 6, 5,
    // Left face
    4, 0, 3,  3, 7, 4,
    // Bottom face
    4, 5, 1,  1, 0, 4,
    // Top face
    3, 2, 6,  6, 7, 3
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