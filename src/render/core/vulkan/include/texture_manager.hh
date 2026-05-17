//
// Created by mgrus on 17.05.2026.
//

#ifndef SWARMS_TEXTURE_MANAGER_H
#define SWARMS_TEXTURE_MANAGER_H
#include <string>
#include <vulkan_renderer.hh>

namespace tz::render
{
class TextureManager
{
  public:
    TextureManager(vulkan::Renderer& renderer) : renderer(renderer) {}
    tz::render::vulkan::Texture* loadTexture(const std::string& filePath);

    // Bring in an externally loaded texture, so it can participate in the cache:
    void registerTexture(const std::string& filePath, vulkan::Texture* texture);

private:
    vulkan::Renderer& renderer;
    std::map<std::string, vulkan::Texture*> textureCache;

};

class BindlessTextureRegistry
{
  public:
      BindlessTextureRegistry(vulkan::Renderer& renderer, int set, int binding, int numberOfTextureSlots) : renderer(renderer), set(set), binding(binding) {
          auto textureDescBinding = renderer.createDescriptorBinding(binding,
                                                                     vulkan::DescriptorResourceType::Sampler,

                                                                      vulkan::ShaderType::Fragment,
                                                                     numberOfTextureSlots, nullptr, nullptr);

          dsLayout = renderer.createDescriptorSetLayout({textureDescBinding}, true);
          descriptorSet = renderer.createMultiframeDescriptorSet(dsLayout);

          };

          int registerTexture(vulkan::Texture* texture)
          {
            int idx = textureIndex++;
            textureMap.insert({textureIndex, texture});
            return idx;
          }

          vulkan::DescriptorSet* getDescriptorSet() {
            return descriptorSet;
          }

          vulkan::DescriptorSetLayout* getDescriptorSetLayout()
          {
            return dsLayout;
          }


  private:
      int set;
      int binding;
      vulkan::Renderer& renderer;
      vulkan::DescriptorSetLayout* dsLayout = nullptr;
      vulkan::DescriptorSet* descriptorSet = nullptr;
      int textureIndex = 0;
      std::map<int, vulkan::Texture*> textureMap;
};


}

#endif //SWARMS_TEXTURE_MANAGER_H
