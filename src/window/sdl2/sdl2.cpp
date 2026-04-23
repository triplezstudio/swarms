
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL_syswm.h>

#include <vector>

#include <SDL_vulkan.h>
#include <common.hh>
#include <sdl2.hh>
#include <stdexcept>
#include <iostream>

namespace tz {

SDL2WindowSystem::SDL2WindowSystem()
{
  init();
}

void SDL2WindowSystem::pollEvents()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    if (event.type == SDL_QUIT)
    {
      // TODO handle graceful shutdown
      exit(0);
    }
  }
}

void SDL2WindowSystem::present()
{
  if (windowDesc.api == WindowDesc::GraphicsAPI::OpenGL)
  {
    SDL_GL_SwapWindow(window);
  }

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
    [this](GraphicsInstance& inst, WindowDesc desc) -> GraphicsSurface {
      return createSurface(inst, desc);
    }, 
    [this](int *width, int *height) {
      SDL_Vulkan_GetDrawableSize(this->window, width, height);
    }};

}

GraphicsSurface tz::SDL2WindowSystem::createSurface(GraphicsInstance& instance, WindowDesc desc) {
  if (desc.api == WindowDesc::GraphicsAPI::Vulkan)
  {
    VkInstance vkInst = reinterpret_cast<VkInstance>(instance.handle);
    VkSurfaceKHR rawSurface;
    if (!SDL_Vulkan_CreateSurface(window, vkInst, &rawSurface))
    {
      std::cerr << "surface error: " << std::string(SDL_GetError()) << std::endl;
      throw std::runtime_error("Could not create surface!" + std::string(SDL_GetError()));
    }
    return {rawSurface};
  }
  else {
    // We only support surface creation for vulkan now.
    return {};
  }

}

client_common::NativeHandles SDL2WindowSystem::getNativeHandles()
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



}


