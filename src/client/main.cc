
#include <Eigen/Dense>
import app;
import render.base;
import render.opengl;
import windowing.sdl2;


/**
 * This represents all the graphical resources we need for our game client:
 * - shaders
 * - textures
 * - pipeline-states
 * - geometry
 */
struct GameGraphicsData
{
  GameGraphicsData()
  {

    auto vs = new tz::OpenGLShaderModule();
    vs->init(tz::ShaderType::Vertex, vertexShaderSource);
    auto fs = new tz::OpenGLShaderModule();
    fs->init(tz::ShaderType::Fragment, fragmentShaderSource);
    defaultShaderPipeline = new tz::OpenGLShaderPipeline();
    defaultShaderPipeline->link({vs, fs});

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
          .dataFormat = tz::AttributeDataFormat::R32G32B32A32_FLOAT,
          .offset = 0
        }
      };

    auto vertexBuffer = new tz::VertexBuffer();

    commandBuffer = new tz::CommandBuffer();
    commandBuffer->begin();
    commandBuffer->recordCommand(new tz::CmdBindPipeline (defaultPSO ));
    commandBuffer->recordCommand(new tz::CmdBindVertexBuffers({vertexBuffer}));
    commandBuffer->recordCommand(new tz::CmdDraw(3, 1, 0, 0));
    commandBuffer->end();
  }

  tz::ShaderPipeline* defaultShaderPipeline;
  tz::PipelineStateObject* defaultPSO;
  tz::CommandBuffer* commandBuffer = nullptr;


  std::string vertexShaderSource = R"(
    #version 460 core
    layout(location = 0) in vec3 pos;
    void main() {
        gl_Position = vec4(pos, 1);
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



GameGraphicsData initializeGameGraphicsData()
{
  GameGraphicsData graphicsData;


  return graphicsData;
}


void runActiveLoopDemo()
{

  auto renderer = new tz::OpenGLRenderer();
  auto ws = new tz::SDL2WindowSystem();
  auto winDesc = renderer->getRequiredWindowDesc();
  auto window = ws->createWindow(winDesc);
  renderer->init(window);

  auto gameGraphicsData = initializeGameGraphicsData();

//  auto mesh = renderer->createMesh(
//    {{-0.5, 0.5, 0},
//    {-0.5, -0.5, 0},
//    {0.2, -0.5, 0}});



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
    renderer->emitPosition({-0.5, 0.5, 0});
    renderer->emitPosition({-0.5, -0.5, 0});
    renderer->emitPosition({0.2, -0.5, 0});

    renderer->emitPosition({-0.4, 0.5, 0});
    renderer->emitPosition({0.3, -0.5, 0});
    renderer->emitPosition({0.3, 0.5, 0});
    renderer->endDraw();

    // Draw with more modern command-buffer approach:
    renderer->executeCommandBuffer(gameGraphicsData.commandBuffer);

    renderer->endFrame();
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