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

class TextRenderer
{
  public:
  TextRenderer(tz::render::vulkan::Renderer& renderer);
  auto createFont(const std::string& fontFile, uint16_t size) -> int;

  std::map<int, Font*> fonts;

  private:
      tz::render::vulkan::Renderer& renderer;
};


}

#endif //SWARMS_TEXT_RENDER_HH
