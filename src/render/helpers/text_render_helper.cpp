#include <text_render.hh>
#include <functional>
#include <render.hh>
#include <render_helpers.hh>
#include <texture_manager.hh>
#include <scene.hh>
#include <vulkan_renderer.hh>
#include <window_system.hh>

namespace tz::render::helpers
{

TextRenderHelper::TextRenderHelper(vulkan::Renderer &renderer,
                                   TextureManager& textureManager,
                                   BindlessTextureRegistry& textureRegistry)
                : renderer(renderer)
{
  textRenderer = new tz::text::TextRenderer(renderer, textureManager, textureRegistry);
  uiFont = textRenderer->createFont("assets/consola.ttf", 12);

}
void render::helpers::TextRenderHelper::renderText(tz::scene::Transform transform, const std::string &text, int fontId)
{

  using namespace tz::scene;
  tz::scene::RenderHints textRenderHints;
  textRenderHints.materialType = MaterialType::Text;
  textRenderHints.vertexShaderType = VertexShaderType::Static;
  textRenderHints.texture = uiFontAtlasTextureIndex;
  tz::scene::PrimitiveRenderData prd;
  prd.transform = transform;;
  prd.geometryType     = PrimitiveGeometryType::Quad;
  prd.renderHints = textRenderHints;
  prd.associatedCamera = activeRenderCamera;


  if (textVertexBuffers.find(text) == textVertexBuffers.end())
  {
    auto textGeometry = textRenderer->getGeometryForText(text, fontId == -1 ? uiFont : fontId);
    textGeometries[text]= textGeometry;

    std::vector<rv::VertexPosTexCoords> vertices;
    for (int i = 0; i < textGeometry.positions.size();i++)
    {
      rv::VertexPosTexCoords vertex;
      vertex.pos = textGeometry.positions[i];
      vertex.texCoords = textGeometry.texCoords[i];
      vertices.push_back(vertex);
    }

    textVertexBuffers[text] = renderer.createBuffer(vertices.data(),
                                                     vertices.size() * sizeof (rv::VertexPosTexCoords),
                                                     rv::BufferUsage::Vertex);

    textIndexBuffers[text] = renderer.createBuffer(textGeometry.indices.data(),
                                                    textGeometry.indices.size() * sizeof(uint32_t),
                                                    rv::BufferUsage::Index);
  }

  prd.vertexBuffer = textVertexBuffers[text];
  prd.indexBuffer = textIndexBuffers[text];
  prd.indexCount = textGeometries[text].indices.size();
  prd.renderHints.texture = fontTextureMap[fontId == -1 ? uiFont : fontId];
  framePrimitives.push_back(prd);

}
Font render::helpers::TextRenderHelper::loadFont(const std::string &fileName, int size)
{
  auto fontId = textRenderer->createFont(fileName, size);
  fontTextureMap[fontId] = globalTextureIndex;
  renderer->updateTextureDescriptorSet(diffuseTextureDescriptorSet, 0, globalTextureIndex++, textRenderer->getAtlasTextureForFont(fontId));
  return {fontId};
}
void TextRenderHelper::endFrame()
{
  framePrimitives.clear();
}

}
