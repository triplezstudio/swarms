#include "../render/scene/include/scene.hh"
#include <Eigen/Dense>
#include <app.hh>
#include <iostream>
#include <render_helpers.hh>
#include <text_render.hh>
#include <texture_asset_manager.hh>
#include <vulkan_renderer.hh>

uint32_t testImageTexture = 0;
uint32_t testImage2Texture = 0;
tz::render::TextRenderer* textRenderer = nullptr;
tz::Scene* main3DScene = nullptr;
tz::Scene* uiScene = nullptr;
tz::Camera* main3DCamera = nullptr;
tz::Camera* uiCamera = nullptr;
tz::render::Font* titleFont = nullptr;
tz::render::Font* smallUIFont = nullptr;

void initialize(tz::App* app)
{
  testImageTexture = app->getTextureAssetManager().loadTexture("assets/test_image.png");
  testImage2Texture = app->getTextureAssetManager().loadTexture("assets/test_image2.png");

  auto& renderer = app->getRenderer();
  main3DCamera= new tz::Camera(Eigen::Vector3f(30 ,15, 15), Eigen::Vector3f(0, 0, 0), tz::CameraType::Perspective);
  main3DScene = new tz::Scene(*main3DCamera, renderer, app->getTextRenderer());
  uiCamera = new tz::Camera(Eigen::Vector3f(0, 0, 4), {0, 0, 0}, tz::CameraType::Ortho);
  uiScene = new tz::Scene(*uiCamera, renderer, app->getTextRenderer());

  app->addScene("uiScene", uiScene, 1);
  app->addScene("main3DScene", main3DScene, 2);

  auto buttonNode = new tz::SceneNode({{500, 300, -3}, {128, 128, 1}}, tz::PrimitiveGeometryType::Quad);
  uiScene->addNode(*buttonNode);

  titleFont = app->getTextRenderer().createFont("assets/consolab.ttf", 58);
  smallUIFont = app->getTextRenderer().createFont("assets/consola.ttf", 14);

}

void gatherInput(const tz::input::SDL2InputSystem& inputSystem)
{
  // TODO
}

void doFrame(tz::App* app)
{

  auto& immCmdProc = app->getImmediateCommandProcessor();

  if (tz::input::SDL2InputSystem::getInstance().isMouseButtonClicked(tz::input::MouseButton::LEFT))
  {
    std::cout << "left mb clicked" << std::endl;
  }

  // This allows us to "see" our scene through a camera in a 3d world
  // and place objects in world coordinates.
  immCmdProc.activate3DCamera(Eigen::Vector3f(30 ,15, 15), Eigen::Vector3f(0, 0, 0));
  for (int i = 0; i < 8; i++) {
    immCmdProc.renderQuad({Eigen::Vector3f(-4 + i * 1.2, 0, 0)});
  }

  immCmdProc.renderCube({Eigen::Vector3f(.5, 3, 2 ), Eigen::Vector3f(1, 6, 4)});
  immCmdProc.renderCube({Eigen::Vector3f(-2.5, 1.5, 2), Eigen::Vector3f(1, 3, 4)});
  for (int i = 0; i < 5; i++) {
    for (int z = 0; z < 5; z++) {
      immCmdProc.renderCube({Eigen::Vector3f(-5 + i * 1.5, 0, -5 + z * 1.5), Eigen::Vector3f(.1, .01, .1)});
    }

  }

  // This allows us to place our objects in screen space coordinates
  // and render our objects accordingly.
  immCmdProc.activateUICamera(Eigen::Vector3f(0, 00, 4));
  immCmdProc.renderQuad({Eigen::Vector3f(100, 100, 0.2), Eigen::Vector3f(48, 48, 1)});

  static float mover = 24;
  static float dir = 1;
  if (tz::input::SDL2InputSystem::getInstance().isKeyPressed(tz::input::KeyCode::D)) {
    mover += 1;
  }
  if (tz::input::SDL2InputSystem::getInstance().isKeyPressed(tz::input::KeyCode::A)) {
    mover -= 1;
  }

  if (mover > 616 || mover < 0 ) {
    dir *= -1;
  }
  immCmdProc.renderQuad({Eigen::Vector3f(24 + mover, 24, 0.2), Eigen::Vector3f(48, 48, 1)});

  namespace rv = tz::render::vulkan;
  immCmdProc.renderQuad({Eigen::Vector3f(500, 250, -2), Eigen::Vector3f(64, 64, 1)},
                  tz::RenderHints{.materialType = rv::MaterialType::DiffuseNormal,
                                            .vertexShaderType =rv::VertexShaderType::Static,
                                            .texture = testImageTexture });

  for (int i = 0; i < 12; i++) {
    immCmdProc.renderQuad({Eigen::Vector3f(16 + (mover*1.2), 50 + i * 45, 0.2), Eigen::Vector3f(32, 32, 1)},
                    tz::RenderHints{.materialType = rv::MaterialType::DiffuseNormal,
                                    .vertexShaderType = rv::VertexShaderType::Static,
                                    .texture = testImage2Texture });
  }

  static int frame = 0;
  // Just limiting our "framecounter" here to avoid unwanted text-buffer-creation explosion..
  // this is just temporary demo code...
  frame++;
  frame = frame % 100;

  //app->getTextRenderer().create({{8, 8, 0.5}}, "Frame: " + std::to_string(frame));
  app->getImmediateCommandProcessor().renderText("Frame: " + std::to_string(frame), *smallUIFont, {{8, 8, 0.5}});
  app->getImmediateCommandProcessor().renderText("SWARMS", *titleFont, {{8, 400, 0.5}});

}

void runApp()
{
  auto app = tz::App(800, 600, "swarms");
  initialize(&app);

  app.setUpdateFunction(doFrame);
  //app.setInputListenerFunc(gatherInput);
  app.run();
}


int main(int argc, char* argv[])
{
  runApp();

  return 0;
}