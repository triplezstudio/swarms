
#include <text_helper.hh>
#include <vulkan_renderer.hh>

namespace tz
{
/*
TextHelper::TextHelper(TextureAssetManager& textureAssetManager)
    : textureAssetManager(textureAssetManager), textRenderer(textRenderer)
{



}
*/
/*
void TextHelper::renderText(Transform transform, const std::string &text, int fontId)
{

  RenderHints textRenderHints;
  textRenderHints.materialType = MaterialType::Text;
  textRenderHints.vertexShaderType = VertexShaderType::Static;
  textRenderHints.texture = uiFontAtlasTextureIndex;
  PrimitiveRenderData prd;
  prd.transform = transform;;
  prd.geometryType     = PrimitiveGeometryType::Quad;
  prd.renderHints = textRenderHints;

  // This is a bigger challenge. Where to get this activeRenderCamera here?
  // Having this here suggests there is some global state which is tracking
  // the current activated camera.
  // Then here we would need access to this global state.
  // This could be a scene.
  // Or we pass the current camera into this function. More verbose but
  // avoding global state.
  // As this is a helper, maybe it is easier for the user to rely on the camera activation.
  prd.associatedCamera = activeRenderCamera;


  if (textVertexBuffers.find(text) == textVertexBuffers.end())
  {
    auto textGeometry = textRenderer->createGeometryForText(text, fontId == -1 ? uiFont : fontId);
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
*/
}
