#include <SDL2/SDL.h>
#include <iostream>
#include <functional>
#include "app.hh"
#include <vulkan_renderer.hh>
#include <window_system.hh>
#include <sdl2.hh>

namespace tz
{
namespace rv = render::vulkan;

App::App()
{
  renderer = new rv::Renderer();
  windowSystem = new tz::SDL2WindowSystem();
  auto winDesc = renderer->getRequiredWindowDesc();
  auto window = windowSystem->createWindow(winDesc);
  renderer->init(window);

  createMasterPipelineLayout();
  prepareRenderPrimitives();
  buildPSOCache();
  commandBuffer = renderer->createCommandBuffer();

  default3DCamera = new Camera({0, 2, 3}, {0, 0, 0 }, CameraType::Perspective);
  defaultUICamera = new Camera({0, 0, 1}, {0, 0, 0}, CameraType::Ortho);

}

void App::createMasterPipelineLayout()
{

  // Camera is set0, binding0
  auto cameraBuffer = renderer->createMultiframeUniformBuffer(2, sizeof(tz::CameraUniformBufferObject));
  auto cameraUBOBinding = vulkanRenderer()->createDescriptorBinding(0,
                                                                    rv::DescriptorResourceType::Ubo,
                                                                    rv::ShaderType::Vertex, 1,
                                                                    cameraBuffer);
  auto cameraDescriptorSetLayout =  (vulkanRenderer()->createDescriptorSetLayout({cameraUBOBinding}));
  cameraDescriptorSet = vulkanRenderer()->createMultiframeDescriptorSet(cameraDescriptorSetLayout);

  // PerObject is set1, binding0
  auto perObjectBuffer = renderer->createMultiframeUniformBuffer(100, sizeof(tz::PerObjectUniformBufferObject));
  auto perObjectUBOBinding = renderer->createDescriptorBinding(0, rv::DescriptorResourceType::Ubo,
                                                               rv::ShaderType::Vertex, 1,
                                                               perObjectBuffer);
  auto perObjectDescriptorSetLayout = renderer->createDescriptorSetLayout({perObjectUBOBinding});
  perObjectDescriptorSet = renderer->createMultiframeDescriptorSet(perObjectDescriptorSetLayout);

  // Diffuse textures at set2, binding0.
  // We allow up to 1000 textures
  auto textureDescBinding = renderer->createDescriptorBinding(0, rv::DescriptorResourceType::Sampler,
                                                              rv::ShaderType::Fragment, 1000, nullptr, nullptr);

  auto diffuseTextureDescriptorSetLayout = renderer->createDescriptorSetLayout({textureDescBinding}, true);
  diffuseTextureDescriptorSet = renderer->createMultiframeDescriptorSet(diffuseTextureDescriptorSetLayout);

  masterPipelineLayout = renderer->createPipelineLayout({cameraDescriptorSetLayout, perObjectDescriptorSetLayout, diffuseTextureDescriptorSetLayout});


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
    {{-1.0f, -1.0f,  1.0f}}, // 0: Front-Bottom-Left
    {{ 1.0f, -1.0f,  1.0f}}, // 1: Front-Bottom-Right
    {{ 1.0f,  1.0f,  1.0f}}, // 2: Front-Top-Right
    {{-1.0f,  1.0f,  1.0f}}, // 3: Front-Top-Left
    {{-1.0f, -1.0f, -1.0f}}, // 4: Back-Bottom-Left
    {{ 1.0f, -1.0f, -1.0f}}, // 5: Back-Bottom-Right
    {{ 1.0f,  1.0f, -1.0f}}, // 6: Back-Top-Right
    {{-1.0f,  1.0f, -1.0f}}  // 7: Back-Top-Left
  };

  std::vector<rv::VertexPosTexCoords> cubeVerticesPosTex = {
    // Front face (Z = 1.0f)
    {{-1.0f, -1.0f,  1.0f}, {0.0f, 0.0f}}, {{ 1.0f, -1.0f,  1.0f}, {1.0f, 0.0f}},
    {{ 1.0f,  1.0f,  1.0f}, {1.0f, 1.0f}}, {{-1.0f,  1.0f,  1.0f}, {0.0f, 1.0f}},

    // Back face (Z = -1.0f)
    {{ 1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}}, {{-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},
    {{-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}}, {{ 1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}},

    // Left face (X = -1.0f)
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}}, {{-1.0f, -1.0f,  1.0f}, {1.0f, 0.0f}},
    {{-1.0f,  1.0f,  1.0f}, {1.0f, 1.0f}}, {{-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}},

    // Right face (X = 1.0f)
    {{ 1.0f, -1.0f,  1.0f}, {0.0f, 0.0f}}, {{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},
    {{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}}, {{ 1.0f,  1.0f,  1.0f}, {0.0f, 1.0f}},

    // Top face (Y = 1.0f)
    {{-1.0f,  1.0f,  1.0f}, {0.0f, 0.0f}}, {{ 1.0f,  1.0f,  1.0f}, {1.0f, 0.0f}},
    {{ 1.0f,  1.0f, -1.0f}, {1.0f, 1.0f}}, {{-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f}},

    // Bottom face (Y = -1.0f)
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f}}, {{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f}},
    {{ 1.0f, -1.0f,  1.0f}, {1.0f, 1.0f}}, {{-1.0f, -1.0f,  1.0f}, {0.0f, 1.0f}}
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

}

rv::PipelineStateObject* App::createColorOnlyPSO()
{

  auto spvPath = "shader_binaries/default_colored_transform.slang.spv";

  auto vs = renderer->createShaderModule(rv::ShaderType::Vertex, spvPath);
  auto fs = renderer->createShaderModule(rv::ShaderType::Fragment, spvPath);
  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  auto renderState = rv::RenderState {};
  renderState.primitiveType = rv::PrimitiveType::Triangles;
  renderState.blending = false;
  renderState.depthTesting = true;
  renderState.fillMode = vk::PolygonMode::eLine;
  renderState.frontFace = vk::FrontFace::eClockwise;
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

rv::PipelineStateObject* App::createTexturedPSO()
{

  auto spvPath = "shader_binaries/default_textured.slang.spv";

  auto vs = renderer->createShaderModule(rv::ShaderType::Vertex, spvPath);
  auto fs = renderer->createShaderModule(rv::ShaderType::Fragment, spvPath);
  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  auto renderState = rv::RenderState {};
  renderState.primitiveType = rv::PrimitiveType::Triangles;
  renderState.blending = true;
  renderState.depthTesting = true;
  renderState.fillMode = vk::PolygonMode::eFill;
  renderState.frontFace = vk::FrontFace::eClockwise;
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
    updateFrameListeners(16.66f);
    renderFrame();
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

  for (auto& prd : primitives)
  {
    auto pso = psoCache[prd.renderHints.getHash()];
    renderer->recordCommand(commandBuffer,new rv::CmdBindPipeline (pso));

    auto transform = Eigen::Affine3f::Identity();  
    transform.translate(prd.transform.position);
    transform.scale(prd.transform.scale);
    Eigen::Matrix4f tm = transform.matrix();
    tz::PerObjectUniformBufferObject perObjectUBO;
    perObjectUBO.model = tm;
    perObjectUBO.textureId = prd.renderHints.texture;
    renderer->updateBuffer(perObjectDescriptorSet->layout->descriptorBindings[0]->buffer, &perObjectUBO, sizeof(tz::PerObjectUniformBufferObject),
                           primitiveCounter);
    renderer->recordCommand(commandBuffer, new rv::CmdBindDescriptors({perObjectDescriptorSet}, masterPipelineLayout, {primitiveCounter}, 1));

    renderer->recordCommand(commandBuffer, new rv::CmdSetViewPorts({{0, 0, 640, 480}}));
    renderer->recordCommand(commandBuffer, new rv::CmdSetScissors({{0, 0, 640, 480}}));

    renderer->recordCommand(commandBuffer,new rv::CmdBindVertexBuffers({prd.vertexBuffer}));
    renderer->recordCommand(commandBuffer, new rv::CmdBindIndexBuffer(prd.indexBuffer, 0));
    renderer->recordCommand(commandBuffer, new rv::CmdDrawIndexed(prd.indexCount, 1,0, 0, 0));

    primitiveCounter++;
  }
}

void App::renderFrame()
{
  renderer->beginFrame();
  renderer->beginCommandBuffer(commandBuffer);

  // We can bind our diffuseTextureDescriptorSet once at the beginning of the frame.
  // This descriptorSet contains slots for up to 1000 textures.
  // The actual texture is then just indexed by the individual rendered object (see renderPrimitives)
  renderer->recordCommand(commandBuffer, new rv::CmdBindDescriptors({diffuseTextureDescriptorSet}, masterPipelineLayout, {0}, 2));

  // We are rendering the scene ordered by "camera".
  // First everything which has the 3d scene camera,
  // then the 2d ui camera.
  // This is more efficient in terms of pipeline-binding and alos
  // makes sure, UI always renders on top of everything else.
  auto camera3DPrimitives = getRenderPrimitivesByCamera(default3DCamera);
  tz::CameraUniformBufferObject cameraUBO;
  cameraUBO.view = default3DCamera->lookAtRH();
  cameraUBO.proj = default3DCamera->getProjectionMatrix(640, 480);
  renderer->recordCommand(commandBuffer, new rv::CmdBindDescriptors({cameraDescriptorSet}, masterPipelineLayout, {0}, 0));
  renderer->updateBuffer(cameraDescriptorSet->layout->descriptorBindings[0]->buffer, &cameraUBO, sizeof(tz::CameraUniformBufferObject),
                         0);

  uint32_t primitiveCounter = 0;
  renderPrimitives(camera3DPrimitives, primitiveCounter);

  // Next UI:
  auto cameraUIPrimitives = getRenderPrimitivesByCamera(defaultUICamera);
  cameraUBO.view = defaultUICamera->lookAtRH();
  cameraUBO.proj = defaultUICamera->getProjectionMatrix(640, 480);
  renderer->recordCommand(commandBuffer, new rv::CmdBindDescriptors({cameraDescriptorSet}, masterPipelineLayout, {1}, 0));

  renderer->updateBuffer(cameraDescriptorSet->layout->descriptorBindings[0]->buffer, &cameraUBO,
                         sizeof(tz::CameraUniformBufferObject),
                         1);
  renderPrimitives(cameraUIPrimitives, primitiveCounter);

  renderer->endCommandBuffer(commandBuffer);
  renderer->submitCommandBuffer(commandBuffer);
  renderer->endFrame();

  framePrimitives.clear();
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







