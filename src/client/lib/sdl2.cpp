module;
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL_syswm.h>
#include <SDL_vulkan.h>

#include <iostream>
#include <vector>

#include <stdexcept>



module windowing.sdl2;

import common;

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

    }
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

//void SDL2WindowSystem::doFrame() {
//
//  SDL_Event event;
//  while (SDL_PollEvent(&event))
//  {
//    if (event.type == SDL_QUIT)
//    {
//
//    }
//  }
//
//
//}

tz::Window* tz::SDL2WindowSystem::createWindow(tz::WindowDesc desc)
{
  int windowFlags = SDL_WINDOW_SHOWN;
  if (desc.api == tz::WindowDesc::GraphicsAPI::OpenGL)
  {
     windowFlags |= SDL_WINDOW_OPENGL;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow("swarms v0.0.1",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              640,
                              480,
                              windowFlags);

    SDL_GL_CreateContext(window);

  }

  return new tz::Window { getNativeHandles().window };

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


