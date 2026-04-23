#include <SDL2/SDL.h>
#include <iostream>
#include <functional>

#include "app.hh"
#include <render.hh>
#include <vulkan_renderer.hh>
#include <window_system.hh>
#include <sdl2.hh>

namespace tz
{

  App::App()
{
  renderer = reinterpret_cast<tz::Renderer *>(new tz::render::vulkan::VulkanRenderer());
  windowSystem = new tz::SDL2WindowSystem();
  auto winDesc = renderer->getRequiredWindowDesc();
  auto window = windowSystem->createWindow(winDesc);
  renderer->init(window);

}

void tz::App::run()
{
  while (true)
  {
    //windowSystem->doFrame();
    updateFrameListeners(16.66f);
  }

}

void App::setUpdateFunction(tz::FrameListener frameListener)
{
  frameListeners.push_back(frameListener);
}

void App::updateFrameListeners(float frameTime)
{
  windowSystem->pollEvents();
  for (auto& frameListenerFunc : frameListeners) {
    frameListenerFunc(this);
  }
  windowSystem->present();
}
float App::getLastFrameTime()
{
  return 16.667f;
}
void App::renderCube(Eigen::Vector3f position) {

}

}







