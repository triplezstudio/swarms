#include <render.hh>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


tz::BitmapData loadBitmapDataFromPath(const std::string & path)
{
  int width, height, channels;
  auto pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  tz::BitmapData bmdata;
  bmdata.pixels = pixels;
  bmdata.width = width;
  bmdata.height = height;
  return bmdata;
}