#include <app.hh>
#include <iostream>
#include <sdl2.hh>
#include <text_render.hh>
#include <vulkan_renderer.hh>
#include <window_system.hh>
#include <sdl2.hh>

namespace tz
{
namespace rv = render::vulkan;

App::App() :inputSystem(tz::input::SDL2InputSystem::getInstance())
{
  renderer = new rv::Renderer();
  windowSystem = new tz::SDL2WindowSystem();

  auto winDesc = tz::WindowDesc();
  winDesc.width = 800;
  winDesc.height = 600;
  auto window = windowSystem->createWindow(winDesc);
  renderer->init(window);

  prepareRenderPrimitives();
  buildPSOCache();
  commandBuffer = renderer->createCommandBuffer();

  default3DCamera = new Camera({0, 2, 3}, {0, 0, 0 }, CameraType::Perspective);
  defaultUICamera = new Camera({0, 0, 1}, {0, 0, 0}, CameraType::Ortho);

}

void App::prepareRenderPrimitives()
{

  std::vector<rv::VertexPos> verticesPos =
    {
      {{-0.5, 0.5, 0.5}},
      {{-0.5, -0.5, 0.5}},
      {{0.5, -0.5, 0.5}},
      {{0.5, 0.5, 0.5}}
    };

  std::vector<rv::VertexPosTexCoords> verticesPosTexCoord =
    {
      {{-0.5, 0.5, 0.5}, {0, 1}},
      {{-0.5, -0.5, 0.5}, {0, 0}},
      {{0.5, -0.5, 0.5},  {1, 0}},
      {{0.5, 0.5, 0.5}, {1, 1}}
    };

  std::vector<rv::VertexPos> cubeVerticesPos = {
    {{-.5f, -.5f,  .50f}}, // 0: Front-Bottom-Left
    {{ .5f, -.5f,  .5f}}, // 1: Front-Bottom-Right
    {{ .5f,  .5f,  .5f}}, // 2: Front-Top-Right
    {{-.5f,  .5f,  .5f}}, // 3: Front-Top-Left
    {{-.5f, -.5f, -.5f}}, // 4: Back-Bottom-Left
    {{ .5f, -.5f, -.5f}}, // 5: Back-Bottom-Right
    {{ .5f,  .5f, -.5f}}, // 6: Back-Top-Right
    {{-.5f,  .5f, -.5f}}  // 7: Back-Top-Left
  };

  std::vector<rv::VertexPosTexCoords> cubeVerticesPosTex = {
    // Front face (Z = 1.0f)
    {{-.5f, -.5f,  .5f}, {0.0f, 0.0f}}, {{ .5f, -.5f,  .5f}, {1.0f, 0.0f}},
    {{ .5f,  .5f,  .5f}, {1.0f, 1.0f}}, {{-.5f,  .5f,  .5f}, {0.0f, 1.0f}},

    // Back face (Z = -1.0f)
    {{ .5f, -.5f, -.5f}, {0.0f, 0.0f}}, {{-.5f, -.5f, -.5f}, {1.0f, 0.0f}},
    {{-.5f,  .5f, -.5f}, {1.0f, 1.0f}}, {{ .5f,  .5f, -.5f}, {0.0f, 1.0f}},

    // Left face (X = -1.0f)
    {{-.5f, -.5f, -.5f}, {0.0f, 0.0f}}, {{-.5f, -.5f,  .5f}, {1.0f, 0.0f}},
    {{-.5f,  .5f,  .5f}, {1.0f, 1.0f}}, {{-.5f,  .5f, -.5f}, {0.0f, 1.0f}},

    // Right face (X = 1.0f)
    {{ .5f, -.5f,  .5f}, {0.0f, 0.0f}}, {{ .5f, -.5f, -.5f}, {1.0f, 0.0f}},
    {{ .5f,  .5f, -.5f}, {1.0f, 1.0f}}, {{ .5f,  .5f,  .5f}, {0.0f, 1.0f}},

    // Top face (Y = 1.0f)
    {{-.5f,  .5f,  .5f}, {0.0f, 0.0f}}, {{ .5f,  .5f,  .5f}, {1.0f, 0.0f}},
    {{ .5f,  .5f, -.5f}, {1.0f, 1.0f}}, {{-.5f,  .5f, -.5f}, {0.0f, 1.0f}},

    // Bottom face (Y = -1.0f)
    {{-.5f, -.5f, -.5f}, {0.0f, 0.0f}}, {{ .5f, -.5f, -.5f}, {1.0f, 0.0f}},
    {{ .5f, -.5f,  .5f}, {1.0f, 1.0f}}, {{-.5f, -.5f,  .5f}, {0.0f, 1.0f}}
  };


  quadIndices =
    {
      0, 1, 2,
      0, 2, 3
    };

  cubeIndices = {
    // Front face
    0, 1, 2,  2, 3, 0,
    // Right face
    1, 5, 6,  6, 2, 1,
    // Back face
    5, 4, 7,  7, 6, 5,
    // Left face
    4, 0, 3,  3, 7, 4,
    // Bottom face
    4, 5, 1,  1, 0, 4,
    // Top face
    3, 2, 6,  6, 7, 3
  };

  cubeIndicesPosTex = {
    0,  1,  2,   2,  3,  0, // Front
    4,  5,  6,   6,  7,  4, // Back
    8,  9, 10,  10, 11,  8, // Left
    12, 13, 14,  14, 15, 12, // Right
    16, 17, 18,  18, 19, 16, // Top
    20, 21, 22,  22, 23, 20  // Bottom
  };

  quadPosVertexBuffer = renderer->createBuffer(verticesPos.data(),
                                                 verticesPos.size() * sizeof (rv::VertexPos),
                                                 rv::BufferUsage::Vertex);

  cubePosVertexBuffer = renderer->createBuffer(cubeVerticesPos.data(),
                                               cubeVerticesPos.size() * sizeof (rv::VertexPos),
                                               rv::BufferUsage::Vertex);

  cubePosTexCoordVertexBuffer = renderer->createBuffer(cubeVerticesPosTex.data(),
                                                       cubeVerticesPosTex.size() * sizeof (rv::VertexPosTexCoords),
                                                       rv::BufferUsage::Vertex);


  quadPosTexCoordVertexBuffer = renderer->createBuffer(verticesPosTexCoord.data(),
                                            verticesPosTexCoord.size() * sizeof (rv::VertexPosTexCoords),
                                            rv::BufferUsage::Vertex);

  quadIndexBuffer = renderer->createBuffer(quadIndices.data(),
                                                quadIndices.size() * sizeof(uint32_t),
                                                rv::BufferUsage::Index);

  cubeIndexBuffer = renderer->createBuffer(cubeIndices.data(),
                                           cubeIndices.size() * sizeof(uint32_t),
                                           rv::BufferUsage::Index);

  cubeTexIndexBuffer = renderer->createBuffer(cubeIndicesPosTex.data(),
                                           cubeIndicesPosTex.size() * sizeof(uint32_t),
                                           rv::BufferUsage::Index);



    textRenderer = new tz::text::TextRenderer(*renderer);
    uiFont = textRenderer->createFont("assets/consola.ttf", 12);
    uiFontAtlasTextureIndex = globalTextureIndex;
    renderer->updateTextureDescriptorSet(diffuseTextureDescriptorSet, 0, globalTextureIndex++, textRenderer->getAtlasTextureForFont(uiFont));
}

tz::render::vulkan::Renderer * App::vulkanRenderer()
{
  return dynamic_cast<tz::render::vulkan::Renderer *>(renderer);
}

/**
 * Here we create all needed PSO variants.
 * We store them in a map which understands
 * RenderingHints hashed keys.
 */
void App::buildPSOCache()
{
  colorOnlyPSO = createColorOnlyPSO();
  RenderHints colorOnlyHints;
  colorOnlyHints.materialType = MaterialType::SingleColor;
  colorOnlyHints.vertexShaderType = VertexShaderType::Static;
  psoCache[colorOnlyHints.getHash()] = colorOnlyPSO;

  auto texturedPSO = createTexturedPSO();
  RenderHints texturedHints;
  texturedHints.materialType = MaterialType::DiffuseNormal;
  texturedHints.vertexShaderType = VertexShaderType::Static;
  psoCache[texturedHints.getHash()] = texturedPSO;

  auto textPSO = createTextPSO();
  RenderHints textHints;
  textHints.materialType = MaterialType::Text;
  textHints.vertexShaderType = VertexShaderType::Static;
  psoCache[textHints.getHash()] = textPSO;

}

rv::PipelineStateObject* App::createColorOnlyPSO()
{

  auto vsPath = "shader_binaries/default_colored_transform_vs.slang.spv";
  auto fsPath = "shader_binaries/default_colored_transform_fs.slang.spv";

  auto vs = renderer->createShaderModule(rv::ShaderType::Vertex, vsPath);
  auto fs = renderer->createShaderModule(rv::ShaderType::Fragment, fsPath);
  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  auto renderState = rv::RenderState {};
  renderState.primitiveType = rv::PrimitiveType::Triangles;
  renderState.blending = false;
  renderState.depthTesting = true;
  renderState.cullMode = vk::CullModeFlagBits::eBack;
  renderState.fillMode = vk::PolygonMode::eFill;
  renderState.frontFace = vk::FrontFace::eCounterClockwise;
  renderState.stencilTesting = false;
  shaderPipeline = shaderPipeline;

  auto vertexLayout = rv::VertexLayout {};
  vertexLayout.bindings =  {rv::VertexBinding {
      .bufferSlot = 0,
      .stride = sizeof(rv::VertexPos),
      .vertexInputRate=rv::VertexInputRate::PerVertex,
  }};
  vertexLayout.attributes = {
    rv::VertexAttribute {
      .shaderLocation = 0,
      .bufferSlot = 0,
      .dataType = rv::DataType::Float,
      .componentCount = 3,
      .offset = 0
    }
  };

  // Descriptor layout and binding for the transformation matrix



  auto pso = renderer->createPipelineStateObject(renderState,
                                                 shaderPipeline,
                                              vertexLayout,
                                                 masterPipelineLayout);

  return pso;
}

rv::PipelineStateObject* App::createTextPSO()
{
  auto spvVertexShaderPath = "shader_binaries/default_textured_vs.slang.spv";
  auto spvFragmentShaderPath = "shader_binaries/text_fs.slang.spv";

  auto vs = renderer->createShaderModule(rv::ShaderType::Vertex, spvVertexShaderPath);
  auto fs = renderer->createShaderModule(rv::ShaderType::Fragment, spvFragmentShaderPath);
  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  auto renderState = rv::RenderState {};
  renderState.primitiveType = rv::PrimitiveType::Triangles;
  renderState.blending = true;
  renderState.depthTesting = true;
  renderState.cullMode = vk::CullModeFlagBits::eBack;
  renderState.fillMode = vk::PolygonMode::eFill;
  renderState.frontFace = vk::FrontFace::eCounterClockwise;
  renderState.stencilTesting = false;
  shaderPipeline = shaderPipeline;

  auto vertexLayout = rv::VertexLayout {};
  vertexLayout.bindings =  {rv::VertexBinding {
      .bufferSlot = 0,
      .stride = sizeof(rv::VertexPosTexCoords),
      .vertexInputRate=rv::VertexInputRate::PerVertex,
  }};
  vertexLayout.attributes = {
    rv::VertexAttribute {
      .shaderLocation = 0,
      .bufferSlot = 0,
      .dataType = rv::DataType::Float,
      .componentCount = 3,
      .offset = 0
    },
    {
      rv::VertexAttribute
      {
        .shaderLocation = 1,
        .bufferSlot = 0,
        .dataType = rv::DataType::Float,
        .componentCount = 2,
        .offset = sizeof(float) * 3
      }
    }
  };


  auto pso = renderer->createPipelineStateObject(renderState,
                                                 shaderPipeline,
                                                 vertexLayout,
                                                 masterPipelineLayout);
  return pso;
}


rv::PipelineStateObject* App::createTexturedPSO()
{

  auto vsPath = "shader_binaries/default_textured_vs.slang.spv";
  auto fsPath = "shader_binaries/default_textured_fs.slang.spv";

  auto vs = renderer->createShaderModule(rv::ShaderType::Vertex, vsPath);
  auto fs = renderer->createShaderModule(rv::ShaderType::Fragment, fsPath);
  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  auto renderState = rv::RenderState {};
  renderState.primitiveType = rv::PrimitiveType::Triangles;
  renderState.blending = true;
  renderState.depthTesting = true;
  renderState.cullMode = vk::CullModeFlagBits::eBack;
  renderState.fillMode = vk::PolygonMode::eFill;
  renderState.frontFace = vk::FrontFace::eCounterClockwise;
  renderState.stencilTesting = false;
  shaderPipeline = shaderPipeline;

  auto vertexLayout = rv::VertexLayout {};
  vertexLayout.bindings =  {rv::VertexBinding {
      .bufferSlot = 0,
      .stride = sizeof(rv::VertexPosTexCoords),
      .vertexInputRate=rv::VertexInputRate::PerVertex,
  }};
  vertexLayout.attributes = {
    rv::VertexAttribute {
      .shaderLocation = 0,
      .bufferSlot = 0,
      .dataType = rv::DataType::Float,
      .componentCount = 3,
      .offset = 0
    },
    {
      rv::VertexAttribute
      {
        .shaderLocation = 1,
        .bufferSlot = 0,
        .dataType = rv::DataType::Float,
        .componentCount = 2,
        .offset = sizeof(float) * 3
      }
    }
  };


  auto pso = renderer->createPipelineStateObject(renderState,
                                                 shaderPipeline,
                                                 vertexLayout,
                                                 masterPipelineLayout);
  return pso;
}


void tz::App::run()
{
  while (true)
  {
    windowSystem->pollEvents();
    inputSystem.update(reinterpret_cast<SDL2WindowSystem*>(windowSystem)->getFrameEvents());
    updateFrameListeners(16.66f);
    updateInputListeners();
    renderFrame();
  }

}

void App::updateInputListeners()
{
  for (auto& inputListener : inputListeners)
  {
    inputListener(inputSystem);
  }
}

std::vector<tz::PrimitiveRenderData> App::getRenderPrimitivesByCamera(Camera* camera)
{
  std::vector<PrimitiveRenderData> filteredPrimitiveData;
  for (auto& rp : framePrimitives)
  {
    if (rp.associatedCamera == camera)
    {
      filteredPrimitiveData.push_back(rp);
    }
  }

  return filteredPrimitiveData;

}

void App::renderPrimitives(const std::vector<PrimitiveRenderData>& primitives, uint32_t& primitiveCounter)
{


}

void App::renderFrame()
{

}

void App::setUpdateFunction(tz::FrameListener frameListener)
{
  frameListeners.push_back(frameListener);
}

void App::updateFrameListeners(float frameTime)
{

  for (auto& frameListenerFunc : frameListeners) {
    frameListenerFunc(this);
  }



}
float App::getLastFrameTime()
{
  return 16.667f;
}
void App::renderCube(Transform transform, RenderHints renderHints)
{
  PrimitiveRenderData prd;
  prd.transform = transform;
  prd.geometryType = PrimitiveGeometryType::Cube;
  prd.renderHints = renderHints;
  prd.associatedCamera = activeRenderCamera;
  prd.vertexBuffer = renderHints.materialType == MaterialType::SingleColor ? cubePosVertexBuffer : cubePosTexCoordVertexBuffer;
  prd.indexBuffer = renderHints.materialType == MaterialType::SingleColor ? cubeIndexBuffer : cubeTexIndexBuffer;
  prd.indexCount = renderHints.materialType == MaterialType::SingleColor ? cubeIndices.size() : cubeIndicesPosTex.size();
  framePrimitives.push_back(prd);

}
void App::renderQuad(Transform transform, RenderHints renderHints)
{
  PrimitiveRenderData prd;
  prd.transform = transform;;
  prd.geometryType     = PrimitiveGeometryType::Quad;
  prd.renderHints = renderHints;
  prd.associatedCamera = activeRenderCamera;
  prd.vertexBuffer = renderHints.materialType == MaterialType::SingleColor ? quadPosVertexBuffer : quadPosTexCoordVertexBuffer;
  prd.indexBuffer = quadIndexBuffer;
  prd.indexCount = quadIndices.size();
  framePrimitives.push_back(prd);
}
Eigen::Matrix4f App::createLookAtMatrix(const Eigen::Vector3f& eye,
                                        const Eigen::Vector3f& center,
                                        const Eigen::Vector3f& up)
{
  Eigen::Vector3f f = (center - eye).normalized();
  Eigen::Vector3f s = f.cross(up).normalized();
  Eigen::Vector3f u = s.cross(f);

  Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();

  // Set Columns (Eigen is Column-Major)
  mat.col(0).head<3>() = s;
  mat.col(1).head<3>() = u;
  mat.col(2).head<3>() = -f;

  // Translation part
  mat(0,3) = -s.dot(eye);
  mat(1,3) = -u.dot(eye);
  mat(2,3) =  f.dot(eye);

  return mat;
}

Eigen::Matrix4f App::createPerspectiveProjectionMatrix(float fovY, float aspect, float zNear, float zFar)
{
  float tanHalfFovy = std::tan(fovY * 0.5f);
  Eigen::Matrix4f m = Eigen::Matrix4f::Zero();

  m(0,0) = 1.0f / (aspect * tanHalfFovy);
  m(1,1) = -1.0f / (tanHalfFovy); // Negated for Vulkan Y-down
  m(2,2) = zFar / (zNear - zFar);
  m(2,3) = (zNear * zFar) / (zNear - zFar);
  m(3,2) = -1.0f; // This must be at (3,2) for Eigen's Col-Major layout

  return m;
}



void App::activate3DCamera(Eigen::Vector3f position, Eigen::Vector3f lookAt)
{
  default3DCamera->pos = position;
  default3DCamera->lookAt = lookAt;
  activeRenderCamera = default3DCamera;
}
void App::activate3DCamera()
{
  activeRenderCamera = default3DCamera;
}
void App::activateUICamera()
{
  activeRenderCamera = defaultUICamera;
}
void App::activateUICamera(Eigen::Vector3f position)
{
  activeRenderCamera = defaultUICamera;
  defaultUICamera->pos = position;
  defaultUICamera->lookAt = Eigen::Vector3f (position.x(), position.y(), -position.z());
}
void App::renderSphere(Transform transform, RenderHints renderHints)
{
  throw std::runtime_error("not yet implemented: renderSphere!");
}

void App::renderCylinder(Transform transform, RenderHints renderHints)
{
  throw std::runtime_error("not yet implemented: renderCylinder!");
}
uint32_t App::createTexture(const std::string &imagePath)
{
  auto bitmapData = rv::loadBitmapDataFromPath(imagePath);
  auto image = renderer->createImage(bitmapData);
  auto texture = renderer->createTexture(image);
  renderer->updateTextureDescriptorSet(diffuseTextureDescriptorSet, 0, globalTextureIndex, texture);
  return globalTextureIndex++;
}

}







