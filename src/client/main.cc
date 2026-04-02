

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
  auto app = tz::App(ws, renderer);

#define USE_FRAME_CALLBACK
#ifdef USE_FRAME_CALLBACK
  // This mode lets the framework manage the game loop.
  // The game registers a callback and is called
  // each frame.
  app.addFrameListener([renderer](float frameTime) {

    renderer->beginDraw(tz::PrimitiveType::Triangles);
    renderer->emitPosition({-0.5, 0.5, 0});
    renderer->emitPosition({-0.5, -0.5, 0});
    renderer->emitPosition({0.5, -0.5, 0});
    renderer->endDraw();

  });
  app.run();

#endif

#ifdef USE_ACTIVE_LOOP
  // This mode shows how the game may also actively drive the game loop
  // itself, and no callback is necessary.

#endif
}


int main(int argc, char* argv[])
{





  return 0;
}