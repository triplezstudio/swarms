#include <texture_manager.hh>



tz::render::vulkan::Texture *tz::render::TextureManager::loadTexture(
  const std::string &filePath)
{
  // Possible early exit:
  // Do we know this image path already?
  // Then we can just give back the cached texture:
  if (textureCache.find(filePath) != textureCache.end())
  {
    return textureCache.at(filePath);
  }

  // Lets create a new texture!
  auto bd = tz::render::vulkan::loadBitmapDataFromPath(filePath);
  auto image = renderer.createImage(bd);
  auto texture = renderer.createTexture(image);
  textureCache.insert({filePath, texture});
  return texture;
}
