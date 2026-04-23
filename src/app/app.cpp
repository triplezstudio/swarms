#include <SDL2/SDL.h>
#include <iostream>
#include <functional>

#include "app.hh"
#include <render.hh>
#include <vulkan_renderer.hh>
#include <window_system.hh>
#include <sdl2.hh>

namespace tz
{

App::App()
{
  renderer = reinterpret_cast<tz::Renderer *>(new tz::render::vulkan::VulkanRenderer());
  windowSystem = new tz::SDL2WindowSystem();
  auto winDesc = renderer->getRequiredWindowDesc();
  auto window = windowSystem->createWindow(winDesc);
  renderer->init(window);

  // Prepare so we can implement the convenience functions
  // for rendering cubes, spheres, meshes etc.
  prepareRenderPrimitives();

}

void App::prepareRenderPrimitives()
{
  // First we create all the shaders we need:
  auto spvPath = "shader_binaries/shader_colored.slang.spv";
  auto vs = renderer->createShaderModule(tz::ShaderType::Vertex, spvPath);
  auto fs = renderer->createShaderModule(tz::ShaderType::Fragment, spvPath);
  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  std::vector<tz::VertexPosColor> verticesPosTexCoord =
    {
      {{-0.8, 0.5, 0.5}, {1, 1,1}},
      {{-0.8, -0.5, 0.5}, {1, 1,1,}},
      {{-0.4, -0.5, 0.5},  {1, 1,1}},
      {{-0.4, 0.5, 0.5}, {1, 1,1}}
    };

  quadIndices =
    {
      0, 1, 2,
      0, 2, 3
    };

  quadVertexBuffer = renderer->createBuffer(verticesPosTexCoord.data(),
                                                 verticesPosTexCoord.size() * sizeof (tz::VertexPosTexCoords),
                                                 tz::BufferUsage::Vertex);

  quadIndexBuffer = renderer->createBuffer(quadIndices.data(),
                                                quadIndices.size() * sizeof(uint32_t),
                                                tz::BufferUsage::Index);

  colorOnlyPSO = createColorOnlyPSO();

  auto transform = Eigen::Affine3f::Identity();
  transform.translate(Eigen::Vector3f(0.5, 0, 0));
  Eigen::Matrix4f tm = transform.matrix();
  Eigen::Matrix4f m = Eigen::Matrix4f::Identity();
  std::vector<Eigen::Matrix4f> vm = {m};
  tz::TransformUniformBufferObject transformUBO;
  transformUBO.model = tm;
  transformUBO.view = createLookAtMatrix({0, 0, 2.17}, {0, 0, 0}, {0, 1,0});
  transformUBO.proj = createPerspectiveProjectionMatrix(1.04f, 640.0f / 480.0f, 0.1f, 100.0f);
  transformUBO.proj(0,0) *= -1.0f;
  renderer->updateBuffer(colorOnlyPSO->descriptorSets[0]->layout->descriptorBindings[0]->buffer, &transformUBO, sizeof(tz::TransformUniformBufferObject));

  commandBuffer = renderer->createCommandBuffer();

}

tz::PipelineStateObject* App::createColorOnlyPSO()
{

  auto spvPath = "shader_binaries/default_shader_stage3.slang.spv";

  auto vs = renderer->createShaderModule(tz::ShaderType::Vertex, spvPath);
  auto fs = renderer->createShaderModule(tz::ShaderType::Fragment, spvPath);
  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  auto renderState = tz::RenderState {};
  renderState.primitiveType = tz::PrimitiveType::Triangles;
  renderState.cullMode = tz::CullMode::Back;
  renderState.blending = false;
  renderState.depthTesting = true;
  renderState.fillMode = tz::FillMode::Solid;
  renderState.frontFace = tz::FrontFace::CounterClockwise;
  renderState.stencilTesting = false;
  shaderPipeline = shaderPipeline;

  auto vertexLayout = tz::VertexLayout {};
  vertexLayout.bindings =  {tz::VertexBinding {
      .bufferSlot = 0,
      .stride = sizeof(tz::VertexPosColor),
      .vertexInputRate=tz::VertexInputRate::PerVertex,
  }};
  vertexLayout.attributes = {
    tz::VertexAttribute {
      .shaderLocation = 0,
      .bufferSlot = 0,
      .dataType = tz::DataType::Float,
      .componentCount = 3,
      .offset = 0
    },
    {
      tz::VertexAttribute
      {
        .shaderLocation = 1,
        .bufferSlot = 0,
        .dataType = tz::DataType::Float,
        .componentCount = 3,
        .offset = sizeof(float) * 3
      }
    }
  };

  // Descriptor layout and binding for the transformation matrix:
  auto transformBuffer = renderer->createMultiframeBuffer(nullptr,
                                                          sizeof(tz::TransformUniformBufferObject),
                                                          tz::BufferUsage::Uniform);
  auto transformDescBinding = renderer->createDescriptorBinding(0,
                                                                tz::ResourceType::Ubo,
                                                                tz::ShaderType::Vertex,
                                                                1,
                                                                transformBuffer);
  auto descriptorSetLayout = renderer->createDescriptorSetLayout({transformDescBinding});
  auto descriptorSet = renderer->createMultiframeDescriptorSet(descriptorSetLayout);

  auto pso = renderer->createPipelineStateObject(renderState,
                                                 shaderPipeline,
                                                 vertexLayout,
                                                            {descriptorSetLayout});
  pso->descriptorSets = {descriptorSet};
  return pso;
}

void tz::App::run()
{
  while (true)
  {

    windowSystem->pollEvents();
    updateFrameListeners(16.66f);
    windowSystem->present();

  }

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

  renderer->beginFrame();
  renderer->beginCommandBuffer(commandBuffer);


  // Render everything submitted during this frame:
  for (auto& prd : framePrimitives)
  {
    if (prd.type == PrimitiveRenderType::Quad)
    {
        renderer->recordCommand(commandBuffer,new tz::CmdBindPipeline (colorOnlyPSO) );
        renderer->recordCommand(commandBuffer, new tz::CmdSetViewPorts({{0, 0, 640, 480}}));
        renderer->recordCommand(commandBuffer, new tz::CmdSetScissors({{0, 0, 640, 480}}));
        renderer->recordCommand(commandBuffer, new tz::CmdBindVertexBuffers ({quadVertexBuffer}));
        renderer->recordCommand(commandBuffer, new tz::CmdBindIndexBuffer(quadIndexBuffer, 0));
        renderer->recordCommand(commandBuffer, new tz::CmdBindDescriptors(colorOnlyPSO->descriptorSets, colorOnlyPSO));
        renderer->recordCommand(commandBuffer, new tz::CmdDrawIndexed(quadIndices.size(), 1,0, 0, 0));
    }
  }

  framePrimitives.clear();
  renderer->endCommandBuffer(commandBuffer);
  renderer->submitCommandBuffer(commandBuffer);


}
float App::getLastFrameTime()
{
  return 16.667f;
}
void App::renderCube(Eigen::Vector3f position) {

}
void App::renderColoredQuad(Eigen::Vector3f position)
{
  PrimitiveRenderData prd;
  prd.position = position;
  prd.type = PrimitiveRenderType::Quad;
  framePrimitives.push_back(prd);
}


// TODO move into math or other helper library (might even already exist!?)
Eigen::Matrix4f App::createLookAtMatrix(const Eigen::Vector3f& eye,
                       const Eigen::Vector3f& center,
                       const Eigen::Vector3f& up)
{
  Eigen::Vector3f f = (center - eye).normalized();
  Eigen::Vector3f s = f.cross(up).normalized();
  Eigen::Vector3f u = s.cross(f);

  Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();

  // Rotation part (Indices are mat(row, col))
  mat(0,0) =  s.x();
  mat(0,1) =  s.y(); // Changed from (1,0)
  mat(0,2) =  s.z(); // Changed from (2,0)

  mat(1,0) =  u.x();
  mat(1,1) =  u.y();
  mat(1,2) =  u.z();

  mat(2,0) = -f.x();
  mat(2,1) = -f.y();
  mat(2,2) = -f.z();

  // Translation part (Now in the 4th Column)
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
  m(1,1) = 1.0f / (tanHalfFovy);

  // Vulkan Depth [0, 1]
  m(2,2) = zFar / (zNear - zFar);
  m(2,3) = (zNear * zFar) / (zNear - zFar); // Translation moved to Col 3

  // Perspective Divide
  m(3,2) = -1.0f; // Moved from (2,3) to (3,2) for Column-Major

  return m;
}



}







