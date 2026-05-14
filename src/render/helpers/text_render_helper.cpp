#include "app.hh"
#include "vulkan_renderer.hh"
#include "window_system.hh"
#include <functional>

namespace tz
{
void App::renderText(Transform transform, const std::string &text, int fontId)
{

  RenderHints textRenderHints;
  textRenderHints.materialType = MaterialType::Text;
  textRenderHints.vertexShaderType = VertexShaderType::Static;
  textRenderHints.texture = uiFontAtlasTextureIndex;
  PrimitiveRenderData prd;
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

    textVertexBuffers[text] = renderer->createBuffer(vertices.data(),
                                                     vertices.size() * sizeof (rv::VertexPosTexCoords),
                                                     rv::BufferUsage::Vertex);

    textIndexBuffers[text] = renderer->createBuffer(textGeometry.indices.data(),
                                                    textGeometry.indices.size() * sizeof(uint32_t),
                                                    rv::BufferUsage::Index);
  }



  prd.vertexBuffer = textVertexBuffers[text];
  prd.indexBuffer = textIndexBuffers[text];
  prd.indexCount = textGeometries[text].indices.size();
  prd.renderHints.texture = fontTextureMap[fontId == -1 ? uiFont : fontId];
  framePrimitives.push_back(prd);

}
int App::createFont(const std::string &fileName, int size)
{
  auto fontId = textRenderer->createFont(fileName, size);
  fontTextureMap[fontId] = globalTextureIndex;
  renderer->updateTextureDescriptorSet(diffuseTextureDescriptorSet, 0, globalTextureIndex++, textRenderer->getAtlasTextureForFont(fontId));
  return fontId;
}
void App::setInputListenerFunc(InputListener inputListener)
{
  inputListeners.push_back(inputListener);
}
}
