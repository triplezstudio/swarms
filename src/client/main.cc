
#include <Eigen/Dense>
#include <app.hh>
#include <input.hh>
#include <iostream>
#include <render_helpers.hh>
#include <scene.hh>
#include <text_render.hh>
#include <ui.hh>
#include <vulkan_renderer.hh>

uint32_t testImageTexture              = 0;
uint32_t testImage2Texture             = 0;
tz::render::TextRenderer *textRenderer = nullptr;
tz::Scene *main3DScene                 = nullptr;
tz::Scene *uiScene                     = nullptr;
tz::Camera *main3DCamera               = nullptr;
tz::Camera *uiCamera                   = nullptr;
tz::render::Font *titleFont            = nullptr;
tz::render::Font *smallUIFont          = nullptr;
tz::input::SDLInputSystem *inputSystem = nullptr;
tz::UISystem *mainUISystem             = nullptr;

void initialize(tz::App *app)
{
  testImageTexture  = app->getTextureAssetManager().loadTexture("assets/test_image.png");
  testImage2Texture = app->getTextureAssetManager().loadTexture("assets/test_image2.png");

  auto &renderer = app->getRenderer();
  main3DCamera   = new tz::Camera(Eigen::Vector3f(30, 15, 15),
                                Eigen::Vector3f(0, 0, 0),
                                tz::CameraType::Perspective);
  main3DScene    = new tz::Scene(*main3DCamera, renderer, app->getTextRenderer());
  uiCamera       = new tz::Camera(Eigen::Vector3f(0, 0, 4), {0, 0, 0}, tz::CameraType::Ortho);
  uiScene        = new tz::Scene(*uiCamera, renderer, app->getTextRenderer());

  app->addScene("uiScene", uiScene, 1);
  app->addScene("main3DScene", main3DScene, 2);

  auto buttonNode = new tz::SceneNode({{500, 300, -3}, {128, 128, 1}},
                                      tz::PrimitiveGeometryType::Quad);
  uiScene->addNode(*buttonNode);

  titleFont   = app->getTextRenderer().createFont("assets/consolab.ttf", 58);
  smallUIFont = app->getTextRenderer().createFont("assets/consola.ttf", 14);

  inputSystem = &app->getInputSystem();

  mainUISystem = &app->createUISystem(0, 0, 600, 400);
  auto& miniWindow = mainUISystem->createWidget(nullptr);
  miniWindow.move(10, 10);
  miniWindow.resize(200, 400 );

  auto &button = mainUISystem->createButton(&miniWindow);
  button.setText("Press me!");
  button.move(10, 10);
  button.resize(128, 48);
}

void doFrame(tz::App *app)
{
  namespace rv     = tz::render::vulkan;
  auto &immCmdProc = app->getImmediateCommandProcessor();

  // This allows us to "see" our scene through a camera in a 3d world
  // and place objects in world coordinates.
  immCmdProc.activate3DCamera(Eigen::Vector3f(30, 15, 15), Eigen::Vector3f(0, 0, 0));
  std::vector<tz::Transform> transforms;
  for (int i = 0; i < 10; i++)
  {
    transforms.push_back({Eigen::Vector3f(-7 + i * 1.2, 0, 0)});
  }
  immCmdProc.renderQuads(transforms,
                         tz::RenderHints{.materialType     = rv::MaterialType::SingleColor,
                                         .vertexShaderType = rv::VertexShaderType::Static,
                                         .color            = {0.9, 0.0, 0.0, 1}});

  immCmdProc.renderQuads({{Eigen::Vector3f(8, 0, -4)}},
                         tz::RenderHints{.materialType     = rv::MaterialType::SingleColor,
                                         .vertexShaderType = rv::VertexShaderType::Static,
                                         .color            = {0.9, 0.9, 0.0, 1}});
  immCmdProc.renderCubes({{Eigen::Vector3f(-2.5, 1.5, 2), Eigen::Vector3f(1, 3, 4)}});

  std::vector<tz::Transform> cubeTransforms;
  for (int i = 0; i < 10; i++)
  {
    for (int z = 0; z < 10; z++)
    {
      cubeTransforms.push_back(
        {Eigen::Vector3f(-5 + i * 1.5, 0, -5 + z * 1.5), Eigen::Vector3f(1, 1.5, 1)});
    }
  }
  immCmdProc.renderCubes(cubeTransforms,
                         tz::RenderHints{.materialType     = rv::MaterialType::SingleColor,
                                         .vertexShaderType = rv::VertexShaderType::Static,
                                         .color            = {0.1f, 0.2f, 0.3f, 1}});

  // This allows us to place our objects in screen space coordinates
  // and render our objects accordingly.
  immCmdProc.activateUICamera(Eigen::Vector3f(0, 00, 4));
  immCmdProc.renderQuads({{Eigen::Vector3f(100, 100, 0.2), Eigen::Vector3f(48, 48, 1)}},
                         tz::RenderHints{.materialType     = rv::MaterialType::SingleColor,
                                         .vertexShaderType = rv::VertexShaderType::Static,
                                         .color            = {0.0, 0.9, 0.0, 1}});

  static float mover = 24;
  static float dir   = 1;
  if (tz::input::SDLInputSystem::getInstance().isKeyPressed(tz::input::KeyCode::D))
  {
    mover += 1;
  }
  if (tz::input::SDLInputSystem::getInstance().isKeyPressed(tz::input::KeyCode::A))
  {
    mover -= 1;
  }

  if (mover > 616 || mover < 0)
  {
    dir *= -1;
  }
  immCmdProc.renderQuads({{Eigen::Vector3f(24 + mover, 24, 0.2), Eigen::Vector3f(48, 48, 1)}},
                         tz::RenderHints{.materialType     = rv::MaterialType::SingleColor,
                                         .vertexShaderType = rv::VertexShaderType::Static,
                                         .color            = {0, 0.5, 0, 1}});

  std::vector<tz::Transform> transformX = {
    {Eigen::Vector3f(500, 250, -2), Eigen::Vector3f(64, 64, 1)}};
  immCmdProc.renderQuads(transformX,
                         tz::RenderHints{.materialType     = rv::MaterialType::DiffuseNormal,
                                         .vertexShaderType = rv::VertexShaderType::Static,
                                         .texture          = testImageTexture});

  std::vector<tz::Transform> transforms2;
  for (int i = 0; i < 12; i++)
  {
    transforms2.push_back(
      {Eigen::Vector3f(16 + (mover * 1.2), 50 + i * 45, 0.2), Eigen::Vector3f(32, 32, 1)});
  }

  immCmdProc.renderQuads(transforms2,
                         tz::RenderHints{.materialType     = rv::MaterialType::DiffuseNormal,
                                         .vertexShaderType = rv::VertexShaderType::Static,
                                         .texture          = testImage2Texture,
                                         .color            = {0.2f, 0.65f, 0.5, 1}});

  static int frame = 0;
  // Just limiting our "framecounter" here to avoid unwanted text-buffer-creation explosion..
  // this is just temporary demo code...
  frame++;
  frame = frame % 100;

  app->getImmediateCommandProcessor().renderText("Frame: " + std::to_string(frame),
                                                 *smallUIFont,
                                                 {{8, 8, 0.5}},
                                                 {0.5, 0.1, 0.1, 1});
  app->getImmediateCommandProcessor().renderText("SWARMS",
                                                 *titleFont,
                                                 {{8, 400, 0.5}},
                                                 {0, 0.9, 0, 1});
}

void runApp()
{
  auto app = tz::App(800, 600, "swarms");
  initialize(&app);

  app.addUpdateListener(doFrame);
  //app.setInputListenerFunc(gatherInput);
  app.run();
}

int main(int argc, char *argv[])
{
  runApp();

  return 0;
}