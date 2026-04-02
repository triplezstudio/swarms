
#include <Eigen/Dense>
import app;
import render.opengl;
import windowing.sdl2;

/**
 * This function demonstrates different "usecases"
 * and options on how to use the clientside apis!
 */
void runDemo()
{

  auto renderer = new tz::OpenGLRenderer();
  auto ws = new tz::SDL2WindowSystem();
  auto winDesc = renderer->getRequiredWindowDesc();
  auto window = ws->createWindow(winDesc);
  renderer->init(window);

  while (true)
  {
    ws->pollEvents();
    renderer->clearScreen();
    ws->present();
  }

}


int main(int argc, char* argv[])
{

  runDemo();



  return 0;
}