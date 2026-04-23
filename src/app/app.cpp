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
  std::vector<uint32_t> indices =
    {
      0, 1, 2,
      0, 2, 3
    };
  auto quadVertexBuffer = renderer->createBuffer(verticesPosTexCoord.data(),
                                                 verticesPosTexCoord.size() * sizeof (tz::VertexPosTexCoords),
                                                 tz::BufferUsage::Vertex);

  auto quadIndexBuffer = renderer->createBuffer(indices.data(),
                                                indices.size() * sizeof(uint32_t),
                                                tz::BufferUsage::Index);

  colorOnlyPSO = createColorOnlyPSO();

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

}
float App::getLastFrameTime()
{
  return 16.667f;
}
void App::renderCube(Eigen::Vector3f position) {

}
void App::renderColoredQuad(Eigen::Vector3f position)
{

}

}







