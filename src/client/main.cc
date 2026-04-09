
#include <Eigen/Dense>
#include <render.hh>
#include <opengl_renderer.hh>
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
  GameGraphicsData(tz::Renderer* renderer)
  {

    // Shaders form the programmable part of our graphics pipeline:
    tz::ShaderModule* vs = reinterpret_cast<tz::ShaderModule *>(new tz::OpenGLShaderModule());
    vs->init(tz::ShaderType::Vertex, vertexShaderSource);
    tz::ShaderModule* fs = reinterpret_cast<tz::ShaderModule *>(new tz::OpenGLShaderModule());
    fs->init(tz::ShaderType::Fragment, fragmentShaderSource);
    defaultShaderPipeline = reinterpret_cast<tz::ShaderPipeline *>(new tz::OpenGLShaderPipeline());
    defaultShaderPipeline->link({vs, fs});

    // The PipelineStateObject holds all the state
    // we need for a pipeline execution, i.e. a draw call.
    // Think of renderstate handling and description
    // of vertex attributes and uniform buffers etc.
    defaultPSO = new tz::PipelineStateObject {};
    defaultPSO->renderState.primitiveType = tz::PrimitiveType::Triangles;
    defaultPSO->renderState.cullMode = tz::CullMode::Back;
    defaultPSO->renderState.blending = false;
    defaultPSO->renderState.depthTesting = true;
    defaultPSO->renderState.fillMode = tz::FillMode::Solid;
    defaultPSO->renderState.frontFace = tz::FrontFace::CounterClockwise;
    defaultPSO->renderState.stencilTesting = false;
    defaultPSO->shaderPipeline = defaultShaderPipeline;
    defaultPSO->vertexLayout.bindings =
      {tz::VertexBinding {
        .bufferSlot = 0,
        .stride = sizeof(float) * 3,
        .vertexInputRate=tz::VertexInputRate::PerVertex,
      }};
    defaultPSO->vertexLayout.attributes =
      {
        tz::VertexAttribute {
          .shaderLocation = 0,
          .bufferSlot = 0,
          .dataType = tz::DataType::Float,
          .componentCount = 3,
          .offset = 0
        }
      };

      tz::DescriptorBinding transformBinding;
      transformBinding.set = 0;
      transformBinding.binding = 1;
      transformBinding.type = tz::ResourceType::Ubo;
      defaultPSO->descriptorBindings = {
        transformBinding
      };

      auto transform = Eigen::Affine3f::Identity();
      transform.translate(Eigen::Vector3f(-0.2, 0.5, 0));
      transformBuffer = renderer->createBuffer(sizeof(Eigen::Matrix4f), transform.matrix().data());
      auto transformDescriptor = tz::Descriptor();
      transformDescriptor.buffer = transformBuffer;
      transformDescriptor.binding = transformBinding;

      std::vector<Eigen::Vector3f> positions =
        {{-0.2, 0.2, 0},
        {-0.2, -0.2, 0},
        {0.2, -0.2, 0}};
    auto vertexBuffer = renderer->createVertexBuffer(positions);



    // The actual commands which shall be executed during the pipeline execution
    // are recorded into a commandbuffer.
    // One commandbuffer can hold many commands and also include multiple draw commands.
    // During one frame, more than one commandbuffer may be executed.
    // Currently, we only allow sequential execution though, multithreaded/parallel execution
    // is a future feature.
    commandBuffer = new tz::CommandBuffer();
    commandBuffer->begin();
    commandBuffer->recordCommand(new tz::CmdBindPipeline (defaultPSO ));
    commandBuffer->recordCommand(new tz::CmdBindVertexBuffers({vertexBuffer}));
    commandBuffer->recordCommand(new tz::CmdBindDescriptors({transformDescriptor}));
    commandBuffer->recordCommand(new tz::CmdDraw(3, 1, 0, 0));
    commandBuffer->end();
  }



  tz::ShaderPipeline* defaultShaderPipeline;
  tz::PipelineStateObject* defaultPSO;
  tz::CommandBuffer* commandBuffer = nullptr;
  tz::Buffer* transformBuffer = nullptr;


  std::string vertexShaderSource = R"(
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

  std::string fragmentShaderSource = R"(
    #version 460 core
    out vec4 color;
    void main() {
        color = vec4(1, 1, 1, 1);
    }

    )";
};



void runActiveLoopDemo()
{

  tz::Renderer* renderer = reinterpret_cast<tz::Renderer *>(new tz::OpenGLRenderer());
  auto ws = new tz::SDL2WindowSystem();
  auto winDesc = renderer->getRequiredWindowDesc();
  auto window = ws->createWindow(winDesc);
  renderer->init(window);

  auto gameGraphicsData = GameGraphicsData(renderer);





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
      gameGraphicsData.transformBuffer->updateData(sizeof(Eigen::Matrix4f), transform.matrix().data());
    }
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


int main(int argc, char* argv[])
{

  runActiveLoopDemo();

  // runFrameCallbackDemo();
  // ...



  return 0;
}