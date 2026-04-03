
#include <Eigen/Dense>
import app;
import render.opengl;
import windowing.sdl2;


void runActiveLoopDemo()
{

  auto renderer = new tz::OpenGLRenderer();
  auto ws = new tz::SDL2WindowSystem();
  auto winDesc = renderer->getRequiredWindowDesc();
  auto window = ws->createWindow(winDesc);
  renderer->init(window);

  while (true)
  {
    ws->pollEvents();
    renderer->beginFrame();
    renderer->clearScreen();
    renderer->beginDraw(tz::PrimitiveType::Triangles);
    renderer->emitPosition({-0.5, 0.5, 0});
    renderer->emitPosition({-0.5, -0.5, 0});
    renderer->emitPosition({0.2, -0.5, 0});

    renderer->emitPosition({-0.4, 0.5, 0});
    renderer->emitPosition({0.3, -0.5, 0});
    renderer->emitPosition({0.3, 0.5, 0});
    renderer->endDraw();
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