
module;
#include <SDL2/SDL.h>
#include <iostream>
#include <functional>

module app;

import :sdl2;

app::App::App()
{
  // TODO: read from config which backend to use?
  // Default is SDL2 for now
  impl = new sdl2::SDL2App();

}

render::Renderer *app::App::getRenderer()
{
  return impl->getRenderer();
}

void app::App::init()
{
  impl->init();
}

void app::App::run()
{
  while (true)
  {
    impl->doFrame();
    updateFrameListeners(16.66f);
  }

}

void app::App::addFrameListener(app::FrameListener frameListenerFunction)
{
  frameListeners.push_back(frameListenerFunction);
}

void app::App::updateFrameListeners(float frameTime)
{
  for (auto& frameListenerFunc : frameListeners) {
    frameListenerFunc(frameTime);
  }
}


