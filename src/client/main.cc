
#include <Eigen/Dense>
#include <render.hh>
#include <vulkan_renderer.hh>
#include <sdl2.hh>
#include <cmath>
#include <app.hh>


void doFrame(tz::App* app)
{

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





}

void runApp()
{
  auto app = tz::App();
  app.setUpdateFunction(doFrame);
  app.run();
}


int main(int argc, char* argv[])
{
  //runDemo();
  runApp();

  return 0;
}