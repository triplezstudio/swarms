#include <SDL2/SDL.h>
#include <SDL_syswm.h>

#include <vector>

#include "src/input/include/input.hh"
#include <../../client_common/include/common.hh>
#include <SDL_vulkan.h>
#include <iostream>
#include <sdl2.hh>
#include <stdexcept>

namespace tz {

SDL2WindowSystem::SDL2WindowSystem()
{
  init();
}

static bool isInputEvent(SDL_Event event)
{
  return event.type == SDL_KEYDOWN || event.type == SDL_KEYUP || event.type == SDL_MOUSEBUTTONDOWN
  || event.type == SDL_MOUSEBUTTONUP;
}


void SDL2WindowSystem::pollEvents()
{
  frameInputEvents.clear();
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    if (isInputEvent(event))
    {
      frameInputEvents.push_back(event);
    }

    if (event.type == SDL_QUIT)
    {
      // TODO handle graceful shutdown
      exit(0);
    }
  }
}



void SDL2WindowSystem::present()
{

}

void SDL2WindowSystem::init()
{
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    throw std::runtime_error("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
  }


  // Wait 3 seconds so you can actually see the window
  SDL_Delay(3000);

  SDL_Event event;
  bool run = true;

}

tz::Window* tz::SDL2WindowSystem::createWindow(tz::WindowDesc desc)
{
  this->windowDesc = desc;
  int windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

  window = SDL_CreateWindow("swarms v0.0.1",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            640,
                            480,
                          windowFlags);
  

  return new tz::Window { getNativeHandles().window, 640, 480, 
    [this](GraphicsInstance& inst) -> GraphicsSurface {
      return createSurface(inst);
    }, 
    [this](int *width, int *height) {
      SDL_Vulkan_GetDrawableSize(this->window, width, height);
    }};

}

GraphicsSurface tz::SDL2WindowSystem::createSurface(GraphicsInstance& instance) {
  auto vkInst = reinterpret_cast<VkInstance>(instance.handle);
  VkSurfaceKHR rawSurface;
  if (!SDL_Vulkan_CreateSurface(window, vkInst, &rawSurface))
  {
    std::cerr << "surface error: " << std::string(SDL_GetError()) << std::endl;
    throw std::runtime_error("Could not create surface!" + std::string(SDL_GetError()));
  }
  return {rawSurface};


}

tz::NativeHandles SDL2WindowSystem::getNativeHandles()
{
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version); // Initialize version info

  if (SDL_GetWindowWMInfo(window, &wmInfo))
  {
#if defined(_WIN32)
    // Windows: connection is HINSTANCE, window is HWND
    return {(void *) wmInfo.info.win.hinstance, (void *) wmInfo.info.win.window, "windows"};

#elif defined(__linux__)
    // Linux: Check if we are running under X11 or Wayland
    if (wmInfo.subsystem == SDL_SYSWM_X11)
    {
      return {(void *) wmInfo.info.x11.display, (void *) (uintptr_t) wmInfo.info.x11.window, "x11"};
    }
    else if (wmInfo.subsystem == SDL_SYSWM_WAYLAND)
    {
      return {(void *) wmInfo.info.wl.display, (void *) wmInfo.info.wl.surface, "wayland"};
    }
#endif
  }

  return {nullptr, nullptr};
}

const std::vector<SDL_Event> SDL2WindowSystem::getFrameEvents() const
{
  return frameInputEvents;
}

}


