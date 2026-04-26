
#include <Eigen/Dense>
#include <render.hh>
#include <vulkan_renderer.hh>
#include <sdl2.hh>
#include <cmath>
#include <app.hh>


tz::Texture* testImageTexture = nullptr;

void initialize(tz::App* app)
{
  testImageTexture = app->createTexture("assets/test_image.png");
}


void doFrame(tz::App* app)
{

  static bool firstTime = true;
  if (firstTime)
  {
    initialize(app);
    firstTime = false;
  }
  // This allows us to "see" our scene through a camera in a 3d world
  // and place objects in world coordinates.
  app->activate3DCamera(Eigen::Vector3f(0, 0, 25), Eigen::Vector3f(0, 0, 0));
  for (int i = 0; i < 3; i++) {
    app->renderQuad({Eigen::Vector3f(0 + i * 1.2, 0, 0)});
  }

  // This allows us to place our objects in screen space coordinates
  // and render our objects accordingly.
  app->activateUICamera();
  app->renderQuad({Eigen::Vector3f(100, 100, 0.2), Eigen::Vector3f(48, 48, 1)});

  static float mover = 24;
  static float dir = 1;
  mover += 0.1 * dir;
  if (mover > 616 || mover < 0 ) {
    dir *= -1;
  }
  app->renderQuad({Eigen::Vector3f(24 + mover, 24, 0.2), Eigen::Vector3f(48, 48, 1)});

  app->renderQuad({Eigen::Vector3f(400, 200, 0.2), Eigen::Vector3f(64, 64, 1)},
                  tz::RenderHints{.materialType = tz::MaterialType::DiffuseNormal,
                                            .vertexShaderType =tz::VertexShaderType::Static,
                                            .texture = testImageTexture });
}

void runApp()
{
  auto app = tz::App();

  app.setUpdateFunction(doFrame);
  app.run();
}


int main(int argc, char* argv[])
{
  runApp();

  return 0;
}