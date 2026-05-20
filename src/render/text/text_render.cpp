
#include <iostream>
#include <text_render.hh>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

auto tz::render::TextRenderer::createFont(const std::string& fontFile, uint16_t fontSize) -> Font*
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

    vulkan::BitmapData atlasBitmapData = {
      .pixels = new uint8_t[512 * 512],
      .width = 512,
      .height = 512,
      .channels = 1

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

    font->textureId = textureAssetManager.loadTexture(fontFile);
    return font;

}
 tz::render::TextRenderer::TextRenderer(vulkan::Renderer & renderer, TextureAssetManager &textureAssetManager)
    :textureAssetManager(textureAssetManager), renderer(renderer)
{

}

tz::render::TextGeometry tz::render::TextRenderer::createGeometryForText(const std::string &text,
  tz::render::Font & font)
{
  std::vector<Eigen::Vector3f> positions;
  std::vector<Eigen::Vector2f> texCoords;
  std::vector<uint32_t> indices;

  float penX = 0, penY = 0;
  float minX =  std::numeric_limits<float>::max();
  float maxX = -std::numeric_limits<float>::max();
  float minY =  std::numeric_limits<float>::max();
  float maxY = -std::numeric_limits<float>::max();
  float baseline = font.baseLine;
  int charCounter = 0;
  for (auto c : text) {
    stbtt_aligned_quad q;
    stbtt_GetBakedQuad(font.bakedChars.data(), 512, 512, c - 32, &penX, &penY, &q, 0);

    float pixel_aligned_x0 = std::floor(q.x0 + 0.0f);
    float pixel_aligned_y0 = std::floor(q.y0 + 0.0f);
    float pixel_aligned_x1 = std::floor(q.x1 + 0.0f);
    float pixel_aligned_y1 = std::floor(q.y1 + 0.0f);

    q.x0 = pixel_aligned_x0;
    q.y0 = pixel_aligned_y0;
    q.x1 = pixel_aligned_x1;
    q.y1 = pixel_aligned_y1;


    // Non vulkan y-flip fix:
    {
      positions.push_back(Eigen::Vector3f(q.x0, -q.y0, 0));
      positions.push_back(Eigen::Vector3f(q.x1, -q.y0, 0));
      positions.push_back(Eigen::Vector3f(q.x1, -q.y1, 0));
      positions.push_back(Eigen::Vector3f(q.x0, -q.y1, 0));

    }


    texCoords.push_back({q.s0, q.t0});
    texCoords.push_back({q.s1, q.t0});
    texCoords.push_back({q.s1, q.t1});
    texCoords.push_back({q.s0, q.t1});

    // Flip vertical uv coordinates
    //texCoords.push_back({q.s0, q.t1});
    //texCoords.push_back({q.s1, q.t1});
    //texCoords.push_back({q.s1, q.t0});
    //texCoords.push_back({q.s0, q.t0});



    int offset = charCounter * 4;
    indices.push_back(2 + offset);indices.push_back(1 + offset);indices.push_back(0 + offset);
    indices.push_back(2 + offset);indices.push_back(0 + offset);indices.push_back(3 + offset);

    // Flipped
    //indices.push_back(0 + offset);indices.push_back(1 + offset);indices.push_back(2 + offset);
    //indices.push_back(3 + offset);indices.push_back(0 + offset);indices.push_back(2 + offset);
    charCounter++;

    // Track min/max for bounding box
    minX = std::min(minX, q.x0);
    maxX = std::max(maxX, q.x1);

    if (c == 32) continue; // ignore space for Y, as this is always zero and messes things up.
    minY = std::min(minY, q.y0); // lowest part (descenders)
    minY = std::min(minY, q.y1);

    maxY = std::max(maxY, q.y0); // highest part (ascenders)
    maxY = std::max(maxY, q.y1);
  }

  return tz::render::TextGeometry {positions, texCoords, indices};
}


