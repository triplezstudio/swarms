#ifndef SWARMS_TEXT_RENDER_HH
#define SWARMS_TEXT_RENDER_HH
#include <stb_truetype.h>
#include <string>
#include <texture_asset_manager.hh>
#include <vulkan_renderer.hh>

namespace tz::render
{

struct Font {
  uint32_t textureId;
  vulkan::Texture* atlas;
  float maxDescent = std::numeric_limits<float>::max();
  float lineHeight = std::numeric_limits<float>::min();
  float baseLine = 0.0f;
  std::vector<stbtt_bakedchar> bakedChars;

};

struct TextGeometry
{
  std::vector<Eigen::Vector3f> positions;
  std::vector<Eigen::Vector2f> texCoords;
  std::vector<uint32_t> indices;
};



/**
 * This class manages fonts and offers functions to recordAndSubmitFrameCommandBuffer
 * text to a vertex buffer.
 *
 */
class TZ_API TextRenderer
{
  public:
  explicit TextRenderer(vulkan::Renderer & renderer, TextureAssetManager& textureAssetManager);
  auto createFont(const std::string& fontFile, uint16_t size) -> Font*;
  TextGeometry createGeometryForText(const std::string& text, Font& font);


  private:
    TextureAssetManager textureAssetManager;
    vulkan::Renderer& renderer;
};


}

#endif //SWARMS_TEXT_RENDER_HH
