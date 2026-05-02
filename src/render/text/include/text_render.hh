#ifndef SWARMS_TEXT_RENDER_HH
#define SWARMS_TEXT_RENDER_HH
#include <string>
#include <cinttypes>
#include <vulkan_renderer.hh>
#include <stb_truetype.h>
#include <map>

namespace tz::text
{

struct Font {
  tz::render::vulkan::Texture* atlas;
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
 * This class manages fonts and offers functions to render
 * text to a vertex buffer.
 *
 */
class TZ_API TextRenderer
{
  public:
  explicit TextRenderer(tz::render::vulkan::Renderer& renderer);
  auto createFont(const std::string& fontFile, uint16_t size) -> int;
  TextGeometry getGeometryForText(const std::string& text, uint32_t fontId);

  render::vulkan::Texture *getAtlasTextureForFont(int fontId);

  private:
      tz::render::vulkan::Renderer& renderer;
      std::map<int, Font*> fonts;
};


}

#endif //SWARMS_TEXT_RENDER_HH
