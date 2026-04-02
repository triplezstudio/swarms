
module;
#include <SDL2/SDL.h>
#include <iostream>
#include <functional>

module app;

import windowing;
import render.base;

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







