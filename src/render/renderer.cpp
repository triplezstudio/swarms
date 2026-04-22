#include <render.hh>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

tz::BitmapData tz::loadBitmapDataFromPath(const std::string &path)
{
  stbi_set_flip_vertically_on_load(true);
  int width, height, channels;
  auto pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  tz::BitmapData bmdata;
  bmdata.pixels = pixels;
  bmdata.width = width;
  bmdata.height = height;
  return bmdata;
}