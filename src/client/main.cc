
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
  tz::ShaderPipeline* defaultShaderPipeline;
  tz::PipelineStateObject defaultPSO;
};

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

GameGraphicsData initializeGameGraphicsData()
{
  GameGraphicsData graphicsData;
  auto vs = new tz::OpenGLShaderModule();
  vs->init(tz::ShaderType::Vertex, vertexShaderSource);
  auto fs = new tz::OpenGLShaderModule();
  fs->init(tz::ShaderType::Fragment, fragmentShaderSource);
  graphicsData.defaultShaderPipeline = new tz::OpenGLShaderPipeline();
  graphicsData.defaultShaderPipeline->link({vs, fs});

  graphicsData.defaultPSO = tz::PipelineStateObject {};
  graphicsData.defaultPSO.renderState.primitiveType = tz::PrimitiveType::Triangles;
  graphicsData.defaultPSO.renderState.cullMode = tz::CullMode::Back;
  graphicsData.defaultPSO.renderState.blending = false;
  graphicsData.defaultPSO.renderState.depthTesting = true;
  graphicsData.defaultPSO.renderState.fillMode = tz::FillMode::Solid;
  graphicsData.defaultPSO.renderState.frontFace = tz::FrontFace::CounterClockwise;
  graphicsData.defaultPSO.renderState.stencilTesting = false;
  graphicsData.defaultPSO.shaderPipeline = graphicsData.defaultShaderPipeline;
  graphicsData.defaultPSO.vertexLayout.bindings =
    {tz::VertexBinding {
      .bufferSlot = 0,
      .stride = sizeof(float) * 3,
      .vertexInputRate=tz::VertexInputRate::PerVertex,
      }};
  graphicsData.defaultPSO.vertexLayout.attributes =
    {
      tz::VertexAttribute {
        .shaderLocation = 0,
        .bufferSlot = 0,
        .dataFormat = tz::AttributeDataFormat::R32G32B32A32_FLOAT,
        .offset = 0
      }
    };

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
    //

    // Rendering in the more modern way, by providing
    // buffers and PSOs directly:
    gameGraphicsData->
    renderer->drawMesh(mesh);

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