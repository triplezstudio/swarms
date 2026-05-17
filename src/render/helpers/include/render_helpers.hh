//
// Created by mgrus on 14.05.2026.
//

#ifndef SWARMS_RENDER_HELPERS_HH
#define SWARMS_RENDER_HELPERS_HH
#include "scene.hh"
#include <string>
#include <vulkan_renderer.hh>

struct Font;

namespace tz::render::helpers
{

class TextRenderHelper
{

  public:
      TextRenderHelper(tz::render::vulkan::Renderer& renderer,
                       TextureManager& textureManager,
                       BindlessTextureRegistry& textureRegistry);
      Font loadFont(const std::string &fileName, int fontSize);
      void renderText(tz::scene::Transform transform, const std::string &text, int fontId);
      void endFrame();

  private:

      vulkan::Renderer &renderer;

      int uiFont = -1;
      uint32_t uiFontAtlasTextureIndex = 0;

      std::map<std::string, tz::text::TextGeometry> textGeometries;
      std::map<std::string, rv::Buffer*> textVertexBuffers;
      std::map<std::string, rv::Buffer*> textIndexBuffers;
      std::map<int, uint32_t> fontTextureMap;
      tz::text::TextRenderer* textRenderer = nullptr;
      std::vector<tz::scene::PrimitiveRenderData> framePrimitives;

};

}





#endif //SWARMS_RENDER_HELPERS_HH
