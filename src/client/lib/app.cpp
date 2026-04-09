#include <SDL2/SDL.h>
#include <iostream>
#include <functional>

#include "app.hh"
#include "window_system.hh"
#include "render.hh"

namespace tz
{

  App::App(tz::WindowSystem* windowSystem, tz::Renderer* renderer) : windowSystem(windowSystem),
  renderer(renderer)
{

}

void tz::App::run()
{
  while (true)
  {
    //windowSystem->doFrame();
    updateFrameListeners(16.66f);
  }

}

void App::addFrameListener(tz::FrameListener frameListenerFunction)
{
  frameListeners.push_back(frameListenerFunction);
}

void App::updateFrameListeners(float frameTime)
{
  for (auto& frameListenerFunc : frameListeners) {
    frameListenerFunc(frameTime);
  }
}

}







