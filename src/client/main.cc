
#include <Eigen/Dense>
#include <render.hh>
#include <opengl_renderer.hh>
#ifdef USE_VULKAN
#include <vulkan_renderer.hh>
#endif
#include <sdl2.hh>


/**
 * This represents all the graphical resources we need for our game client:
 * - shaders
 * - textures
 * - pipeline-states
 * - geometry
 */
struct GameGraphicsData
{

  // We collect information related to
  // the rendering of our client data
  // in a custom struct which holds our
  // shaders, PSO, uniforms matrices, textures etc.
  GameGraphicsData(tz::Renderer* renderer, tz::ShaderPipeline* shaderPipeline)
  {

    // The PipelineStateObject holds all the state
    // we need for a pipeline execution, i.e. a draw call.
    // Think of renderstate handling and description
    // of vertex attributes and uniform buffers etc.
    auto pso = new tz::PipelineStateObject {};
    pso->renderState.primitiveType = tz::PrimitiveType::Triangles;
    pso->renderState.cullMode = tz::CullMode::Back;
    pso->renderState.blending = false;
    pso->renderState.depthTesting = true;
    pso->renderState.fillMode = tz::FillMode::Solid;
    pso->renderState.frontFace = tz::FrontFace::CounterClockwise;
    pso->renderState.stencilTesting = false;
    pso->shaderPipeline = shaderPipeline;
    pso->vertexLayout.bindings =
      {tz::VertexBinding {
        .bufferSlot = 0,
        .stride = sizeof(float) * 3,
        .vertexInputRate=tz::VertexInputRate::PerVertex,
      }};
    pso->vertexLayout.attributes =
      {
        tz::VertexAttribute {
          .shaderLocation = 0,
          .bufferSlot = 0,
          .dataType = tz::DataType::Float,
          .componentCount = 3,
          .offset = 0
        }
      };

      defaultPSO = renderer->createPipelineStateObject(pso->renderState, pso->shaderPipeline,
                                                     pso->vertexLayout, pso->descriptorBindings);

      tz::DescriptorBinding transformBinding;
      transformBinding.set = 0;
      transformBinding.binding = 1;
      transformBinding.type = tz::ResourceType::Ubo;
      defaultPSO->descriptorBindings = {
        transformBinding
      };

      auto transform = Eigen::Affine3f::Identity();
      transform.translate(Eigen::Vector3f(-0.2, 0.5, 0));
      Eigen::Matrix4f m = transform.matrix();
      std::vector<Eigen::Matrix4f> vm = {m};
      transformBuffer = renderer->createBuffer(vm.data(), vm.size() * sizeof(Eigen::Matrix4f), tz::BufferUsage::Uniform);
      transformDescriptor = new tz::Descriptor();
      transformDescriptor->buffer = transformBuffer;
      transformDescriptor->binding = transformBinding;

      std::vector<Eigen::Vector3f> positions =
        {{-0.2, 0.2, 0},
        {-0.2, -0.2, 0},
        {0.2, -0.2, 0}};
      triangleVertexBuffer = renderer->createBuffer(positions.data(), positions.size() * sizeof(Eigen::Vector3f), tz::BufferUsage::Vertex);

    // The actual commands which shall be executed during the pipeline execution
    // are recorded into a commandbuffer.
    // One commandbuffer can hold many commands and also include multiple draw commands.
    // During one frame, more than one commandbuffer may be executed.
    // Currently, we only allow sequential execution though, multithreaded/parallel execution
    // is a future feature.
    commandBuffer = renderer->createCommandBuffer();

  }



  tz::ShaderPipeline* defaultShaderPipeline;
  tz::PipelineStateObject* defaultPSO;
  tz::CommandBuffer* commandBuffer = nullptr;
  tz::Buffer* transformBuffer = nullptr;
  tz::Buffer* triangleVertexBuffer = nullptr;
  tz::Descriptor* transformDescriptor = nullptr;

};

std::string getGLSLVertexShader() {
  return R"(
#version 460 core
layout(location = 0) in vec3 pos;

layout(std140, binding = 0) uniform Camera
{
  mat4 view;
  mat4 proj;
} camera;

layout(std140, binding = 1) uniform Transform
{
  mat4 world;
} transform;

void main() {
    //gl_Position = camera.proj * camera.view * transform.world * vec4(pos, 1);
    gl_Position = transform.world * vec4(pos, 1);
}
)";
}

std::string getGLSLFragmentShader()
{
  return R"(
    #version 460 core
    out vec4 color;
    void main() {
        color = vec4(1, 1, 1, 1);
    }

    )";
}


void runDemoGL()
{

  tz::Renderer* renderer = reinterpret_cast<tz::Renderer *>(new tz::OpenGLRenderer());
  auto ws = new tz::SDL2WindowSystem();
  auto winDesc = renderer->getRequiredWindowDesc();
  auto window = ws->createWindow(winDesc);
  renderer->init(window);

  const std::string& vertexShaderSource = getGLSLVertexShader();
  const std::string& fragmentShaderSource = getGLSLFragmentShader();
  auto vs = renderer->createShaderModule(tz::ShaderType::Vertex, vertexShaderSource);
  auto fs = renderer->createShaderModule(tz::ShaderType::Fragment, fragmentShaderSource);

  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});

  auto gameGraphicsData = GameGraphicsData(renderer, shaderPipeline);

  while (true)
  {
    ws->pollEvents();
    renderer->beginFrame();
    renderer->clearScreen();

    // Immediate style rendering, similar to how OpenGL 2.0
    // worked with its "glVertex" and "glNormal" calls.
    // In the background these immediate calls are interpreted and
    // mapped to "modern-gpu" architecture, uploading into VertexBuffers etc.
    // The framework does make its best to interpret the incoming immediate
    // commands efficiently, but immediate-mode will normally not be as performant
    // as custom built buffers, pipeline states and shaders.
    renderer->beginDraw(tz::PrimitiveType::Triangles);
    renderer->emitPosition({-0.1, 0.1, -0.1});
    renderer->emitPosition({-0.1, -0.1, -0.1});
    renderer->emitPosition({0.1, -0.1, -0.1});

    renderer->emitPosition({-0.09, 0.1, 0});
    renderer->emitPosition({0.11, -0.1, 0});
    renderer->emitPosition({0.11, 0.1, 0});
    renderer->endDraw();

    // Use modern command buffer approach.
    // Every drawing/pipeline execution is materialised through
    // specific commands which are recorded into a (reusable)
    // commandBuffer, which gets submitted.
    // Any number of command buffers may be submitted per frame.
    // Submission it not immediate execution, but collection of the buffer.
    // Update our transform buffer, making the object move:
    {
      static Eigen::Vector3f posOffset = {0, 0,0};
      posOffset += Eigen::Vector3f {0.000, -0.000, 0};
      auto transform = Eigen::Affine3f::Identity();
      transform.translate(posOffset);
      static float angle = 0.0f;
      angle += 0.0001f;
      transform.rotate(Eigen::AngleAxis(angle, Eigen::Vector3f::UnitZ()));
      gameGraphicsData.transformBuffer->updateData(transform.matrix().data(), sizeof(Eigen::Matrix4f));
    }
    gameGraphicsData.commandBuffer->begin();
    gameGraphicsData.commandBuffer->recordCommand(new tz::CmdBindPipeline (gameGraphicsData.defaultPSO ));
    gameGraphicsData.commandBuffer->recordCommand(new tz::CmdBindVertexBuffers ({gameGraphicsData.triangleVertexBuffer}));
    gameGraphicsData.commandBuffer->recordCommand(new tz::CmdBindDescriptors({gameGraphicsData.transformDescriptor}));
    gameGraphicsData.commandBuffer->recordCommand(new tz::CmdDraw(3, 1, 0, 0));
    gameGraphicsData.commandBuffer->end();
    renderer->submitCommandBuffer(gameGraphicsData.commandBuffer);

    // Ending the frame is where all the drawing & computation work is actually being
    // done on the GPU.
    // For legacy frameworks as OpenGL this means all the actual draw calls are done.
    // For modern APIs such as Vulkan and DX12, it means the actual submission
    // of their internal command buffers, fence waiting etc.
    renderer->endFrame();


    // Final step for a frame to present the contents of the backbuffer
    // to the actual display.
    ws->present();
  }

}

#ifdef USE_VULKAN
void runDemoVulkan()
{
  tz::Renderer* renderer = reinterpret_cast<tz::Renderer *>(new tz::render::vulkan::VulkanRenderer());
  auto ws = new tz::SDL2WindowSystem();
  auto winDesc = renderer->getRequiredWindowDesc();
  auto window = ws->createWindow(winDesc);
  renderer->init(window);

  auto spvPath = "shader_binaries/default_shader.slang.spv";

  auto vs = renderer->createShaderModule(tz::ShaderType::Vertex, spvPath);
  auto fs = renderer->createShaderModule(tz::ShaderType::Fragment, spvPath);

  auto shaderPipeline = renderer->createShaderPipeline({vs, fs});
  auto gameGraphicsData = GameGraphicsData(renderer, shaderPipeline);

  while (true)
  {
    ws->pollEvents();
    renderer->beginFrame();
    renderer->clearScreen();

    {
      static Eigen::Vector3f posOffset = {0, 0,0};
      posOffset += Eigen::Vector3f {0.000, -0.000, 0};
      auto transform = Eigen::Affine3f::Identity();
      transform.translate(posOffset);
      static float angle = 0.0f;
      angle += 0.0001f;
      transform.rotate(Eigen::AngleAxis(angle, Eigen::Vector3f::UnitZ()));
      gameGraphicsData.transformBuffer->updateData(transform.matrix().data(), sizeof(Eigen::Matrix4f));
    }

    gameGraphicsData.commandBuffer->begin();
    gameGraphicsData.commandBuffer->recordCommand(new tz::CmdBindPipeline (gameGraphicsData.defaultPSO ));
    gameGraphicsData.commandBuffer->recordCommand(new tz::CmdBindVertexBuffers ({gameGraphicsData.triangleVertexBuffer}));
    gameGraphicsData.commandBuffer->recordCommand(new tz::CmdBindDescriptors({gameGraphicsData.transformDescriptor}));
    gameGraphicsData.commandBuffer->recordCommand(new tz::CmdDraw(3, 1, 0, 0));
    gameGraphicsData.commandBuffer->end();
    renderer->submitCommandBuffer(gameGraphicsData.commandBuffer);

    // Ending the frame is where all the drawing & computation work is actually being
    // done on the GPU.
    // For legacy frameworks as OpenGL this means all the actual draw calls are done.
    // For modern APIs such as Vulkan and DX12, it means the actual submission
    // of their internal command buffers, fence waiting etc.
    renderer->endFrame();


    // Final step for a frame to present the contents of the backbuffer
    // to the actual display.
    ws->present();
  }
}
#endif


int main(int argc, char* argv[])
{
#ifndef USE_VULKAN
  runDemoGL();
#else
  runDemoVulkan();
#endif


  return 0;
}