
#include <iostream>
#include <text_render.hh>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

auto tz::text::TextRenderer::createFont(const std::string& fontFile, uint16_t fontSize) -> int
{


    // Read font file
    FILE *fp = fopen(fontFile.c_str(), "rb");
    if (!fp) {
      auto msg = "Failed to open font file: " + fontFile;
      std::cerr << msg << std::endl;
      throw std::runtime_error(msg);
    }
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    unsigned char *ttf_buffer = new unsigned char[size];
    fread(ttf_buffer, 1, size, fp);
    fclose(fp);


    tz::render::vulkan::BitmapData  atlasBitmapData = {
      .pixels = new uint8_t[512 * 512],
      .width = 512,
      .height = 512,

    };

    // Retrieve font measurements
    stbtt_fontinfo info;
    stbtt_InitFont(&info, ttf_buffer, 0);

    // Create the actual font.
    // First define its measurements and dimensions:
    auto font = new Font;
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
    float scale = stbtt_ScaleForPixelHeight(&info, fontSize);
    auto scaled_ascent    = ascent  * scale;  // typically a positive number
    auto scaled_descent   = descent * scale;  // typically negative
    auto scaled_line_gap  = lineGap * scale;
    font->baseLine = scaled_ascent;
    font->lineHeight = (scaled_ascent - scaled_descent) + scaled_line_gap;
    font->bakedChars.resize(96);

    // Create the glyph atlas texture.
    // Every rasterized glyph is packed into a single texture.
    int result = stbtt_BakeFontBitmap(ttf_buffer, 0, fontSize,
                                      atlasBitmapData.pixels,
                                      atlasBitmapData.width,
                                      atlasBitmapData.height,
                                      32, 96,
                                      font->bakedChars.data());

    if (result <= 0) {
      auto msg ="Failed to bake font bitmap!";
      std::cerr << msg << std::endl;
      delete[] ttf_buffer;
      throw std::runtime_error(msg);
    }

    auto atlasImage = renderer.createImage(atlasBitmapData);
    font->atlas = renderer.createTexture(atlasImage);

    static uint16_t fontId = 0;
    fonts[fontId++] = font;
    return fontId;

}
tz::text::TextRenderer::TextRenderer(tz::render::vulkan::Renderer &renderer) :renderer(renderer)
{

}
