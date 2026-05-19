//
// Created by Martin Gruscher on 19.05.26.
//

#ifndef SWARMS_BINDLESS_TEXTURE_REGISTRY_HH
#define SWARMS_BINDLESS_TEXTURE_REGISTRY_HH
#include "vulkan_renderer.hh"
#include <map>
#include <string>
#include <array>

namespace tz {


/**
 * This class has 2 main purposes:
 * 1. Convenient for the user to store and load textures.
 * 2. Maintains a global texture index which we need inside our pipeline layout.
 * Our textures are stored in a bindless DescriptorSet, which has place for 1000 textures.
 * To access the correct textures at runtime, inside our shaders, we must
 * keep track of which texture has which id.
 *
 */
class TextureAssetManager
{
public:
  explicit TextureAssetManager(render::vulkan::Renderer& renderer): renderer(renderer)
  {

  }

  void loadTexture(const std::string& imagePath)
  {

    auto existingIndex = getIndexForTexture(imagePath);
    if (existingIndex > -1)
    {
      return;
    }

    auto bmData = render::vulkan::loadBitmapDataFromPath(imagePath);
    auto image = renderer.createImage(bmData);
    auto texture = renderer.createTexture(image);
    auto idx = registerTexture(imagePath);
    textureArray[idx] = texture;
  }

private:
  int registerTexture(const std::string& name)
  {
    if (textureIndexMap.contains(name))
    {
      const auto id = textureIndex++;
      textureIndexMap[name] = id;
      return id;
    }

    return textureIndexMap[name];
  }

  render::vulkan::Texture * getTextureByIndex(int idx)
  {
    if (idx < 0 || idx >= textureArray.size()) return nullptr;
    if (idx >= textureIndex) return nullptr;

    return textureArray[idx];
  }

  render::vulkan::Texture * getTextureByName(const std::string& name)
  {
    const auto idx = getIndexForTexture(name);
    if (idx == -1) return nullptr;
    return getTextureByIndex(idx);
  }

  int getIndexForTexture(const std::string& name)
  {
    if (textureIndexMap.contains(name))
    {
      return textureIndexMap[name];
    }

    return -1;

  }

  int textureIndex = 0;
  std::map<std::string, int> textureIndexMap;
  std::array<render::vulkan::Texture*, 1000> textureArray {};
  render::vulkan::Renderer& renderer;

};


}

#endif //SWARMS_BINDLESS_TEXTURE_REGISTRY_HH
