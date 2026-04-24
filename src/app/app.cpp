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

  default3DCamera = new Camera({0, 2, 3}, {0, 0, 0 }, CameraType::Perspective);
  defaultUICamera = new Camera({0, 0, 1}, {0, 0, 0}, CameraType::Ortho);

}

void App::prepareRenderPrimitives()
{
  // First we create all the shaders we need:
  auto spvPath = "shader_binaries/shader_colored.slang.spv";
  auto vs = renderer->createShaderModule(tz::ShaderType::Vertex, spvPath);
  auto fs = renderer->createShaderModule(tz::ShaderType::Fragment, spvPath);
  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  std::vector<tz::VertexPos> verticesPos =
    {
      {{-0.5, 0.5, 0.5}},
      {{-0.5, -0.5, 0.5}},
      {{0.5, -0.5, 0.5}},
      {{0.5, 0.5, 0.5}}
    };

  quadIndices =
    {
      0, 1, 2,
      0, 2, 3
    };

  quadVertexBuffer = renderer->createBuffer(verticesPos.data(),
                                                 verticesPos.size() * sizeof (tz::VertexPos),
                                                 tz::BufferUsage::Vertex);

  quadIndexBuffer = renderer->createBuffer(quadIndices.data(),
                                                quadIndices.size() * sizeof(uint32_t),
                                                tz::BufferUsage::Index);

  colorOnlyPSO = createColorOnlyPSO();

  commandBuffer = renderer->createCommandBuffer();

}

tz::PipelineStateObject* App::createColorOnlyPSO()
{

  auto spvPath = "shader_binaries/default_colored_transform.slang.spv";

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
      .stride = sizeof(tz::VertexPos),
      .vertexInputRate=tz::VertexInputRate::PerVertex,
  }};
  vertexLayout.attributes = {
    tz::VertexAttribute {
      .shaderLocation = 0,
      .bufferSlot = 0,
      .dataType = tz::DataType::Float,
      .componentCount = 3,
      .offset = 0
    }
  };

  // Descriptor layout and binding for the transformation matrix

  // TODO: we should have the client of the app make this configurable, though a sensible default value makes sense.
  // In this case 100 * 192 bytes = 188KB of memory, seems ok even for smaller GPUs.
  int maxNumberOfObjects = 1000;
  auto transformBuffer = renderer->createMultiframeUniformBuffer(maxNumberOfObjects, sizeof(TransformUniformBufferObject));
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
    //windowSystem->present();

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
  uint32_t counter = 0;
  for (auto& prd : framePrimitives)
  {

    if (prd.geometryType == PrimitiveGeometryType::Quad)
    {
        auto transform = Eigen::Affine3f::Identity();
        transform.translate(prd.position);
        transform.scale(prd.scale);
        Eigen::Matrix4f tm = transform.matrix();
        tz::TransformUniformBufferObject transformUBO;
        transformUBO.model = tm;
        //transformUBO.view = createLookAtMatrix({0, 0, 15}, {0, 0, 0}, {0, 1,0});
        transformUBO.view = prd.associatedCamera->getViewMatrix();
        //transformUBO.proj = createPerspectiveProjectionMatrix(1.04f, 640.0f / 480.0f, 0.1f, 100.0f);
        transformUBO.proj = prd.associatedCamera->getProjectionMatrix(640, 480);
        renderer->updateBuffer(colorOnlyPSO->descriptorSets[0]->layout->descriptorBindings[0]->buffer, &transformUBO, sizeof(tz::TransformUniformBufferObject), counter);

        // Decide based on the material subtype which pipeline we should use:
        if (prd.materialType == PrimitiveMaterialType::SingleColor) {
          renderer->recordCommand(commandBuffer,new tz::CmdBindPipeline (colorOnlyPSO));
          renderer->recordCommand(commandBuffer, new tz::CmdBindDescriptors(colorOnlyPSO->descriptorSets, colorOnlyPSO, {counter}));
        }

        renderer->recordCommand(commandBuffer, new tz::CmdSetViewPorts({{0, 0, 640, 480}}));
        renderer->recordCommand(commandBuffer, new tz::CmdSetScissors({{0, 0, 640, 480}}));
        renderer->recordCommand(commandBuffer, new tz::CmdBindVertexBuffers ({quadVertexBuffer}));
        renderer->recordCommand(commandBuffer, new tz::CmdBindIndexBuffer(quadIndexBuffer, 0));
        renderer->recordCommand(commandBuffer, new tz::CmdDrawIndexed(quadIndices.size(), 1,0, 0, 0));

    }
    counter++;
  }


  renderer->endCommandBuffer(commandBuffer);
  renderer->submitCommandBuffer(commandBuffer);
  renderer->endFrame();

  framePrimitives.clear();


}
float App::getLastFrameTime()
{
  return 16.667f;
}
void App::renderCube(Eigen::Vector3f position) {

}
void App::renderColoredQuad(Eigen::Vector3f position, Eigen::Vector3f scale, Eigen::Vector3f color)
{
  PrimitiveRenderData prd;
  prd.position = position;
  prd.scale = scale;
  prd.geometryType     = PrimitiveGeometryType::Quad;
  prd.color = color;
  prd.materialType = PrimitiveMaterialType::SingleColor;
  prd.associatedCamera = activeRenderCamera;
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

}







