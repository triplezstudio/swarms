module;

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL_syswm.h>
#include <SDL_vulkan.h>

#include <iostream>
#include <vector>

#include <stdexcept>



module app;


import :sdl2;

import render.vulkan;



namespace sdl2 {



int SDL2App::decideRenderBackend() {
  // TODO: read configfile and or env var etc.
  // For now we just set PpenGL as the default
  return SDL_WINDOW_OPENGL;
}

// TODO move to app_sdl2_vulkan.cpp
void SDL2App::doVulkanFrame()
{

}



void SDL2App::init()
{
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    throw std::runtime_error("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
  }

  renderBackendFlag = decideRenderBackend();

  switch (renderBackendFlag) {
    case SDL_WINDOW_VULKAN: initVulkanApp(); break;
    case SDL_WINDOW_OPENGL: initOpenGLApp(); break;
  }

  // Wait 3 seconds so you can actually see the window
  SDL_Delay(3000);

  SDL_Event event;
  bool run = true;

}

void SDL2App::doFrame() {

  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    if (event.type == SDL_QUIT)
    {

    }
  }

  switch(renderBackendFlag)
  {
    // TODO measure frame time
    case SDL_WINDOW_OPENGL: doGLFrame(); break;
    case SDL_WINDOW_VULKAN: doVulkanFrame(); break;

  }
}



client_common::NativeHandles SDL2App::getNativeHandles()
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

render::Renderer* SDL2App::getRenderer()
{
  return renderer;
}

}


