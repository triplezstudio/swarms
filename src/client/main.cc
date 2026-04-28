#include <Eigen/Dense>
#include <app.hh>


uint32_t testImageTexture = 0;
uint32_t testImage2Texture = 0;

void initialize(tz::App* app)
{
  testImageTexture = app->createTexture("assets/test_image.png");
  testImage2Texture = app->createTexture("assets/test_image2.png");
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
  app->activate3DCamera(Eigen::Vector3f(0,10, 20), Eigen::Vector3f(0, 0, 0));
  for (int i = 0; i < 3; i++) {
    app->renderQuad({Eigen::Vector3f(0 + i * 1.2, 0, -10)});
  }

  app->renderCube({Eigen::Vector3f(.5, 0, 0), Eigen::Vector3f(1, 1, 1)});
   app->renderCube({Eigen::Vector3f(1.5, 0, -1.5), Eigen::Vector3f(1, 1, 1)});
  for (int i = 0; i < 5; i++) {
    for (int z = 0; z < 5; z++) {
       app->renderCube({Eigen::Vector3f(-5 + i * 1.5, 0, -5 + z * 1.5), Eigen::Vector3f(.1, .01, .1)});
    }
  
  }

  // This allows us to place our objects in screen space coordinates
  // and render our objects accordingly.
  app->activateUICamera(Eigen::Vector3f(0, 00, 4));
  app->renderQuad({Eigen::Vector3f(100, 100, 0.2), Eigen::Vector3f(48, 48, 1)});

  static float mover = 24;
  static float dir = 1;
  mover += 0.1 * dir;
  if (mover > 616 || mover < 0 ) {
    dir *= -1;
  }
  app->renderQuad({Eigen::Vector3f(24 + mover, 24, 0.2), Eigen::Vector3f(48, 48, 1)});

  app->renderQuad({Eigen::Vector3f(500, 250, -2), Eigen::Vector3f(64, 64, 1)},
                  tz::RenderHints{.materialType = tz::MaterialType::DiffuseNormal,
                                            .vertexShaderType =tz::VertexShaderType::Static,
                                            .texture = testImageTexture });

  for (int i = 0; i < 12; i++) {
    app->renderQuad({Eigen::Vector3f(16 + (mover*1.2), 50 + i * 45, 0.2), Eigen::Vector3f(32, 32, 1)},
                    tz::RenderHints{.materialType = tz::MaterialType::DiffuseNormal,
                                    .vertexShaderType =tz::VertexShaderType::Static,
                                    .texture = testImage2Texture });
  }

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